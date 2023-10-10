#pragma warning( disable : 4996 ) // for warnings: due to compare of boost::uuids
#include "TP_SpeedAnalysisModel.h"
// keys
#include <Commands\tp_speedanalysiskeys.h>
// Document
#include <hcntcore/document.h>

#include <propertyservice/propertybag/minmaxvalidatort.h>

// warnings
#include <warningsservice/warningdefine.h>
#include <translate.h>
#include <warningsservice/warningfactory.h>

#include <bgeo/typeofbgeotypesiterators.h>
#include <hcntcore/typeofreferencesystem.h>

#include <selectionservice/filter.h>
#include <selectionservice/filters.h>
#include <selectionservice/entityfilter.h>
#include <selectionservice/interactiveselector.h>
#include <selectionservice/selectionservice.h>
#include <selectionservice/entityfilterutilities.h>
#include <hcntcore/application.h>
#include <commands\common_functions.h>


#include <materials/colormaterial.h>
#include <materials/colorspalette.h>
#include <materials/colorspalettefactory.h>
#include <materials/material.h>
#include <materials/materials.h>
#include <materials/materialslibrary.h>
#include <materials/materialsstatus.h>
#include <..\..\..\INTERFACES\iHCNT\iTopoAttrib.h>
#include <graphicattributes/toolpathgraphicattributes.h>
#include <graphicattributes/toolpathsegmentgraphicattributes.h>
#include <graphicattributes/pointcloudgraphicattributes.h>
#include <graphicattributes/graphicattributesfactory.h>
#include <hcntcore/polylineithpoint.h>
#include <hcntcore/geomentity.h>
#include <hcntcore/pargeomentity.h>
#include <containers/mapcontainer.h>
#include <containers/iterator.h>
#include <hcntcore/model.h>
#include <hcntcore/cadentitymgr.h>

#include <commands/tp_listener.h>
#include <commands/tp_signalemitter.h>
#include <tchar.h>
typedef omsl::SharedPtr<omsl::iterator::UlongUlongMapIteratorForwardReadOnly> UlongUlongMapIteratorForwardReadOnlyPtr;
typedef omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> CadEntityForwardPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::GraphicAttributes> GraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::ToolpathGraphicAttributes> ToolpathGraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::ToolpathSegmentGraphicAttributes> ToolpathSegmentGraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::PointCloudGraphicAttributes> PointCloudGraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::geom::TopoGeom> TopoGeomPtr;
typedef omsl::SharedPtr<omsl::geom::ToolpathSegment> ToolpathSegmentPtr;
typedef omsl::SharedPtr<omsl::hcntcore::ToolPathEntity> ToolpathPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::CurveGraphicAttributes> CurveGraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::hcntcore::GeomEntity> GeomEntityPtr;
typedef omsl::SharedPtr<omsl::geom::Geom3d> Geom3dPtr;

typedef omsl::container::MapContainerWrapper<omsl::ULong, omsl::ULong, std::map<omsl::ULong, omsl::ULong>, omsl::iterator::UlongUlongMapIteratorForwardReadOnly> ULULMap;
typedef omsl::SharedPtr<ULULMap> ULULMapPtr;

typedef omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> CadEntityForwardPtr;

#include <hcntLib/ToolPathHlp.h>

using omsl::linguistic::QT_TRANSLATE_NOOP;
using omsl::linguistic::QT_TRANSLATE_NOOP3;

OM_BEGIN_WARNINGS_DEFINITION
DEFINE_WARNING ( kGeneralWarningTPSpeedAnalysis, "tp_analyzer.TPSpeedAnalysis", "General warning." )
OM_END_WARNINGS_DEFINITION
using namespace omsl::command;
using namespace omsl::selectionservice;



namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {

            OMSL_COMMANDMODEL_FACTORY ( TPSpeedAnalysis )
            {
                return new TPSpeedAnalysisModel ( pDocument );
            }

            TPSpeedAnalysisModel::TPSpeedAnalysisModel ( omsl::hcntcore::Document* pDocument )
            {
                clear_callStackString ();
                callStackString ( L"TPSpeedAnalysisModel", 1 );

                m_pDocument = pDocument;
                m_compTPAMode = cm1_First;
                callStackString ( L"TPSpeedAnalysisModel", -1 );
                omsl::hmbroadcast::RegisterSender ();
                omsl::hmbroadcast::RegisterListener ();
            }

            TPSpeedAnalysisModel::~TPSpeedAnalysisModel ()
            {
                omsl::hmbroadcast::EmitTPSignal ( L"TPCMPingAllTP", omsl::hmbroadcast::bcs_TPCMPingAllTP_ColorSwitchOff, nullptr, -1 );
                omsl::hmbroadcast::EmitTPSignal ( L"bcs_FRA_end", omsl::hmbroadcast::bcs_FRA_end, nullptr, -1 );
                omsl::hmbroadcast::RemoveListener ();
                omsl::hmbroadcast::RemoveSender ();
//                bool old = SetSignalListening ( false );
            }
            void TPSpeedAnalysisModel::InternalDefineContext ()
            {
                callStackString ( L"InternalDefineContext", 1 );

                // interface to hm data and info
                // Context definition

                AddEntityValue ( tpanalysis::UI_Toolpath, propertyservice::Applicable::kMandatory,
                                 L"ToolPathEntity", 1 );

                Add ( tpanalysis::UI_Direction   , 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_Label       , 0, propertyservice::Applicable::kMandatory );
                Add ( tpanalysis::UI_Vector      , 0, propertyservice::Applicable::kMandatory );
//                Add ( tpanalysis::UI_Orientation , 0, propertyservice::Applicable::kMandatory );
//                Add ( tpanalysis::UI_Compensation, 0, propertyservice::Applicable::kMandatory );

                // set the first value
                Add ( tpanalysis::UI_RefSpeedType, 0, propertyservice::Applicable::kMandatory );

                // set the GUI output context
                SharedPtr<propertyservice::PropertyBag> pEmpty;


                Add ( tpanalysis::DummyParameter, 0, propertyservice::Applicable::kMandatory );
//                Add ( tpanalysis::ToolpathCounter, -1, propertyservice::Applicable::kMandatory );

                SetValueState ( tpanalysis::DummyParameter, propertyservice::Value::kWrong ); // make the command unapplyable

                // to react on viewmode changes

                // set the geometrical output context

                //                Add ( tpanalysis::TGr_CompensationVectors, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::TGr_LabelVectors, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::TGr_TextPoints, propertyservice::Applicable::kHardCoded );
                Add ( tpanalysis::TGr_Texts, propertyservice::Applicable::kHardCoded );

//                Add ( tpanalysis::TGr_PreviewPoints_R3rd, propertyservice::Applicable::kHardCoded );

                // Setup validators
                SetValidator ( tpanalysis::UI_RefSpeedType, new propertyservice::MinMaxValidatorT<int> ( 0, 1 ) );

                // Connection to values

                ADD_COMMAND_VALUE_RULE ( TPSpeedAnalysisModel::HighlightToolPath, tpanalysis::DummyParameter, tpanalysis::UI_Toolpath );
                //// Rules
                ADD_COMMAND_STRENGTH_RULE ( TPSpeedAnalysisModel::ActivateToolpath , tpanalysis::UI_Toolpath,  tpanalysis::DummyParameter );

                ADD_COMMAND_VALUE_RULE ( TPSpeedAnalysisModel::OnSetupColorMap, tpanalysis::DummyParameter, tpanalysis::UI_RefSpeedType );
// compute should be called by highlight or other changes
//                ADD_COMMAND_VALUE_RULE ( TPSpeedAnalysisModel::Compute, tpanalysis::UI_SpeedAnalyseResult, );//, tpanalysis::UI_RefPoint  , tpanalysis::UI_RefSystemType );

                CadEntityPtr pEmptyEnt;
                CadEntitiesIt_T pEmptyEnts;

                SetValue ( tpanalysis::UI_Toolpath, pEmptyEnts );


                m_withUpdate = false;
                SetValue ( tpanalysis::UI_RefSpeedType, GetValue ( tpanalysis::UI_RefSpeedType ) );
                m_withUpdate = true;

//                SetValue ( tpanalysis::UI_Direction, GetValue ( tpanalysis::UI_Direction ) );
//                SetValue ( tpanalysis::UI_Label, GetValue ( tpanalysis::UI_Label ) );
//                SetValue ( tpanalysis::UI_Vector, GetValue ( tpanalysis::UI_Vector ) );
//                SetValue ( tpanalysis::UI_Orientation, GetValue ( tpanalysis::UI_Orientation ) );
//                SetValue ( tpanalysis::UI_Compensation, GetValue ( tpanalysis::UI_Compensation ) );

                omsl::hmbroadcast::SetModelToListener ( this, true );
                omsl::hmbroadcast::EmitTPSignal ( L"bcs_FRA_start", omsl::hmbroadcast::bcs_FRA_start, nullptr, -1 );
                omsl::hmbroadcast::EmitTPSignal ( L"bcs_TTIP_request", omsl::hmbroadcast::bcs_TTIP_request, nullptr, -1 );
                omsl::hmbroadcast::EmitTPSignal ( L"TPCMPingAllTP", omsl::hmbroadcast::bcs_TPCMPingAllTP_ColorSwitchOn, nullptr, -1 );
                omsl::hmbroadcast::EmitTPSignal ( L"TPCMPingAllTP", omsl::hmbroadcast::bcs_TPCMPingAllTP_GetExtrema, nullptr, -1 );
                {
                    callStackString ( L"return 0", -1 );
                    return;
                }
            }


            bool TPSpeedAnalysisModel::CheckSignalTypeForCadEntityFRA
            (
                omsl::hcntcore::CadEntity* pEnt
                , int internal_HM_type
                , int data
            )
            {
                bool res = true;
                switch ( internal_HM_type )
                {
                    case omsl::hmbroadcast::bcs_TPA_cb_GV_on:
                    {
                        bool old = SetSignalListening ( false );

                        b_GVIcon = true;

                        SetSignalListening ( old );
                        break;
                    }
                    case omsl::hmbroadcast::bcs_TPA_cb_GV_off:
                    {
                        bool old = SetSignalListening ( false );

                        b_GVIcon = false;

                        SetSignalListening ( old );
                        break;
                    }
                }

                if ( pEnt == nullptr )
                    return true;

                // strategy 
                // first check if toolpath or toolpathegment

                // check if the entity is part of the selection
                // check if visible : true 
                // check for visible points and visible vectors

                // part of selection and invisible ( current state ) clear selection
                // part of selection : update checkboxes 

                // if not part of selection do nothing

                CadEntityPtr pEntTP ( pEnt );

                if ( pEntTP.Get () == nullptr )
                    return true;

                pEntTP = GetReferencedToolpath ( pEntTP );

                if ( pEntTP.Get () == nullptr )
                    return true;

                ToolpathPtr pToolpath = pEntTP.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );
                if ( pToolpath.Get () == nullptr )
                    return true;

                // delete reaction missing

                switch ( internal_HM_type )
                {
                    //case omsl::hmbroadcast::bcs_TPCMHide: // only in hide case the entity is known by the command
                    //{
                    //    //removeLabel ( pEnt );
                    //    //removeDirection ( pEnt );

                    //    //// reset selection if necessary

                    //    //int i = 0;
                    //    //for ( i = 0; i < 2; i++ )
                    //    //{
                    //    //    if ( !m_data.pSelectedTP [i].Get () )
                    //    //        continue;

                    //    //    if ( m_data.pSelectedTP [i].Get () == pToolpath.Get () )
                    //    //    {
                    //    //        break;
                    //    //    }
                    //    //}

                    //    //if ( i == 2 )
                    //    //{
                    //    //    break;
                    //    //}

                    //    //// clear content
                    //    //CadEntitiesIt_T pEmptyEnts;
                    //    //if ( i == 0 )
                    //    //    SetValue ( tpanalysis::UI_FirstEntities, pEmptyEnts );
                    //    //else if ( i == 1 )
                    //    //    SetValue ( tpanalysis::UI_SecondEntities, pEmptyEnts );

                    //    // clear selection
                    //    break;
                    //}
                    //case omsl::hmbroadcast::bcs_TPCMShow:
                    //{
                    //    //SetupTemporaryGraphicsLabel ( GetValue ( tpanalysis::viewModeParameter ), pEnt );
                    //    //SetupTemporaryGraphicsDirection ( GetValue ( tpanalysis::viewModeParameter ), pEnt );
                    //    break;
                    //}
                    case omsl::hmbroadcast::bcs_TPCMPingAllTP: // simple on mode or state depending on phase
                    {
                        //SetupTemporaryGraphicsLabel ( GetValue ( tpanalysis::viewModeParameter ), pEnt );
                        //SetupTemporaryGraphicsDirection ( GetValue ( tpanalysis::viewModeParameter ), pEnt );
                        break;
                    }
                    case omsl::hmbroadcast::bcs_TPCMPingAllTP_Label:
                    {
                        //SetupTemporaryGraphicsLabel ( vm_SHOWLABEL, pEnt );
                        break;
                    }
                    case omsl::hmbroadcast::bcs_TPCMPingAllTP_Direction:
                    {
                        //SetupTemporaryGraphicsDirection ( vm_SHOWDIRECTION, pEnt );
                        break;
                    }
                    case omsl::hmbroadcast::bcs_TPCMPingAllTP_GetExtrema:
                    {
                        static double s_smin = 100000.0, s_smax = -100000.0, s_fmin = 100000.0, s_fmax = -100000.0, s_cmax = -100000.0;

                        double smin = 100000.0, smax = -100000.0, fmin = 100000.0, fmax = -100000.0, cmax = -100000.0;

                        CadEntityForwardPtr it ( pToolpath->GetCadEntities () );

                        OMSL_FOREACH ( it )
                        {
                            CadEntityPtr cep ( it->Get () );
                            ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep ) );
                            GeomEntityPtr epop = cep.shared_cast < omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                            if ( !pSegment.Get () )
                                continue;
                            TopoGeomPtr pTopo = pSegment.shared_cast< omsl::geom::TopoGeom >( L"TopoGeom" );
                            if ( !pTopo || !pTopo->GetAttribs () )
                                continue;

                            SharedPtr<omsl::geom::Attribute>m_pAttr ( pTopo->GetAttribs ()->Get ( L"HM_ToolpathData" ) );
                            if ( !m_pAttr.Get () )
                                continue;

                            omsl::geom::AttributeRefCounter* pAttrRef = ( omsl::geom::AttributeRefCounter* ) m_pAttr->Cast ( L"AttributeRefCounter" );
                            if ( !pAttrRef )
                                continue;

                            const RefCounter * pRef = pAttrRef->GetValue ();
                            if ( !pRef )
                                continue;

                            ToolpathData* ptpData = ( ToolpathData * ) pRef->Cast ( L"ToolpathData" );
                            // error
                            if ( !ptpData )
                                continue;

                            ptpData->GetExtrema ( smin, smax, fmin, fmax, cmax );
                            if ( smin < s_smin ) s_smin = smin;
                            if ( fmin < s_fmin ) s_fmin = fmin;
                            if ( smax > s_smax ) s_smax = smax;
                            if ( fmax > s_fmax ) s_fmax = fmax;
                            if ( cmax > s_cmax ) s_cmax = cmax;
                        }
                        break;
                    }
                    case omsl::hmbroadcast::bcs_TPCMShow:
                    case omsl::hmbroadcast::bcs_TPCMPingAllTP_ColorSwitchOn :
                    {
                        std::map<double, int> speedClrMap;
                        std::map<omsl::ULong, omsl::ULong> newPt2IdMap;
                        //omsl::materials::ColorsPalette*pPalette = nullptr;

                        double tp_aclrs [] = {
                              1., 0.6, .40   // 0 // red
                            , 1., 0.5, .50   // 1 // yellow
                            , 1., 0.4, .60   // 0 // red
                            , 1., 0.3, .70   // 1 // yellow
                            , 1., 0.2, .80   // 0 // red
                            , 1., 0.1, .90   // 1 // yellow
                            , 0.6, .40, 1.   // 0 // red
                            , 0.5, .50, 1.   // 1 // yellow
                            , 0.4, .60, 1.   // 0 // red
                            , 0.3, .70, 1.   // 1 // yellow
                            , 0.2, .80, 1.   // 0 // red
                            , 0.1, .90, 1.   // 1 // yellow

                        };
                        std::vector<int> clr_ids;

                        newPt2IdMap.clear ();
                        speedClrMap [0] = 0; // allway needed
                        speedClrMap [500] = 6; // lower limit
                        speedClrMap [1000] = 7;
                        speedClrMap [1500] = 8;
                        speedClrMap [2000] = 9;
                        speedClrMap [2500] =10;
                        speedClrMap [3000] =11;
                        speedClrMap [3500] =12;
                        speedClrMap [4000] =13;
                        speedClrMap [4500] =14;
                        speedClrMap [5000] = 15;
                        speedClrMap [5500] = 0; // upper limit


                        CadEntityForwardPtr it (pToolpath->GetCadEntities ());

                        OMSL_FOREACH ( it )
                        {
                            CadEntityPtr cep ( it->Get () );
                            ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep ) );
                            GeomEntityPtr epop = cep.shared_cast <omsl::hcntcore::GeomEntity>( L"GeomEntity" );
                            if ( !pSegment.Get () )
                                continue;
                            TopoGeomPtr pTopo = pSegment.shared_cast< omsl::geom::TopoGeom >( L"TopoGeom" );
                            if ( !pTopo || !pTopo->GetAttribs () )
                                continue;

                            SharedPtr<omsl::geom::Attribute>m_pAttr ( pTopo->GetAttribs ()->Get ( L"HM_ToolpathData" ) );
                            if ( !m_pAttr.Get () )
                                continue;

                            omsl::geom::AttributeRefCounter* pAttrRef = ( omsl::geom::AttributeRefCounter* ) m_pAttr->Cast ( L"AttributeRefCounter" );
                            if ( !pAttrRef )
                                continue;

                            const RefCounter * pRef = pAttrRef->GetValue ();
                            if ( !pRef )
                                continue;

                            ToolpathData* ptpData = ( ToolpathData * ) pRef->Cast ( L"ToolpathData" );
                            // error
                            if ( !ptpData )
                                continue;

                            size_t s1;
                            int s = 6; //ptpData->getSavedColorPaletteSize ();
                            clr_ids.clear (); // to reduce data
                            clr_ids.push_back ( s + 0 );     // m_colNM
                            clr_ids.push_back ( s + 1 );     // m_colNM
                            clr_ids.push_back ( s + 2 );     // m_colNM
                            clr_ids.push_back ( s + 3 );     // m_colNM
                            clr_ids.push_back ( s + 4 );     // m_colNM
                            clr_ids.push_back ( s + 5 );     // m_colNM
                            clr_ids.push_back ( s + 6 );     // m_colNM
                            clr_ids.push_back ( s + 7 );     // m_colNM
                            clr_ids.push_back ( s + 8 );     // m_colNM
                            clr_ids.push_back ( s + 9 );     // m_colNM
                            clr_ids.push_back ( s + 10);     // m_colNM
                            clr_ids.push_back ( s + 11);     // m_colNM
                            GraphicAttributesPtr gap ( cep->GetGraphicAttributes () );
                            if ( gap.Get () )
                            {
                                ToolpathSegmentGraphicAttributesPtr tpsgaporig = gap.shared_cast<omsl::graphicattributes::ToolpathSegmentGraphicAttributes>( L"ToolpathSegmentGraphicAttributes" );

                                GraphicAttributesPtr tpsgnew = omsl::graphicattributes::OverriddenGraphicAttributesFactory ( gap.get () );
                                ToolpathSegmentGraphicAttributesPtr tpsgap = tpsgnew.shared_cast<omsl::graphicattributes::ToolpathSegmentGraphicAttributes>( L"ToolpathSegmentGraphicAttributes" );

                                if ( tpsgap.Get () )
                                {

                                    hcNTLib::ColorsPalettePtr pColorPalette;
                                    hcNTLib::ColorsPaletteCreateAsMap ( pColorPalette, (int)clr_ids.size (),tp_aclrs,&clr_ids.front () );

                                    hcNTLib::ColorsPalettePtr pColorPalette1 ( tpsgaporig->GetColorsPalette () );
                                    s1 = pColorPalette1->Size ();
                                    omsl::materials::Status st;
                                    float r, g, b;
                                    size_t i = 0;
                                    // copy orig colors ?
                                    for ( i = 0; i < s1 ; i++ )
                                    {
                                        pColorPalette1->GetColor ( (unsigned long)  i )->GetAll ( r, g, b );

                                        hcNTLib::ColorMaterialPtr cn =  omsl::materials::ColorMaterialRgbaFFactory ( r, g, b, 0.0 );

                                        st = pColorPalette->SetColor ( i, cn.Get () );
                                    }
                                    s1 = clr_ids.size ();
                                    for ( i = 0; i < s1 ; i++ )
                                    {
                                        hcNTLib::ColorMaterialPtr cn =  omsl::materials::ColorMaterialRgbaFFactory ((float) tp_aclrs [i * 3 ], ( float ) tp_aclrs [i * 3 + 1], ( float ) tp_aclrs [i*3+2], 0.0 );

                                        st = pColorPalette->SetColor ( clr_ids[i], cn.Get () );
                                    }

                                    tpsgap->SetColorsPalette ( pColorPalette.Get () );
                                    cep->UpdateGraphicAttributes ( tpsgnew.Get () );
                                    pToolpath->UpdateGraphicAttributes ( tpsgnew.Get () );
                                    hcNTLib::ToolPathEntityUpdateGraphicAttributes ( pToolpath );

                                    ptpData->createPtIndexBySpeed ( speedClrMap, newPt2IdMap, 0 );
                                    ULULMapPtr ulptr ( new ULULMap ( newPt2IdMap ) );
                                    pSegment->ResetPointToIdentifierMap ();
                                    pSegment->MapPointsToIdentifiers ( ulptr.Get () );

                                    hcntcore::Model* model = ( hcntcore::Model* )m_pDocument->Cast ( L"Model" );
                                    (void) model->CadEntityMgr ()->RebuildCadEntityGraphic ( omsl::hcntcore::CadEntityMgr::kVisible );

                                    cep->UpdateGraphicAttributes ( tpsgnew.Get () );

//                                    

                                    pToolpath->UpdateGraphicAttributes ( tpsgap.Get () );
                                    hcNTLib::ToolPathEntityUpdateGraphicAttributes ( pToolpath );


//                                    int stat = model->CadEntityMgr ()->RebuildCadEntityGraphic ( omsl::hcntcore::CadEntityMgr::kVisible );
                                }
                            }
                        }

