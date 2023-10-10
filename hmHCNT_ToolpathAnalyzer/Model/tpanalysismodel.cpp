#pragma warning( disable : 4996 ) // for warnings: due to compare of boost::uuids
#include "TPAnalysisModel.h"
// keys
#include <Commands\tpanalysiskeys.h>
// Document
#include <hcntcore/document.h>

#include <propertyservice/propertybag/minmaxvalidatort.h>

// warnings
#include <warningsservice/warningdefine.h>

#include <warningsservice/warningfactory.h>

#include <bgeo/typeofbgeotypesiterators.h>
#include <hcntcore/typeofreferencesystem.h>

#include <selectionservice/filter.h>
#include <selectionservice/filters.h>
#include <selectionservice/entityfilter.h>
#include <selectionservice/interactiveselector.h>
#include <selectionservice/selectionservice.h>
#include <selectionservice/entityfilterutilities.h>

#include <gui/gui.h>
#include <gui/guihandshake.h>

#include <commands/tp_listener.h>
#include <commands/tp_signalemitter.h>

#include <materials/gradient.h>
#include <hcNTLib/ConnectionHlpTT.h>


#include <..\..\..\INTERFACES\iBSys\icdCtPathEnums.h>
#include <Model\tpanalysismodel.h>

OM_BEGIN_WARNINGS_DEFINITION
DEFINE_WARNING ( kGeneralWarningTPAnalysis, "tp_analyzer.TPAnalysis", "General warning." )
OM_END_WARNINGS_DEFINITION
using namespace omsl::command;
using namespace omsl::selectionservice;
using namespace omsl::hmbroadcast;