//                        hcNTLib::ToolPathEntityUpdateGraphicAttributes ( pToolpath );

                        static unsigned long s_MYDUMPFILTER = NCDATA_EXTREMA | SEGMENT_GRAPHICATTRIBUTE;
                        const TCHAR *env1 = _tgetenv ( _TEXT ( "OM_TOOLPATH" ) );
                        const TCHAR *env2 = _tgetenv ( _TEXT ( "OM_TOOLPATHDUMP" ) );
                        const TCHAR *env3 = _tgetenv ( _TEXT ( "OM_TOOLPATHDUMPOUT" ) );
                        std::vector<std::wstring> out1;
                        std::vector<std::wstring> out2;
                        std::vector<std::wstring> out3;
                        if ( env2 )
                        {
                            swscanf ( env2, L"%x", &s_MYDUMPFILTER );
                            if ( env2 )
                            {
                                hcNTLib::dumpToolpath ( pToolpath, out1, s_MYDUMPFILTER, true );
                            }
                        }

                        if ( env3 )
                        {
                            hcNTLib::printDump ( env3, out1, true );
                        }
                        else
                        {
                            hcNTLib::printDump ( L"D:/temp/dumpTP.txt", out1, true );
                        }

                        break;
                    }
                    case omsl::hmbroadcast::bcs_TPCMHide:
                    case omsl::hmbroadcast::bcs_TPCMPingAllTP_ColorSwitchOff:
                    {
                        static unsigned long s_MYDUMPFILTER = NCDATA_EXTREMA | SEGMENT_GRAPHICATTRIBUTE;
                        const TCHAR *env1 = _tgetenv ( _TEXT ( "OM_TOOLPATH" ) );
                        const TCHAR *env2 = _tgetenv ( _TEXT ( "OM_TOOLPATHDUMP" ) );
                        const TCHAR *env3 = _tgetenv ( _TEXT ( "OM_TOOLPATHDUMPOUT" ) );
                        std::vector<std::wstring> out1;
                        std::vector<std::wstring> out2;
                        std::vector<std::wstring> out3;

                        std::map<int, int> motionClrMap;
                        std::map<omsl::ULong, omsl::ULong> newPt2IdMap;
                        //omsl::materials::ColorsPalette*pPalette = nullptr;

                        CadEntityForwardPtr it ( pToolpath->GetCadEntities () );

                        OMSL_FOREACH ( it )
                        {
                            CadEntityPtr cep ( it->Get () );
                            ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep ) );

                            if ( !pSegment.Get () )
                                continue;
                            TopoGeomPtr pTopo = pSegment.shared_cast< omsl::geom::TopoGeom >( L"TopoGeom" );
                            if ( !pTopo || !pTopo->GetAttribs () )
                                continue;

                            SharedPtr<omsl::geom::Attribute>m_pAttr ( pTopo->GetAttribs ()->Get ( L"HM_ToolpathData" ) );
                            if ( !m_pAttr.Get () )
                                continue;

                            omsl::geom::AttributeRefCounter* pAttrRef = ( omsl::geom::AttributeRefCounter* ) m_pAttr->Cast ( L"AttributeRefCounter" );
                            if ( !pAttrRef )
                                continue;

                            const RefCounter * pRef = pAttrRef->GetValue ();
                            if ( !pRef )
                                continue;

                            ToolpathData* ptpData = ( ToolpathData * ) pRef->Cast ( L"ToolpathData" );
                            // error
                            if ( !ptpData )
                                continue;
                            double smin, smax, fmin, fmax, cmax;
                            ptpData->GetExtrema ( smin, smax, fmin, fmax, cmax );

                            newPt2IdMap.clear ();
                            motionClrMap [-1] = 0; // unknown
                            motionClrMap [0] = 0; // G0
                            motionClrMap [1] = 1; // G1
                            if ( env2 )
                            {
                                motionClrMap [2] = 2; // G2
                                motionClrMap [3] = 3; // G3
                            }

                            ptpData->createPtIndexByMotion ( motionClrMap, newPt2IdMap );
                            ULULMapPtr ulptr ( new ULULMap ( newPt2IdMap ) );
                            pSegment->ResetPointToIdentifierMap ();
                            pSegment->MapPointsToIdentifiers ( ulptr.Get () );

                            size_t s = ptpData->getSavedColorPaletteSize ();

                            GraphicAttributesPtr gap ( cep->GetGraphicAttributes () );
                            if ( gap.Get () )
                            {
                                GraphicAttributesPtr attr2 = omsl::graphicattributes::OverriddenGraphicAttributesFactory ( gap.get () );
                                ToolpathSegmentGraphicAttributesPtr tpsgap = attr2.shared_cast<omsl::graphicattributes::ToolpathSegmentGraphicAttributes>( L"ToolpathSegmentGraphicAttributes" );

                                if ( tpsgap.Get () )
                                {
                                    hcNTLib::ColorsPalettePtr pColorPalette ( tpsgap->GetColorsPalette () );
                                    size_t s1 = pColorPalette->Size ();
                                    for ( ; s < s1; s++ )
                                    {
                                        pColorPalette->RemoveColor ( s );
                                    }

                                    tpsgap->SetColorsPalette ( pColorPalette.Get () );
                                    cep->UpdateGraphicAttributes ( tpsgap.Get () );
                                }
                            }
                        }

                        hcNTLib::ToolPathEntityUpdateGraphicAttributes ( pToolpath );

                        if ( env2 )
                        {
                            swscanf ( env2, L"%x", &s_MYDUMPFILTER );
                            if ( env2 )
                            {
                                hcNTLib::dumpToolpath ( pToolpath, out1, s_MYDUMPFILTER, true );
                            }
                        }

                        if ( env3 )
                        {
                            hcNTLib::printDump ( env3, out1, true );
                        }
                        else
                        {
                            hcNTLib::printDump ( L"D:/temp/dumpTP.txt", out1, true  );
                        }
                        break;
                    }
                }

                {
                    //                    SetupTemporaryGraphics ();
                    //int vm = GetValue ( tpanalysis::viewModeParameter );
                }
                return res;
            }

            bool TPSpeedAnalysisModel::CheckToggleSignal
            (
                int internal_HM_type
            )
            {
                bool res = true;
                //bool updateGraphics = false;

                if ( SignalListeningIsEnabled () == false )
                    return true;

                bool old = SetSignalListening ( false );

                switch ( internal_HM_type )
                {
                    case omsl::hmbroadcast::bcs_TPCMShowGV:
                    {
                        // toggle
                        b_GVIcon = !b_GVIcon;
                        omsl::hmbroadcast::EmitTPSignal ( L"TPCMShowGV", omsl::hmbroadcast::bcs_TPCMShowGV, nullptr, -1 );
                        omsl::hmbroadcast::EmitGVIcon ( b_GVIcon == true ? 1 : 0 );

                        break;
                    }
                }

                SetSignalListening ( old );

                return res;
            }

            void TPSpeedAnalysisModel::InternalApply ()
            {
                callStackString ( L"InternalApply", 1 );

//                int i = 0;
                m_withUpdate = true;
                callStackString ( L"InternalApply", -1 );

            }

            propertyservice::Applicable::Strength TPSpeedAnalysisModel::ActivateToolpath 
            ( 
                const int& // iMode 
            )
            {
                callStackString ( L"ActivateToolpath", 1 );

                propertyservice::Applicable::Strength retVal = propertyservice::Applicable::kMandatory;
                {
                    callStackString ( L"return 0", -1 );
                }
                return retVal;
            }

            void TPSpeedAnalysisModel::ResetPreviewData ( const int type )
            {
                callStackString ( L"ResetPreviewData", 1 );

                CadEntitiesIt_T pEmptyEnts;
                tpanalysis::Coord3dIt_T pPoint;

                switch ( type )
                {
                    case analyseData1::dai_REFP :
                    {
                        m_data.specialIndex [analyseData1::dai_REFP] = -1;
                        break;
                    }
                    case analyseData1::dai_Index1stTpEnt :
                    {
                        m_data.specialIndex [m_data.dai_Index1stTpEnt] = -1;

                        //SetValue ( tpanalysis::UI_FirstEntities, pEmptyEnts );
                        //SetValue ( tpanalysis::UI_SecondEntities, pEmptyEnts );
                        m_data.viewVec.clear ();
                        m_data.pointSizeVec.clear ();
                        break;
                    }
                    case analyseData1::dai_Index2ndTpEnt: 
                    {
                        m_data.specialIndex [m_data.dai_Index2ndTpEnt] = -1;
                        break;
                    }
                    case analyseData1::dai_Index1stTpSeg :
                    {
                        m_data.specialIndex [m_data.dai_Index1stTpSeg] = -1;
                        m_data.specialIndex [analyseData1::dai_SXP1] = -1;

                        m_data.pointVec.clear ();
                        m_data.typeVec.clear ();

                        m_data.m_pAttr = nullptr;
                        m_data.m_pTpSeg = nullptr;
                        m_data.m_pTpSegCadEnt = nullptr;
                        m_data.m_pTpSegCast = nullptr;
                        break;
                    }
                    case analyseData1::dai_Index2ndTpSeg:
                    {
                        m_data.specialIndex [m_data.dai_Index2ndTpSeg] = -1;
                        m_data.specialIndex [analyseData1::dai_SXP2] = -1;

                        break;
                    }
                }
                {
                    callStackString ( L"return 0", -1 );
                    return;
                }
            }

            SharedPtr<propertyservice::PropertyBag> TPSpeedAnalysisModel::Compute ( const int& )
            {
                callStackString ( L"Compute", 1 );
                SharedPtr<propertyservice::PropertyBag> pBag0;
                pBag0 = propertyservice::PropertyBag::Create ();
                // ToDo setup the path via setup / install
                if ( pBag0.Get () == nullptr )
                {
                    callStackString ( L"return 1", -1 );
                    return pBag0;
                }

                // ??? add rootpath
                // pBag->SetName ( LR"(C:\OPEN MIND\hyperMILL\99.0.1\mnu\hcNT\tp.analysis.description)" );
                // ??? add rootpath
                if ( !m_descriptionPath || m_descriptionPath.Get () == nullptr )
                {
                    omsl::hcntcore::Application* pApplication = omsl::hcntcore::GetApplication ();
                    if ( pApplication )
                    {
                        propertyservice::PropertyBag *pBag = pApplication->GetPropertyBag ();

                        if ( pBag->Exists ( omsl::hcntcore::settingskeys::rootFolder ) )
                        {

                            SharedPtr<omsl::WString> m_path = pBag->GetValue <omsl::SharedPtr<omsl::WString> > ( omsl::hcntcore::settingskeys::rootFolder );
                            if ( m_path )
                            {
                                const wchar_t* t = nullptr;
                                t = ( const wchar_t* ) m_path->Get ();
                                std::wstring s ( t );
                                s += L"\\files\\commands\\ToolpathAnalyser\\tp.speedanalysis.description";
                                m_descriptionPath = omsl::WStringImpl::Create ( s.c_str () );
                            }
                        }
                    }
                }

                if ( m_descriptionPath && m_descriptionPath.Get () )
                    pBag0->SetName ( m_descriptionPath.Get ()->Get () );
                else
                {
                    callStackString ( L"return 1", -1 );
                    return pBag0;
                }

                //    SetupTemporaryGraphics ();

                //SetupPropertyBag ( pBag0 );
                {
                    callStackString ( L"return 0", -1 );
                }
                return pBag0;
            }


            int TPSpeedAnalysisModel::OnSetupColorMap ( const int& UI_GBox_RefSpeedType )
            {
                callStackString ( L"OnSetupCoordinateSystem", 1 );

                //tpanalysis::Motion_T pGraphicsRefSys;

                //SetValue ( tpanalysis::TGr_ReferenceSystem, pGraphicsRefSys );
                //SetValue ( tpanalysis::WorkplaneEnabled, false );

                //if ( !m_pDocument || !m_pDocument.Get () )
                //{
                //    callStackString ( L"return 1", -1 );
                //    return 0;
                //}
                //// get the current reference system
                //if ( m_data.specialIndex [analyseData1::dai_Index1stTpEnt] == -1 )
                //{
                //    callStackString ( L"return 1", -1 );
                //    return 0;
                //}
                //switch ( UI_BtnGroup_RefSystemType )
                //{
                //    case cs_NCS:
                //    {
                //        m_data.motion = m_data.pof_ncs.Inverse ();
                //        break;
                //    }
                //    case cs_Frame: // default
                //    {
                //        m_data.motion = m_data.pof_frame.Inverse ();
                //        break;
                //    }
                //    case cs_World: // default
                //    {
                //        hcntcore::Model* model = ( hcntcore::Model* )m_pDocument->Cast ( L"Model" );
                //        SharedPtr<hcntcore::workplane::ReferenceSystem> rsys ( model->GetReferenceSystemMgr ()->GetCurrentReferenceSystem () );
                //        m_data.motion = rsys->GetMotion ().Inverse ();
                //        break;
                //    }
                //    default:
                //    {
                //        m_data.motion = omsl::bgeo::RigidMotion3d ();
                //        break;
                //    }
                //}

                //SetValue ( tpanalysis::TGr_ReferenceSystem, m_data.motion.Inverse () );
                //SetValue ( tpanalysis::WorkplaneEnabled, true );

                //if ( m_withUpdate )
                //    OnChangeMoveNr2 ( m_data.specialIndex [m_data.dai_SXP1] );
                {
                    callStackString ( L"return 0", -1 );
                }
                return 1;
            }
            omsl::geom::ToolpathSegment* TPSpeedAnalysisModel::CastToToolpathSegment ( const SharedPtr<hcntcore::CadEntity>& pEntsIn )
            {
                callStackString ( L"CastToToolpathSegment", 1 );

                if ( m_data.m_pTpSegCast.Get () != nullptr && m_data.m_pTpSegCadEnt.Get () == pEntsIn.Get () )
                {
                    callStackString ( L"return 1", -1 );
                    return m_data.m_pTpSegCast.Get ();
                }

                m_data.m_pTpSegCadEnt = pEntsIn.Get ();
                m_data.m_pTpSegCast = nullptr;

                SharedPtr<hcntcore::CadEntity> pEnts ( pEntsIn.Get () );
                GeomEntityPtr gentp = pEnts.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                if ( gentp.Get () )
                {
                    Geom3dPtr g3dp ( gentp->Geom () );
                    if ( g3dp.Get () )
                        m_data.m_pTpSegCast = g3dp.shared_cast< omsl::geom::ToolpathSegment >( L"ToolpathSegment" );
                }

                if ( m_data.m_pTpSegCast.Get () == nullptr )
                {
                    SharedPtr<omsl::hcntcore::ParGeomEntity> pParGeom = pEnts.shared_cast< omsl::hcntcore::ParGeomEntity >( L"ParGeomEntity" );
                    if ( pParGeom.Get () )
                    {
                        omsl::SharedPtr<omsl::hcntcore::pargeom::Geom> pGeom ( pParGeom->GetParGeom () );

                        omsl::SharedPtr<omsl::hcntcore::pargeom::PolylineIthPoint> pgplipp ( pGeom.shared_cast< omsl::hcntcore::pargeom::PolylineIthPoint >( L"PolylineIthPoint" ) );
                        if ( pgplipp.Get () )
                        {
                            m_data.m_pParGeomCast = pgplipp->GetRefEntity ();
                            if ( m_data.m_pParGeomCast.Get () )
                            {
                                GeomEntityPtr gentp2 = m_data.m_pParGeomCast.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                                if ( gentp2.Get () )
                                {
                                    Geom3dPtr g3dp2 ( gentp2->Geom () );
                                    if ( g3dp2.Get () )
                                    {
                                        m_data.m_pTpSegCast = g3dp2.shared_cast< omsl::geom::ToolpathSegment >( L"ToolpathSegment" );
                                    }
                                }
                            }
                        }
                    }
                }
                callStackString ( L"return 0", -1 );
                return m_data.m_pTpSegCast.Get ();
            }

            int TPSpeedAnalysisModel::getSpeedColorIndexMap (std::map < double, int>& map )
            {
                // todo get the data from the color map class
                double s = 0;
                for ( int i = 0; i < 20; i++ )
                {
                    map [s] = i;
                    s += 100;
                }
                return 0;
            }

            typedef omsl::SharedPtr<omsl::materials::ColorMaterial> ColorMaterialPtr;
            typedef omsl::SharedPtr<omsl::materials::ColorsPalette> ColorsPalettePtr;
            typedef omsl::SharedPtr<omsl::materials::Material> MaterialPtr;

            typedef omsl::SharedPtr<omsl::materials::Material> MaterialPtr;
            typedef omsl::SharedPtr<omsl::materials::MaterialsLibrary> MaterialsLibraryPtr;
            typedef omsl::SharedPtr<omsl::materials::MaterialsLibraryIterator> MaterialsLibraryIteratorPtr;
            typedef omsl::SharedPtr<omsl::materials::ColorMaterial> ColorMaterialPtr;
            typedef omsl::SharedPtr<omsl::materials::ColorsPalette> ColorsPalettePtr;

            typedef omsl::SharedPtr<omsl::container::VectULongConcreteRandomReadOnly> VectULongConcreteRandomReadOnlyPtr;

            double s_maxspeed, s_minspeed, s_maxfeed, s_minfeed;
            struct IndexMapStruct
            {
                int tpIndex;
                int tpsegIndex;
                std::map<omsl::ULong, double> ptIndexSpeedMap;
                std::map<omsl::ULong, double> ptIndexFeedrateMap;
                std::map<omsl::ULong, omsl::ULong> ptIndexSClrIndexMap;
                std::map<omsl::ULong, omsl::ULong> ptIndexFClrIndexMap;
                ULULMapPtr pISCMap;
                ULULMapPtr pIFCMap;
                VectULongConcreteRandomReadOnlyPtr pVecUL_S;
                VectULongConcreteRandomReadOnlyPtr pVecUL_F;
            };

            std::vector <IndexMapStruct> s_mapVector;


            int ColorsPaletteCreateAsMap
            (
              ColorsPalettePtr&   p_cpp
            , int size
                
            )
            {
                try
                {
                    double color [3] = { 0.0,0.0,0.0 };
                    double ratio = 4.0 / (double)size;
                    int end = size / 4;
                    int between = size % 4;
                    int step2 = end;
                    p_cpp = omsl::materials::ColorsPaletteMapFactory ();


                    int i = 0;

                    //B2C
                    color [1] = 0.0;
                    color [2] = 1.0;
                    for ( ; i < end; i++ )
                    {
                        ColorMaterialPtr cmp
                            = ColorMaterialPtr (
                                omsl::materials::ColorMaterialRgbaFFactory (
                                static_cast<float>( color [0] ),
                                static_cast<float>( color [1] ),
                                static_cast<float>( color [2] ),
                                0.0
                            )
                            );

                        p_cpp->SetColor ( i, cmp.Get () );

                        color [1] += ratio;
                    }

                    //C2G
                    color [1] = 1.0;
                    color [2] = 1.0;

                    if ( between > 1 )
                    {
                        ColorMaterialPtr cmp
                            = ColorMaterialPtr (
                                omsl::materials::ColorMaterialRgbaFFactory (
                                static_cast<float>( color [0] ),
                                static_cast<float>( color [1] ),
                                static_cast<float>( color [2] ),
                                0.0
                            )
                            );

                        p_cpp->SetColor ( i, cmp.Get () );
                        i++;
                        end++;
                    }

                    end += step2;
                    for ( ; i < end; i++ )
                    {
                        ColorMaterialPtr cmp
                            = ColorMaterialPtr (
                                omsl::materials::ColorMaterialRgbaFFactory (
                                static_cast<float>( color [0] ),
                                static_cast<float>( color [1] ),
                                static_cast<float>( color [2] ),
                                0.0
                            )
                            );

                        p_cpp->SetColor ( i, cmp.Get () );

                        color [2] -= ratio;

                    }

                    //G2Y
                    color [1] = 1.0;
                    color [2] = 0.0;

                    if ( between == 1 || between == 3 )
                    {
                        ColorMaterialPtr cmp
                            = ColorMaterialPtr (
                                omsl::materials::ColorMaterialRgbaFFactory (
                                static_cast<float>( color [0] ),
                                static_cast<float>( color [1] ),
                                static_cast<float>( color [2] ),
                                0.0
                            )
                            );

                        p_cpp->SetColor ( i, cmp.Get () );
                        i++;
                        end++;
                    }

                    end += step2;
                    for ( ; i < end; i++ )
                    {
                        ColorMaterialPtr cmp
                            = ColorMaterialPtr (
                                omsl::materials::ColorMaterialRgbaFFactory (
                                static_cast<float>( color [0] ),
                                static_cast<float>( color [1] ),
                                static_cast<float>( color [2] ),
                                0.0
                            )
                            );

                        p_cpp->SetColor ( i, cmp.Get () );

                        color [0] += ratio;

                    }

                    // R2Y
                    color [0] = 1.0;
                    color [1] = 1.0;

                    if ( between > 1 )
                    {
                        ColorMaterialPtr cmp
                            = ColorMaterialPtr (
                                omsl::materials::ColorMaterialRgbaFFactory (
                                static_cast<float>( color [0] ),
                                static_cast<float>( color [1] ),
                                static_cast<float>( color [2] ),
                                0.0
                            )
                            );

                        p_cpp->SetColor ( i, cmp.Get () );
                        i++;
                        end++;
                    }

                    for ( ; i < size ; i++ )
                    {
                        ColorMaterialPtr cmp
                         = ColorMaterialPtr (
                            omsl::materials::ColorMaterialRgbaFFactory (
                            static_cast<float>( color [0] ),
                            static_cast<float>( color [1] ),
                            static_cast<float>( color [2] ),
                            0.0
                        )
                        );

                        p_cpp->SetColor ( i, cmp.Get () );

                        color [1] -= ratio;
                    }

                    //WHITE as default
                    ColorMaterialPtr cmp
                        = ColorMaterialPtr (
                            omsl::materials::ColorMaterialRgbaFFactory (
                            static_cast<float>( 1. ),
                            static_cast<float>( 1. ),
                            static_cast<float>( 1. ),
                            0.0
                        )
                        );

                    p_cpp->SetColor ( i, cmp.Get () );

                }
                catch ( ... )
                {
                }
                return __LINE__;
            }


            int setupColorIndexMaps ( std::map<double, omsl::ULong>& speedMap, double minspeed, double maxspeed, int type )
            {
                // calculate colors // to replace with reading the UI
                size_t size = speedMap.size ();

                double ratio = ( maxspeed - minspeed ) / size;
                if ( ratio > 0.0 )
                {
                    //setup color indizes;
                    for ( std::map<double, omsl::ULong>::iterator it1 = speedMap.begin (); it1 != speedMap.end (); it1++ )
                    {
                        ( *it1 ).second = ( int ) ( ( ( *it1 ).first - minspeed ) / ratio );
                        // to avoid broken limits
                        if ( ( *it1 ).second > size )
                            ( *it1 ).second = size + 1;
                        if ( ( *it1 ).second < 0 )
                            ( *it1 ).second = size + 1;
                    }
                }
                // fill the pointidmap to process with the TP segment
                for ( int i = 0; i < s_mapVector.size (); i++ )
                {
                    IndexMapStruct& segref = s_mapVector [i];
                    std::map<omsl::ULong, double>& speedRef = ( type == 0 ) ? segref.ptIndexFeedrateMap : segref.ptIndexSpeedMap;
                    std::map<omsl::ULong, omsl::ULong>&clrRef = ( type == 0 ) ? segref.ptIndexFClrIndexMap : segref.ptIndexSClrIndexMap;
//                    ULULMap& ucRef = ( type == 0 ) ? segref.pIFCMap : segref.pISCMap;
                    VectULongConcreteRandomReadOnlyPtr ucRef = ( type == 0 ) ? segref.pVecUL_F : segref.pVecUL_S;
                    clrRef.clear ();
                    ucRef->Clear ();

                    for ( std::map<omsl::ULong, double>::iterator it = speedRef.begin (); it != speedRef.end (); it++ )
                    {
                        size_t index = 0;
                        std::map<double, omsl::ULong>::iterator it2;

                        it2 = speedMap.find ( ( *it ).second );
                        if ( it2 != speedMap.end () )
                            index = ( *it2 ).second;
                        else
                        {
                            index = ( ( *it ).second < minspeed || ( *it ).second > maxspeed ) ? speedMap.size () : 0;
                        }

                        clrRef [( *it ).first] = index;
                        ucRef->PushBack ( ( *it ).first );
                        ucRef->PushBack ( index );

//                        ucRef.Insert ( ( *it ).first,index );
                    }
                }

                return 0;
            }

            int TPSpeedAnalysisModel::createPointIdMaps ( const tpanalysis::CadEntitiesIt_T& pTPEntsIt )
            {

                if ( !pTPEntsIt.Get () || pTPEntsIt->Size () == 0 )
                    return 0;
                //hcntcore::Model* model = ( hcntcore::Model* )m_pDocument->Cast ( L"Model" );
                omPofTechData pofdata;
                s_mapVector.clear ();
                std::map<double, omsl::ULong> feedrateMap,speedMap;
                if ( !pTPEntsIt.Get () )
                    return 0;
                s_maxspeed =-11111, s_minspeed = 99999999, s_maxfeed = -11111 , s_minfeed = 999999999;

                int tpindex = 0;
                for ( OMSL_EACH ( pTPEntsIt ) )
                {
                    SharedPtr<hcntcore::CadEntity> pEntTP ( pTPEntsIt->Get () );
                    ToolpathPtr pToolpath;

                    pToolpath = pEntTP.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );

                    CadEntityForwardPtr ents ( pToolpath->GetCadEntities () );
                    if ( !ents.Get () )
                    {
                        tpindex++;
                        continue;
                    }
                    int tpsegindex = 0;
                    for ( OMSL_EACH ( ents ) )
                    {
                        tpsegindex++;
                        CadEntityPtr cep ( ents->Get () );

                        if ( !cep.Get () )
                            continue;

                        ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep ) );

                        if ( !pSegment.Get () )
                            continue;

                        TopoGeomPtr pTopo = pSegment.shared_cast< omsl::geom::TopoGeom >( L"TopoGeom" );
                        if ( !pTopo || !pTopo->GetAttribs () )
                            continue;

                        SharedPtr<omsl::geom::Attribute>m_pAttr ( pTopo->GetAttribs ()->Get ( L"HM_ToolpathData" ) );
                        if ( !m_pAttr.Get () )
                            continue;

                        omsl::geom::AttributeRefCounter* pAttrRef = ( omsl::geom::AttributeRefCounter* ) m_pAttr->Cast ( L"AttributeRefCounter" );
                        if ( !pAttrRef )
                            continue;

                        const RefCounter * pRef = pAttrRef->GetValue ();
                        if ( !pRef )
                            continue;

                        ToolpathData* ptpData = ( ToolpathData * ) pRef->Cast ( L"ToolpathData" );
                        // error
                        if ( !ptpData )
                            continue;

                        omsl::ULong size = pSegment->Size ();
                        double speed = -11111;
                        double feedrate = -1111;
                        IndexMapStruct ims;
                        ims.tpIndex = tpindex;
                        ims.tpsegIndex = tpsegindex-1;

                        // store data for segment
                        if ( !ims.pVecUL_F.Get () )
                        {
                            ims.pVecUL_F = new omsl::container::VectULongConcreteRandomReadOnly;
                        }
                        if ( !ims.pVecUL_S.Get () )
                        {
                            ims.pVecUL_S = new omsl::container::VectULongConcreteRandomReadOnly;
                        }
                        //if ( !ims.pIFCMap.Get () )
                        //{
                        //    ims.pIFCMap = new ULULMap( std::map<omsl::ULong, omsl::ULong> );
                        //}
                        //if ( !ims.pISCMap.Get () )
                        //{
                        //    ims.pISCMap = new ULULMap(( std::map<omsl::ULong, omsl::ULong> );
                        //}

                        for ( omsl::ULong i = 0; i < size; i++ )
                        {
                            pofdata = ptpData->TechData ( (int)i );
                            if ( pofdata.gmotion == 0 )
                                continue;

                            pofdata.feedrate = ( int ) pofdata.feedrate;

                            // scan all feedrates/speeds over all TP and TPSeg
                            feedrateMap [pofdata.feedrate] = 1;
                            speedMap [pofdata.speed] = 1;

                            // store only changes
                            if ( feedrate != pofdata.feedrate )
                            {
                                feedrate = pofdata.feedrate;
                                ims.ptIndexFeedrateMap [i] = feedrate;

                                if ( feedrate < s_minfeed )
                                    s_minfeed = feedrate;
                                if ( feedrate > s_maxfeed )
                                    s_maxfeed = feedrate;
                            }
                            // store only changes
                            if ( speed != pofdata.speed )
                            {
                                speed = pofdata.speed;
                                ims.ptIndexSpeedMap [i] = speed;

                                if ( speed < s_minspeed )
                                    s_minspeed = speed;
                                if ( speed > s_maxspeed )
                                    s_maxspeed = speed;
                            }
                        }

                        s_mapVector.push_back ( ims );
                    }
                    tpindex++;
                }

                // results : a map over all speeds / feedrates
                setupColorIndexMaps ( feedrateMap, s_minfeed, s_maxfeed, 0 );
                setupColorIndexMaps ( speedMap, s_minspeed, s_maxspeed, 1 );
                // setup colormaps
                ColorsPalettePtr feedratecolor;
                ColorsPalettePtr speedcolor;
                ColorsPaletteCreateAsMap ( feedratecolor, (int)feedrateMap.size () );
                ColorsPaletteCreateAsMap ( speedcolor, ( int ) speedMap.size () );
                // missing : save old palette, save old indexmap
                // missing set new palette, set new indexmap
                int tpvecindex = 0;
                for ( OMSL_EACH ( pTPEntsIt ) )
                {
                    SharedPtr<hcntcore::CadEntity> pEntTP ( pTPEntsIt->Get () );
                    ToolpathPtr pToolpath;

                    pToolpath = pEntTP.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );

                    CadEntityForwardPtr ents ( pToolpath->GetCadEntities () );
                    if ( !ents.Get () )
                    {
                        continue;
                    }

                    for ( OMSL_EACH ( ents ) )
                    {
                        omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );

                        if ( !cep.Get () )
                            continue;

                        ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep ) );

                        if ( !pSegment.Get () )
                            continue;

                        GraphicAttributesPtr gattrp ( cep->GetGraphicAttributes () );
                        if ( !gattrp.Get () )
                            continue;

                        ToolpathSegmentGraphicAttributesPtr tggattrp = gattrp.shared_cast<omsl::graphicattributes::ToolpathSegmentGraphicAttributes>( L"ToolpathSegmentGraphicAttributes" );
                        if ( !tggattrp.Get () )
                            continue;

                        ColorsPalettePtr colorMap = ( GetValue ( tpanalysis::UI_RefSpeedType ) == 0 ) ? feedratecolor : speedcolor;

                        // ToDo save the old colormap
                        IndexMapStruct& segref = s_mapVector [tpvecindex];
                        std::map<omsl::ULong, omsl::ULong>& tMap = ( GetValue ( tpanalysis::UI_RefSpeedType ) == 0 ) ? segref.ptIndexFClrIndexMap : segref.ptIndexSClrIndexMap;

                        std::map<omsl::ULong, omsl::ULong>::iterator it;
                        std::map<omsl::ULong, omsl::ULong>::iterator endit = ( GetValue ( tpanalysis::UI_RefSpeedType ) == 0 ) ? segref.ptIndexFClrIndexMap.end () : segref.ptIndexSClrIndexMap.begin ();

                        VectULongConcreteRandomReadOnlyPtr ucRef = ( GetValue ( tpanalysis::UI_RefSpeedType ) == 0 ) ? segref.pVecUL_F : segref.pVecUL_S;