#define TPOWNERID 11223344
static int s_defined = 0;

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            extern hcNTLib::HCNTToolTipConnection m_tt_cnct;

            void analyseData::reset ( int sel )
            {
                pSelectedTP [sel] = nullptr;
                m [sel].m_pAttr = nullptr;
                m [sel].m_pParGeomCast = nullptr;
                m [sel].m_pTpSeg = nullptr;
                m [sel].m_pTpSegCadEnt = nullptr;
                m [sel].m_pTpSegCast = nullptr;
                m [sel].m_origCadEnt = nullptr;
                m [sel].pointSizeVec.clear ();
                m [sel].pointVec.clear ();
                m [sel].segmentSizeVec.clear ();
                m [sel].typeVec.clear ();
                m [sel].viewVec.clear ();
                for ( int i = 0; i < dai_indexArraySize; i++ )
                    m [sel].specialIndex [i] = UNKNOWNINDEX;
                m [sel].specialIndex [analyseData::dai_Index1stTpEnt] = UNKNOWNINDEX;
                m [sel].specialIndex [analyseData::dai_Index2ndTpEnt] = UNKNOWNINDEX;
            }

            void analyseData::reset ()
            {

                pPnt_C2ndSegment = nullptr;
                pPnt_L1stSegment = nullptr;
                pPnt_L2ndSegment = nullptr;
                pPnt_R2ndSegment = nullptr;
                pPnt_R3rdSegment = nullptr;

                pToolpathMap.clear ();

                for ( int sel = MASTER_TP ; sel < ALL_SEL ; sel++ )
                { 
                    reset ( sel );
                }
            }

            OMSL_COMMANDMODEL_FACTORY ( TPAnalysis )
            {
                return new TPAnalysisModel ( pDocument );
            }

            TPAnalysisModel::TPAnalysisModel ( omsl::hcntcore::Document* pDocument )
            : level  (0)
            {
                s_defined++;

                m_enableSignalListening = false;
                m_callsm.clear ();
                s_data.reset ();

                m_waitForCadShell = false;
                m_pDocument = pDocument;
                s_data.m[L_SEL].m_compTPAMode = tpanalysis::cm_TPAFirst;
                s_data.m[R_SEL].m_compTPAMode = tpanalysis::cm_TPAFirst;

                omsl::hmbroadcast::RegisterSender ();
                omsl::hmbroadcast::RegisterListener ();

            }

            TPAnalysisModel::~TPAnalysisModel ()
            {
                s_defined--;

                m_waitForCadShell = true;
                SetupMotionColors ( 0 );
//                ResetGraphicsSize ();
//                OnEditToolpathChanged ( false ); // to close the workspace

                if ( s_defined == 0 )
                {
                    omsl::hmbroadcast::EmitTPSignal ( L"TPArunning", omsl::hmbroadcast::bcs_TPA_ResetLockInfo, nullptr, TPOWNERID, 0.0 );
                    omsl::hmbroadcast::EmitTPSignal ( L"bcs_TPA_end", omsl::hmbroadcast::bcs_TPA_end, nullptr, -1, 0.0 );
                }

                omsl::hmbroadcast::RemoveListener ();
                omsl::hmbroadcast::RemoveSender ();
                /*bool old =*/ (void)SetSignalListening ( false );
                s_data.reset ();
                if ( m_tt_cnct.IsConnected () )
                {
                    m_tt_cnct.clear_tooltips ();
                    m_tt_cnct.Disconnect ();
                }
            }

            void TPAnalysisModel::InternalDefineContext ()
            {
                SetSignalListening ( false );
                // interface to hm data and info
                // Context definition

                //AddEntityValue ( tpanalysis::UI_Toolpath, propertyservice::Applicable::kMandatory,
                //                 L"ToolPathEntity", 1 );
                AddEntityValue ( tpanalysis::UI_FirstEntities, propertyservice::Applicable::kOptional,
                                 L"ParGeomEntity:ParGeom:Point CompositeEntity.ParGeomEntity:ParGeom:Point", 1, 1 );
                AddEntityValue ( tpanalysis::UI_SecondEntities, propertyservice::Applicable::kOptional,
                                 L"ParGeomEntity:ParGeom:Point CompositeEntity.ParGeomEntity:ParGeom:Point", 1, 1 );
                //AddEntityValue ( tpanalysis::UI_RefPoint, propertyservice::Applicable::kOptional,
                //                 L"ParGeomEntity:ParGeom:Point CompositeEntity.ParGeomEntity:ParGeom:Point", 1, 1 );

                AddEntityValue ( tpanalysis::UI_LeftSliderPick, propertyservice::Applicable::kOptional,
                                 L"ParGeomEntity:ParGeom:Point CompositeEntity.ParGeomEntity:ParGeom:Point", 1, 1 );
                AddEntityValue ( tpanalysis::UI_RightSliderPick, propertyservice::Applicable::kOptional,
                                 L"ParGeomEntity:ParGeom:Point CompositeEntity.ParGeomEntity:ParGeom:Point", 1, 1 );

                Add ( tpanalysis::UI_Direction, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_Label, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_Vector, 1, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_Point, 2, propertyservice::Applicable::kMandatory );
                //Add ( tpanalysis::UI_Tool, 0, propertyservice::Applicable::kMandatory );

                Add ( tpanalysis::UI_FeedRate, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_ClearanceLevel, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_AutoUpdateMin, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_AutoUpdateMax, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_AutoUpdateLvl, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::useExactFeedrates, 0, propertyservice::Applicable::kMandatory );
                //                Add ( tpanalysis::UI_Orientation , 0, propertyservice::Applicable::kMandatory );
                //                Add ( tpanalysis::UI_Compensation, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_SpinBox_MoveNr, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_DblSldr_Limits, 0, propertyservice::Applicable::kMandatory );
                //                Add ( tpanalysis::DblSldr_Limits, 0 ); // , propertyservice::Applicable::kMandatory );
                                // set the first value
                Add ( tpanalysis::UI_RefSystemType, 0, propertyservice::Applicable::kMandatory );
                // set the first value
                Add ( tpanalysis::UI_Tab_General, 0, propertyservice::Applicable::kMandatory );
                // set the GUI output context
                SharedPtr<propertyservice::PropertyBag> pEmpty;
                Add ( tpanalysis::UI_AnalysisResult, pEmpty, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::UI_Settings, pEmpty, propertyservice::Applicable::kHardCoded );

                Add ( tpanalysis::DummyParameter, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::TabIndex, 0, propertyservice::Applicable::kMandatory );

                //                Add ( tpanalysis::ToolpathCounter, -1, propertyservice::Applicable::kMandatory );

                SetValueState ( tpanalysis::DummyParameter, propertyservice::Value::kWrong ); // make the command unapplyable

                // to react on viewmode changes
                //Add ( tpanalysis::viewModeParameter, 0, propertyservice::Applicable::kMandatory );

                // set the functionMode
                //Add ( tpanalysis::UI_Sel_TPA_Mode, 0, propertyservice::Applicable::kMandatory );
                //Add ( tpanalysis::UI_Sel_FRA_Mode, 0, propertyservice::Applicable::kMandatory );

                // set the geometrical output context
                Add ( tpanalysis::TGr_ReferenceSystem, propertyservice::Applicable::kHardCoded );
                //Add ( tpanalysis::TGr_ReferenceArrowS, propertyservice::Applicable::kHardCoded );
                //Add ( tpanalysis::TGr_ReferenceArrowE, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::TGr_ReferenceArrowC, propertyservice::Applicable::kHardCoded );

                Add ( tpanalysis::WorkplaneEnabled, propertyservice::Applicable::kHardCoded );
                //                Add ( tpanalysis::TGr_CompensationVectors, propertyservice::Applicable::kHardCoded );
//                Add ( tpanalysis::TGr_OrientationVectorsL, propertyservice::Applicable::kHardCoded );
//                Add ( tpanalysis::TGr_OrientationVectorsR, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::TGr_LabelVectors, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::TGr_TextPoints, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::TGr_Texts, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::TGr_MovementArcMarkers, propertyservice::Applicable::kHardCoded );
                //                Add ( tpanalysis::TGr_PreviewPoints_Ent1, propertyservice::Applicable::kHardCoded );
                //                Add ( tpanalysis::TGr_PreviewPoints_C, propertyservice::Applicable::kHardCoded );
                //                Add ( tpanalysis::TGr_PreviewPoints_Ent2, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::updateColorsTableAllowed, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::updateFeedrateDraw, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::updateSliderDraw, 0, propertyservice::Applicable::kMandatory );

                // measurements

                Add ( tpanalysis::Msr_DistPointsTexts, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::Msr_TextPoints, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::Msr_DistanceTexts, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::Msr_DistanceTextPoints, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::Msr_DistanceLine, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::Msr_DistPoints, propertyservice::Applicable::kHardCoded );

                // common settings

                Add ( tpanalysis::UI_Env_ShowAdditionalPathes, ( 1 << eCt_tp ), propertyservice::Applicable::kMandatory ); // todo get the default length from somewhere

// color settings

                Add ( tpanalysis::UI_feedrateLevels, DEFAULTSPEEDLVL, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_feedrateLevelsAuto, DEFAULTSPEEDLVL );

                Add ( tpanalysis::UI_feedrateValueMin, DEFAULTSPEEDMIN, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_feedrateValueMax, DEFAULTSPEEDMAX, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_colorMapMin_2, DEFAULTSPEEDMIN );
                Add ( tpanalysis::UI_colorMapMax_2, DEFAULTSPEEDMAX );
                Add ( tpanalysis::UI_colorMapCMax_3, DEFAULTSPEEDMAX );
                // limits
                Add ( tpanalysis::UI_DblSldr_Limits, propertyservice::Applicable::kMandatory ); // todo get the default length from somewhere     
                Add ( tpanalysis::UI_sliderValueMin, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_sliderValueMax, DEFAULTSEGMAX, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_sliderMin_2, 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_sliderMax_2, DEFAULTSEGMAX, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_sliderEnable, 0, propertyservice::Applicable::kMandatory );

                std::vector<int> speedClrIndexVec;
                std::vector<double> colorTable, speedTable;

                speedClrIndexVec.push_back ( 0 + GENERAL_COLOR_SIZE );
                speedClrIndexVec.push_back ( 1 + GENERAL_COLOR_SIZE );
                speedClrIndexVec.push_back ( 2 + GENERAL_COLOR_SIZE );
                speedClrIndexVec.push_back ( 3 + GENERAL_COLOR_SIZE );
                speedClrIndexVec.push_back ( 4 + GENERAL_COLOR_SIZE );
                speedClrIndexVec.push_back ( 5 + GENERAL_COLOR_SIZE );
                speedClrIndexVec.push_back ( 6 + GENERAL_COLOR_SIZE );
                speedClrIndexVec.push_back ( 7 + GENERAL_COLOR_SIZE );
                speedClrIndexVec.push_back ( 8 + GENERAL_COLOR_SIZE );
                speedClrIndexVec.push_back ( 9 + GENERAL_COLOR_SIZE );

                SharedPtr<omsl::materials::Gradient> pGradient = materials::Gradient::CreateBlueRedHSL ();
                pGradient->SetQuantization ( DEFAULTSPEEDLVL );

                colorTable.push_back ( 1. );
                colorTable.push_back ( 0. );
                colorTable.push_back ( 0. );

                for ( int i = DEFAULTSPEEDLVL - 2; i >= 1 /*- 2*/; i-- )
                {
                    double t = ( double ) i / ( double ) ( DEFAULTSPEEDLVL - 1 );//(double)(2 * (i + 1) - 1) / (double)(2 * nLevels + 1);
                    omsl::Rgb col = pGradient->GetColour ( t );

                    colorTable.push_back ( col.r );
                    colorTable.push_back ( col.g );
                    colorTable.push_back ( col.b );
                }

                colorTable.push_back ( 0. );
                colorTable.push_back ( 0. );
                colorTable.push_back ( 1. );

                speedTable.push_back ( DEFAULTSPEEDMAX );
                speedTable.push_back ( DEFAULTSPEEDMIN );
                Add ( tpanalysis::doubleSpeedVec, speedTable );
                Add ( tpanalysis::doubleClrVec, colorTable );
                Add ( tpanalysis::intClrIdxVec, speedClrIndexVec );
                Add ( tpanalysis::clrTblRoundingPrec, 0 ); // default
                Add ( tpanalysis::clrTblRoundingLimit, DEFAULTSPEEDMAX ); // default

                Add ( tpanalysis::numberOfDifferentFeedrates, DEFAULTSPEEDLVL + 10 ); // default
                Add ( tpanalysis::resetPickPointSelection, 0 ); // default

                Add ( tpanalysis::showLayer25, 0 ); // default

                Add ( tpanalysis::sl25_cb_min, 0 );
                Add ( tpanalysis::sl25_cb_max, 0 );
                Add ( tpanalysis::sl25_cb_lvl, 0 );
                Add ( tpanalysis::sl25_cb_clr, 0 );
                Add ( tpanalysis::sl25_in_min, 0. );
                Add ( tpanalysis::sl25_in_max, 0. );
                Add ( tpanalysis::sl25_in_clr, 0. );
                Add ( tpanalysis::sl25_in_lvl, 0 );

                if ( s_defined > 1 )
                    return;

                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnUpdateFeedrateDraw, tpanalysis::DummyParameter, tpanalysis::updateFeedrateDraw );


                //                Add ( tpanalysis::TGr_PreviewPoints_R3rd, propertyservice::Applicable::kHardCoded );

                                // Setup validators
                SetValidator ( tpanalysis::UI_RefSystemType, new propertyservice::MinMaxValidatorT<int> ( 0, 2 ) );
                //SetValidator ( tpanalysis::UI_Sel_TPA_Mode, new propertyservice::MinMaxValidatorT<int> ( tpanalysis::cm_TPAFirst, tpanalysis::cm_TPALast ) );

                SetValidator ( tpanalysis::UI_feedrateLevels, new propertyservice::MinMaxValidatorT<int> ( 3, ADDITIONAL_FEEDRATE_COLORLIMIT - 5 ) );
                //SetValidator ( tpanalysis::UI_feedrateLevelsAuto, new propertyservice::MinMaxValidatorT<int> ( 3, ADDITIONAL_FEEDRATE_COLORLIMIT-5 ) );
                //SetValidator ( tpanalysis::UI_sliderValueMin, new propertyservice::MinMaxValidatorT<int> ( 0, DEFAULTSEGMAX- MINIMUMSEGGAP ));
                //SetValidator ( tpanalysis::UI_sliderValueMax, new propertyservice::MinMaxValidatorT<int> ( MINIMUMSEGGAP, DEFAULTSEGMAX ) );

                //SetValidator ( tpanalysis::UI_Sel_FRA_Mode, new propertyservice::MinMaxValidatorT<int> ( cm_FRAFirst, cm_FRALast ) );
                // Connection to values
//                ConnectValueChange ( tpanalysis::UI_Sel_TPA_Mode, &TPAnalysisModel::OnModeTPAChange ); // point, movement ....

                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::CheckSelection, tpanalysis::DummyParameter, tpanalysis::UI_FirstEntities, tpanalysis::UI_SecondEntities );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::HighlightToolPath, tpanalysis::DummyParameter, tpanalysis::UI_FirstEntities, tpanalysis::UI_SecondEntities );

                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::CheckLeftPick, tpanalysis::DummyParameter, tpanalysis::UI_LeftSliderPick );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::CheckRightPick, tpanalysis::DummyParameter, tpanalysis::UI_RightSliderPick );

                // Rules

                ADD_COMMAND_STRENGTH_RULE ( TPAnalysisModel::ActivateFeedRateMin, tpanalysis::UI_feedrateValueMin, tpanalysis::UI_AutoUpdateMin );
                ADD_COMMAND_STRENGTH_RULE ( TPAnalysisModel::ActivateFeedRateMax, tpanalysis::UI_feedrateValueMax, tpanalysis::UI_AutoUpdateMax );
                ADD_COMMAND_STRENGTH_RULE ( TPAnalysisModel::ActivateColorLevel, tpanalysis::UI_feedrateLevels, tpanalysis::UI_AutoUpdateLvl );

                // with layer 25 all feedrate related fields need to be disabled and the values / states need to be stored / restored

                ADD_COMMAND_STRENGTH_RULE ( TPAnalysisModel::Activate_CB_Min_ColorSettings, tpanalysis::UI_AutoUpdateMin, tpanalysis::showLayer25 );
                ADD_COMMAND_STRENGTH_RULE ( TPAnalysisModel::Activate_CB_Max_ColorSettings, tpanalysis::UI_AutoUpdateMax, tpanalysis::showLayer25 );
                ADD_COMMAND_STRENGTH_RULE ( TPAnalysisModel::Activate_CB_Lvl_ColorSettings, tpanalysis::UI_AutoUpdateLvl, tpanalysis::showLayer25 );
                ADD_COMMAND_STRENGTH_RULE ( TPAnalysisModel::Activate_CB_Clr_ColorSettings, tpanalysis::UI_ClearanceLevel, tpanalysis::showLayer25 );

                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_Label, tpanalysis::DummyParameter, tpanalysis::UI_Label );
                //                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_Orientation , tpanalysis::DummyParameter, tpanalysis::UI_Orientation );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_Direction, tpanalysis::DummyParameter, tpanalysis::UI_Direction );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_Vector, tpanalysis::DummyParameter, tpanalysis::UI_Vector );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_Point, tpanalysis::DummyParameter, tpanalysis::UI_Point );
                //ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_Tool, tpanalysis::DummyParameter, tpanalysis::UI_Tool );
                //                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_MoveLimits  , tpanalysis::DummyParameter, tpanalysis::UI_MoveLimits );
                //                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_Compensation, tpanalysis::DummyParameter, tpanalysis::UI_Compensation );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_Feedrate, tpanalysis::DummyParameter, tpanalysis::UI_FeedRate );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_Clearance, tpanalysis::DummyParameter, tpanalysis::UI_ClearanceLevel );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_AutoUpdateMin, tpanalysis::DummyParameter, tpanalysis::UI_AutoUpdateMin );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_AutoUpdateMax, tpanalysis::DummyParameter, tpanalysis::UI_AutoUpdateMax );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChange_CB_AutoUpdateLvl, tpanalysis::DummyParameter, tpanalysis::UI_AutoUpdateLvl );

                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnSetupCoordinateSystem, tpanalysis::DummyParameter, tpanalysis::UI_RefSystemType );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChangeMoveNr, tpanalysis::UI_AnalysisResult, tpanalysis::UI_SpinBox_MoveNr );