//                        pSegment->MapPointsToIdentifiers (ucRef->GetIterator());
                        pSegment->IdentifiersResize ( colorMap->Size () );
                        tggattrp->SetColorsPalette ( colorMap.Get () );
                        for ( int jj = 0; jj < colorMap->Size (); jj++ )
                        {
                            pSegment->SetIdentifier ( jj, jj );
                        }
                        pSegment->ResetPointToIdentifierMap ();
                        pSegment->IdentifiersReserve ( tMap.size () );

                        it = tMap.begin ();

                        //for ( int i = 0; i < pSegment->Size (); i++ )
                        //{
                        //    pSegment->MapPointToIdentifier ( i , ( *it ).second );
                        //    if ( i == ( *it ).first )
                        //        it++;
                        //}

                        for ( it = tMap.begin (); it != tMap.end (); it++ )
                        {
                            pSegment->MapPointToIdentifier ( ( *it ).first, ( *it ).second );
                        }

                        cep->UpdateGraphicAttributes ( gattrp.Get () );
//                        model->CadEntityMgr()->CreateCadEntityGraphic ( cep.Get () );
                        tpvecindex++;
                    }

                    pToolpath.Get ()->SetUpdateNeeded ( omsl::True );
                    tpindex++;
                }

                // toDo call graphics refresh
                return 0;
            }


            int TPSpeedAnalysisModel::setupToolpathColors ( const tpanalysis::CadEntitiesIt_T& pTPEntsIt )
            {

                std::map < double, int> speedColorIndexMap;
                ColorsPalettePtr   p_cpp_pl_clrs;
                MaterialPtr        p_mat_nv_clr;
                if ( getSpeedColorIndexMap ( speedColorIndexMap ) )
                    return -1;
                if ( !pTPEntsIt.Get () )
                    return 0;
                
                omPofTechData pofdata;
                double* pSpeed = ( GetValue ( tpanalysis::UI_RefSpeedType ) > 0 ) ? &pofdata.feedrate : &pofdata.speed;


                for ( OMSL_EACH ( pTPEntsIt ) )
                {
                    SharedPtr<hcntcore::CadEntity> pEntTP ( pTPEntsIt->Get () );
                    ToolpathPtr pToolpath;

                    pToolpath = pEntTP.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );

                    CadEntityForwardPtr ents ( pToolpath->GetCadEntities () );
                    if ( !ents.Get () )
                        continue;
                    for ( OMSL_EACH ( ents ) )
                    {
                        omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );

                        if ( !cep.Get () )
                            continue;

                        ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep ) );

                        if ( !pSegment.Get () )
                            continue;

                        GraphicAttributesPtr gap ( cep->GetGraphicAttributes () );
                        if ( !gap.Get () )
                            continue;

                        TopoGeomPtr pTopo = pSegment.shared_cast< omsl::geom::TopoGeom >( L"TopoGeom" );
                        if ( !pTopo || !pTopo->GetAttribs () )
                            continue;

                        SharedPtr<omsl::geom::Attribute>m_pAttr ( pTopo->GetAttribs ()->Get ( L"HM_ToolpathData" ));
                        if ( !m_pAttr.Get () )
                            continue;

                        omsl::geom::AttributeRefCounter* pAttrRef = ( omsl::geom::AttributeRefCounter* ) m_pAttr->Cast ( L"AttributeRefCounter" );
                        if ( !pAttrRef )
                            continue;

                        const RefCounter * pRef = pAttrRef->GetValue ();
                        if ( !pRef )
                            continue;

                        ToolpathSegmentGraphicAttributesPtr tpsgap = gap.shared_cast<omsl::graphicattributes::ToolpathSegmentGraphicAttributes>( L"ToolpathSegmentGraphicAttributes" );
                        if ( !tpsgap.Get () )
                            continue;

                        p_cpp_pl_clrs =  tpsgap->GetColorsPalette ();

                        CurveGraphicAttributesPtr cgap ( tpsgap->GetNormalsAttributes ( true ) );
                        if ( !cgap.Get () )
                            continue;

                        p_mat_nv_clr = cgap->GetMaterial ();