//                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChangeReferencePoint, tpanalysis::DummyParameter, tpanalysis::UI_RefPoint );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::Compute, tpanalysis::UI_AnalysisResult, tpanalysis::UI_FirstEntities, tpanalysis::UI_SecondEntities );

                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnSetupGeneralTab, tpanalysis::TabIndex, tpanalysis::UI_Tab_General );

                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnChangeRangeSlider, tpanalysis::DummyParameter, tpanalysis::updateSliderDraw );


                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnColorStepCount, tpanalysis::DummyParameter, tpanalysis::UI_feedrateLevels );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnColorTargetMax, tpanalysis::DummyParameter, tpanalysis::UI_feedrateValueMax );
                ADD_COMMAND_VALUE_RULE ( TPAnalysisModel::OnColorTargetMin, tpanalysis::DummyParameter, tpanalysis::UI_feedrateValueMin );

                SharedPtr<hcntcore::CadEntity> pEmptyEnt;
                CadEntitiesIt_T pEmptyEnts;

//                SetValue ( tpanalysis::UI_RefPoint, pEmptyEnt );
                SetValue ( tpanalysis::UI_FirstEntities, pEmptyEnts );
                SetValue ( tpanalysis::UI_SecondEntities, pEmptyEnts );

                SetValue ( tpanalysis::UI_LeftSliderPick, pEmptyEnt );
                SetValue ( tpanalysis::UI_RightSliderPick, pEmptyEnt );

                //SetValue ( tpanalysis::UI_Toolpath, pEmptyEnts );

                //SetValue ( tpanalysis::UI_Sel_TPA_Mode, GetValue ( tpanalysis::UI_Sel_TPA_Mode ) );
                //SetValue ( tpanalysis::UI_Sel_FRA_Mode, GetValue ( tpanalysis::UI_Sel_FRA_Mode ) );

                m_withUpdate = false;
                SetValue ( tpanalysis::UI_RefSystemType, GetValue ( tpanalysis::UI_RefSystemType ) );
                m_withUpdate = true;

                SetValue ( tpanalysis::UI_FeedRate, GetValue ( tpanalysis::UI_FeedRate ) );
                SetValue ( tpanalysis::UI_ClearanceLevel, GetValue ( tpanalysis::UI_ClearanceLevel ) );
                SetValue ( tpanalysis::UI_AutoUpdateMin, GetValue ( tpanalysis::UI_AutoUpdateMin ) );
                SetValue ( tpanalysis::UI_AutoUpdateMax, GetValue ( tpanalysis::UI_AutoUpdateMax ) );
                SetValue ( tpanalysis::UI_AutoUpdateLvl, GetValue ( tpanalysis::UI_AutoUpdateLvl ) );
                SetValue ( tpanalysis::UI_Vector, GetValue ( tpanalysis::UI_Vector ) );
                SetValue ( tpanalysis::UI_Point, GetValue ( tpanalysis::UI_Point ) );
                SetValue ( tpanalysis::UI_Direction, GetValue ( tpanalysis::UI_Direction ) );
                SetValue ( tpanalysis::UI_Label, GetValue ( tpanalysis::UI_Label ) );
                //                SetValue ( tpanalysis::UI_Orientation, GetValue ( tpanalysis::UI_Orientation ) );
                //                SetValue ( tpanalysis::UI_Compensation, GetValue ( tpanalysis::UI_Compensation ) );

                //SetValue ( tpanalysis::UI_Env_ShowAdditionalPathes, GetValue ( tpanalysis::UI_Env_ShowAdditionalPathes ) );

                SetValue ( tpanalysis::WorkplaneEnabled, false );
                ActivateSlider ( 0 );

                s_data.m[MASTER_TP].specialIndex [analyseData::dai_SpinBoxValue] = ( int ) GetValue ( tpanalysis::UI_SpinBox_MoveNr );

//                SetupPropertyBagForSettings ();

                omsl::hmbroadcast::SetModelToListener ( this, false );

                omsl::hmbroadcast::EmitTPSignal ( L"bcs_TPA_start", omsl::hmbroadcast::bcs_TPA_start, nullptr, -1, 0.0 );
                omsl::hmbroadcast::EmitTPSignal ( L"TPArunning", omsl::hmbroadcast::bcs_TPA_SetLockInfo, nullptr, TPOWNERID, 0.0 );
                omsl::hmbroadcast::EmitTPSignal ( L"bcs_TTIP_request", omsl::hmbroadcast::bcs_TTIP_request, nullptr, -1, 0.0 );
                omsl::hmbroadcast::EmitTPSignal ( L"TPCMPingAllTP", omsl::hmbroadcast::bcs_TPCMShow, nullptr, -1, 0.0 ); // to fill the tpmap

                double tol = 0.001;
                omsl::geom::Globals* g = omsl::geom::GetGlobals ();
                if ( g )
                {
                    for ( int i = 0; i < 5; i++ )
                    {
                        switch ( i )
                        {
                            case 0: tol = g->AngleTol (); break;
                            case 1: tol = g->Tol (); break;
                            case 2: tol = g->Tol2d (); break;
                            case 3: tol = g->TessellationToleranceLimit (); break;
                            case 4: tol = g->MeshVsSurfaceTesselationTolerance (); break;
                        }
                    }
                }

                SetSignalListening ( true );

                // refresh first color map
                {
                    SetupExtremaToUI ( 0x0020 );
                    //SetupTemporaryTPGraphics ( omsl::hmbroadcast::bcs_TPCMShowGV, vm );
                    //SetupTemporaryTPGraphics ( omsl::hmbroadcast::bcs_TPCMShowPt, vm );
                }
                { // refresh points and vectors
                }
            }

            void TPAnalysisModel::InternalApply ()
            {
                callStackString ( L"InternalApply", 1 );

//                int i = 0;
                m_withUpdate = true;
                callStackString ( L"InternalApply", -1 );
            }

            void TPAnalysisModel::ActivateSlider ( const bool on )
            {
                propertyservice::Applicable::Strength retVal = on == true ? propertyservice::Applicable::kMandatory : propertyservice::Applicable::kInactive;

                SetStrength ( tpanalysis::UI_LeftSliderPick, retVal );
                SetStrength ( tpanalysis::UI_RightSliderPick, retVal );
                SetValue ( tpanalysis::UI_sliderValueMax, GetValue ( tpanalysis::UI_sliderMax_2 ) );
                SetValue ( tpanalysis::UI_sliderValueMin, 0 );
                SetValue ( tpanalysis::UI_sliderEnable, ( int ) on );
            }

            void TPAnalysisModel::ResetPreviewData ( const int type, int sel )
            {
                CadEntitiesIt_T pEmptyEnts;
                tpanalysis::Coord3dIt_T pPoint;

                switch ( type )
                {
                    case analyseData::dai_REFP :
                    {
                        s_data.m[sel].specialIndex [analyseData::dai_REFP] = UNKNOWNINDEX;
                        break;
                    }
                    case analyseData::dai_Index1stTpEnt :
                    {
                        s_data.m[sel].specialIndex [analyseData::dai_Index1stTpEnt] = UNKNOWNINDEX;
                        s_data.pSelectedTP [sel].Reset ();
                               
                        s_data.m[sel].viewVec.clear ();
                        s_data.m[sel].pointSizeVec.clear ();
                        break;
                    }
                    case analyseData::dai_Index2ndTpEnt: 
                    {
                        s_data.pSelectedTP [sel].Reset ();
                        s_data.m[sel].specialIndex [analyseData::dai_Index2ndTpEnt] = UNKNOWNINDEX;
                        break;
                    }
                    case analyseData::dai_Index1stTpSeg :
                    {
                        s_data.m[sel].specialIndex [analyseData::dai_Index1stTpSeg] = UNKNOWNINDEX;
                        s_data.m[sel].specialIndex [analyseData::dai_SXP1] = UNKNOWNINDEX;
                        s_data.pSelectedTP [sel].Reset ();
                        //SetValue ( tpanalysis::TGr_PreviewPoints_Ent1, pPoint );

                        s_data.m[sel].pointVec.clear ();
                        s_data.m[sel].typeVec.clear ();

                        s_data.m[sel].m_pAttr = nullptr;
                        s_data.m[sel].m_pTpSeg = nullptr;
                        s_data.m[sel].m_pTpSegCadEnt = nullptr;
                        s_data.m[sel].m_pTpSegCast = nullptr;
                        break;
                    }
                    case analyseData::dai_Index2ndTpSeg:
                    {
                        s_data.m[sel].specialIndex [analyseData::dai_Index2ndTpSeg] = UNKNOWNINDEX;
                        s_data.m[sel].specialIndex [analyseData::dai_SXP2] = UNKNOWNINDEX;
                        s_data.pSelectedTP [sel].Reset ();
                        //SetValue ( tpanalysis::TGr_PreviewPoints_Ent2, pPoint );
                        break;
                    }
                }
            }

            void TPAnalysisModel::copyInternalSettings ( const TPAnalysisModel* pSource )
            {
                if ( pSource )
                {
                    b_GnIcon = pSource->b_GnIcon;
                    //pSource->
                }
            }
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