//                                    const omPofTechData* data = nullptr;
                       ToolpathData* ptpData = ( ToolpathData * ) pRef->Cast ( L"ToolpathData" );

                       if ( !ptpData )
                           continue;
                       
                       std::vector<int> pntidmap;
                       omsl::ULong size = pSegment->Size ();
                       double speed = -11111;
                       for ( omsl::ULong i = 0; i < size; i++ )
                       {
                           pofdata = ptpData->TechData ( ( int ) i );
                           if ( speed == *pSpeed )
                               continue;
                           speed = *pSpeed;

                           std::map < double, int >::iterator c_it = speedColorIndexMap.find ( speed );
                           
                           size_t _is = 0, s = speedColorIndexMap.size ();

                           if ( c_it == speedColorIndexMap.end () )
                           {
                               for ( c_it = speedColorIndexMap.begin (); c_it != speedColorIndexMap.end () ; c_it++ , _is++ )
                               {
                                   if ( speed < ( *c_it ).first )
                                       break;
                                   if ( _is + 1 == s ) // to have the last entry
                                       break;
                               }

                               std::pair<double, int> p ( speed, ( *c_it ).second );
                               speedColorIndexMap.insert (p);
                               c_it = speedColorIndexMap.find ( speed );
                           }
                           if ( c_it == speedColorIndexMap.end () )
                               continue;
                           int clrIx = ( *c_it ).second;

                           pSegment->MapPointToIdentifier ( i > 0 ? i - 1 : 0, (omsl::ULong)clrIx );
                           pntidmap.push_back ( i > 0 ? (int)i - 1 : 0 );
                           pntidmap.push_back ( clrIx );
                       }

                       tpsgap->SetColorsPalette ( p_cpp_pl_clrs.Get () );

                       cgap->SetMaterial ( p_mat_nv_clr.Get () );

                    }
                }

                return 0;
            }

            int TPSpeedAnalysisModel::HighlightToolPath ( const CadEntitiesIt_T& pTPEntsIt )
            {
                callStackString ( L"HighlightToolPath", 1 );
                int counts = 0;
//                setupToolpathColors ( pTPEntsIt );
                createPointIdMaps ( pTPEntsIt );
                ////SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPntVec;
                ////SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPntVec1;
                ////SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPntVec2;

                //tpanalysis::Coord3dIt_T pPoint;
                //std::map<int, int> rsMap;
                //if ( pTPEntsIt.Get () )
                //{
                //    int i = 0;
                //    for ( OMSL_EACH ( pTPEntsIt ) )
                //    {
                //        rsMap [i] = 0;
                //        i++;
                //    }
                //}

                //int viewModeParameter = GetValue ( tpanalysis::viewModeParameter );

                //SetValue ( tpanalysis::TGr_PreviewPoints_Ent1, pPoint );
                //SetValue ( tpanalysis::TGr_PreviewPoints_Ent2, pPoint );

                //SetSignalListening ( false );

                //if ( !m_data.pTpSelectionVec.Get () )
                //{
                //    m_data.pTpSelectionVec = new omsl::container::hcntcore::VectorCadEntityPtrConcreteForward;
                //}
                //std::vector<unsigned int> tviewVec;
                //// restore settings
                //if ( m_data.pTpSelectionVec.Get () && m_data.pTpSelectionVec->Size () )
                //{
                //    int i = 0;
                //    omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> _it ( m_data.pTpSelectionVec.Get () );
                //    for ( OMSL_EACH ( _it ) )
                //    {
                //        SharedPtr<hcntcore::CadEntity> pEntTP ( _it->Get () );
                //        ToolpathPtr pToolpath;

                //        pToolpath = pEntTP.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );

                //        if ( pTPEntsIt.Get () )
                //        {
                //            int ii = 0;
                //            for ( OMSL_EACH ( pTPEntsIt ) )
                //            {
                //                SharedPtr<hcntcore::CadEntity> pEntTPold ( pTPEntsIt->Get () );
                //                if ( pEntTPold.Get () == pEntTP.Get () )
                //                {
                //                    rsMap [ii] = 1;
                //                    ii = -1;
                //                    if ( pToolpath.Get () )
                //                    {
                //                        int sz = pToolpath->Size ();
                //                        for ( int j = 0; j < sz; j++ )
                //                        {
                //                            tviewVec.push_back ( m_data.viewVec [i] );
                //                            i++;
                //                        }
                //                    }
                //                    break;
                //                }
                //                ii++;
                //            }
                //            if ( ii == -1 )
                //                continue;
                //        }

                //        if ( pToolpath.Get () ) // selected an Toolpath
                //        {
                //            GraphicAttributesPtr gattrp1 ( pToolpath->GetGraphicAttributes () );
                //            if ( !gattrp1.Get () )
                //                continue;
                //            ToolpathGraphicAttributesPtr tggattrp1 = gattrp1.shared_cast< omsl::graphicattributes::ToolpathGraphicAttributes >( L"ToolpathGraphicAttributes" );
                //            if ( !tggattrp1.Get () )
                //                continue;

                //            omsl::graphicattributes::Status stat;

                //            omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> ents ( pToolpath->GetCadEntities () );
                //            if ( !ents.Get () )
                //                continue;

                //            //                            int err = 0;
                //            stat = omsl::graphicattributes::OK;
                //            for ( OMSL_EACH ( ents ) )
                //            {
                //                omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );

                //                if ( !cep.Get () )
                //                    continue;

                //                ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep ) );

                //                GraphicAttributesPtr gattrp ( cep->GetGraphicAttributes () );
                //                if ( !gattrp.Get () )
                //                    continue;

                //                ToolpathSegmentGraphicAttributesPtr tggattrp = gattrp.shared_cast<omsl::graphicattributes::ToolpathSegmentGraphicAttributes>( L"ToolpathSegmentGraphicAttributes" );
                //                if ( !tggattrp.Get () )
                //                    continue;

                //                // reset colors
                //                tggattrp->GetColorsPalette ()->SetColor ( 0, const_cast < omsl::materials::ColorMaterial* > ( tggattrp->GetColorsPalette ()->GetColor ( 4 ) ) );
                //                tggattrp->GetColorsPalette ()->SetColor ( 1, const_cast < omsl::materials::ColorMaterial* > ( tggattrp->GetColorsPalette ()->GetColor ( 5 ) ) );

                //                // reset original data

                //                if ( i < m_data.pointSizeVec.size () )
                //                {
                //                    PointCloudGraphicAttributesPtr pcgattr ( tggattrp->GetPointsAttributes () );

                //                    if ( pcgattr.Get () )
                //                        pcgattr->SetGraphicWidth ( m_data.pointSizeVec [i] );
                //                }

                //                if ( i < m_data.viewVec.size () )
                //                {
                //                    stat = tggattrp->SetShowPoints ( ( m_data.viewVec [i] & analyseData1::dai_viewOrigPoints ) ? omsl::True : omsl::False );
                //                    stat = tggattrp->SetShowNormals ( ( m_data.viewVec [i] & analyseData1::dai_viewOrigNormals ) ? omsl::True : omsl::False );
                //                }

                //                cep->UpdateGraphicAttributes ( gattrp.Get () );

                //                i++;
                //            }

                //            pToolpath->UpdateGraphicAttributes ( gattrp1.Get () );
                //            pToolpath.Get ()->SetUpdateNeeded ( omsl::True );
                //        }
                //    }

                //    auto &vecRef = m_data.pTpSelectionVec->GetContainerReference ();
                //    vecRef.clear ();
                //}
                //// to store temporary viewdata

                //if ( !pTPEntsIt || !pTPEntsIt->Size () )
                //{
                //    ResetPreviewData ( analyseData1::dai_Index1stTpEnt );
                //    ResetPreviewData ( analyseData1::dai_Index1stTpSeg );
                //    SetupTemporaryGraphicsPickPoints ( viewModeParameter );
                //    SetupTemporaryGraphicsDirection ( viewModeParameter );
                //    SetupCurrentToolDirection ( viewModeParameter );
                //    SetupTemporaryGraphicsLabel ( viewModeParameter );
                //    SetupTemporaryGraphicsVector ( viewModeParameter );
                //    SetSignalListening ( true );
                //    callStackString ( L"return 1", -1 );
                //    return 0;
                //}
                //else
                //{
                //    ResetPreviewData ( analyseData1::dai_Index1stTpEnt );
                //}

                //if ( pTPEntsIt.Get () )
                //{
                //    int i = 0, ii = 0, jj = 0;
                //    for ( OMSL_EACH ( pTPEntsIt ) )
                //    {
                //        SharedPtr<hcntcore::CadEntity> pEntTP ( pTPEntsIt->Get () );

                //        m_data.pTpSelectionVec->PushBack ( pTPEntsIt->Get () );

                //        ToolpathPtr pToolpath;

                //        pToolpath = pEntTP.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );

                //        if ( rsMap [ii] == 1 )
                //        {
                //            ii++;
                //            if ( pToolpath.Get () )
                //            {
                //                int sz = pToolpath->Size ();
                //                for ( int j = 0; j < sz; j++ )
                //                {
                //                    m_data.viewVec.push_back ( tviewVec [jj] );
                //                    jj++;
                //                    i++;
                //                }
                //            }
                //            continue;
                //        }


                //        if ( pToolpath.Get () ) // selected an Toolpath
                //        {
                //            GraphicAttributesPtr gattrp1 ( pToolpath->GetGraphicAttributes () );
                //            if ( !gattrp1.Get () )
                //                continue;
                //            ToolpathGraphicAttributesPtr tggattrp1 = gattrp1.shared_cast< omsl::graphicattributes::ToolpathGraphicAttributes >( L"ToolpathGraphicAttributes" );
                //            if ( !tggattrp1.Get () )
                //                continue;

                //            omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> ents ( pToolpath->GetCadEntities () );
                //            if ( !ents.Get () )
                //                continue;

                //            //                            int err = 0;
                //            omsl::graphicattributes::Status stat;
                //            stat = omsl::graphicattributes::OK;

                //            for ( ents->MoveToBegin (); !ents->IsAtEnd (); ents->Next () )
                //            {
                //                omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );
                //                if ( !cep.Get () )
                //                    continue;

                //                ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep ) );

                //                GraphicAttributesPtr gattrp ( cep->GetGraphicAttributes () );
                //                if ( !gattrp.Get () )
                //                    continue;

                //                ToolpathSegmentGraphicAttributesPtr tggattrp = gattrp.shared_cast<omsl::graphicattributes::ToolpathSegmentGraphicAttributes>( L"ToolpathSegmentGraphicAttributes" );
                //                if ( !tggattrp.Get () )
                //                    continue;

                //                omsl::materials::ColorsPalette* p = tggattrp->GetColorsPalette ();

                //                const omsl::materials::ColorMaterial* c0 = tggattrp->GetColorsPalette ()->GetColor ( 0 );
                //                const omsl::materials::ColorMaterial* c1 = tggattrp->GetColorsPalette ()->GetColor ( 1 );
                //                const omsl::materials::ColorMaterial* c2 = tggattrp->GetColorsPalette ()->GetColor ( 2 );
                //                const omsl::materials::ColorMaterial* c3 = tggattrp->GetColorsPalette ()->GetColor ( 3 );
                //                const omsl::materials::ColorMaterial* c4 = tggattrp->GetColorsPalette ()->GetColor ( 4 );
                //                const omsl::materials::ColorMaterial* c5 = tggattrp->GetColorsPalette ()->GetColor ( 5 );

                //                tggattrp->GetColorsPalette ()->SetColor ( 0, const_cast < omsl::materials::ColorMaterial* > ( tggattrp->GetColorsPalette ()->GetColor ( 2 ) ) );
                //                tggattrp->GetColorsPalette ()->SetColor ( 1, const_cast < omsl::materials::ColorMaterial* > ( tggattrp->GetColorsPalette ()->GetColor ( 3 ) ) );

                //                const omsl::materials::ColorMaterial* c10 = tggattrp->GetColorsPalette ()->GetColor ( 0 );
                //                const omsl::materials::ColorMaterial* c11 = tggattrp->GetColorsPalette ()->GetColor ( 1 );
                //                const omsl::materials::ColorMaterial* c12 = tggattrp->GetColorsPalette ()->GetColor ( 2 );
                //                const omsl::materials::ColorMaterial* c13 = tggattrp->GetColorsPalette ()->GetColor ( 3 );

                //                // store the original viewmodes

                //                int view = 0;
                //                view |= tggattrp->GetShowPoints () == omsl::True ? analyseData1::dai_viewOrigPoints : 0;
                //                view |= tggattrp->GetShowNormals () == omsl::True ? analyseData1::dai_viewOrigNormals : 0;

                //                m_data.viewVec.push_back ( view );


                //                // set the temporary view modes

                //                stat = tggattrp->SetShowPoints ( omsl::True );

                //                if ( viewModeParameter&vm_SHOWVECTORS )
                //                { // modify palette ???
                //                    tggattrp->SetShowNormals ( omsl::True );
                //                }
                //                else
                //                {// modify palette ???
                //                    tggattrp->SetShowNormals ( omsl::False );
                //                }

                //                PointCloudGraphicAttributesPtr pcgattr ( tggattrp->GetPointsAttributes () );

                //                if ( pcgattr.Get () )
                //                {
                //                    double width = pcgattr->GetGraphicWidth ();

                //                    m_data.pointSizeVec.push_back ( width );

                //                    pcgattr->SetGraphicWidth ( width*2.0 );
                //                }

                //                cep->UpdateGraphicAttributes ( gattrp.Get () );

                //                i++;
                //            }

                //            ii++;

                //            pToolpath->UpdateGraphicAttributes ( gattrp1.Get () );

                //            pToolpath.Get ()->SetUpdateNeeded ( omsl::True );
                //        }
                //    }
                //}

                //{

                //    ToolpathPtr pToolpath;

                //    auto &vecRef = m_data.pTpSelectionVec->GetContainerReference ();

                //    if ( m_data.specialIndex [analyseData1::dai_Index1stTpEnt] > -1 && m_data.specialIndex [analyseData1::dai_Index1stTpEnt] < vecRef.size () )
                //    {
                //        pToolpath = vecRef [m_data.specialIndex [analyseData1::dai_Index1stTpEnt]].shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );
                //    }

                //    if ( pToolpath.Get () )
                //    {
                //        CadEntitiesIt_T ents ( pToolpath->GetCadEntities () );
                //        ToolpathSegmentPtr pToolpathSegment;

                //        if ( !ents.Get () )
                //        {
                //            callStackString ( L"return 1", -1 );
                //            return counts;
                //        }
                //        m_data.segmentSizeVec.clear ();
                //        size_t s = 0;

                //        for ( ents->MoveToBegin (); !ents->IsAtEnd (); ents->Next () )
                //        {
                //            m_data.segmentSizeVec.push_back ( s );

                //            omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );
                //            pToolpathSegment = CastToToolpathSegment ( cep );
                //            if ( !pToolpathSegment.Get () )
                //                continue;

                //            s += pToolpathSegment->Size ();
                //        }

                //        m_data.segmentSizeVec.push_back ( s );
                //    }
                //}

                ////tpanalysis::Coord3dIt_T pPointIt ( pPntVec.get () );
                ////// only if new data

                ////pPointIt = pPntVec1.get ();
                ////pPointIt = pPntVec2.get ();

                //SetValue ( tpanalysis::DummyParameter, ( int ) counts );
                //SetValue ( tpanalysis::ToolpathCounter, ( int ) m_data.pTpSelectionVec->Size () );

                //SetSignalListening ( true );

                //SetupTemporaryGraphicsLabel ( viewModeParameter );
                //SetupTemporaryGraphicsDirection ( viewModeParameter );
                //SetupTemporaryGraphicsPickPoints ( viewModeParameter );
                //SetupTemporaryGraphicsVector ( viewModeParameter );
                //SetupCurrentToolDirection ( viewModeParameter );
                callStackString ( L"return 0", -1 );
                return counts;
            }



        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
