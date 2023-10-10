#pragma warning( disable : 4996 ) // for warnings: due to compare of boost::uuids
#include "TPAnalysisModel.h"
// keys
#include <Commands\tpanalysiskeys.h>
#include <Commands\tpanalysepropertieskeys.h>
// signals
#include "../Commands/tp_listener.h"
#include "../Commands/tp_signalemitter.h"
#include "../Commands/tpanalysepropertiesimpl.h"
#include "hcntLib/HCNTCoreHlp.h"
#include "hcntLib/BGeoHlp.h"
#include <hcntLib/ContainersHlp.h>
#include <hcntLib/ToolPathHlp.h>
#include <iBSys\icdCtPathEnums.h>
#include <tchar.h>
#include <geom/toolpathtrimmingdata.h>

using namespace omsl::command;

typedef omsl::SharedPtr<omsl::hcntcore::CadEntity> CadEntityPtr;
typedef omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> CadEntityForwardPtr;
typedef omsl::SharedPtr<omsl::geom::ToolpathSegment> ToolpathSegmentPtr;
typedef omsl::SharedPtr<omsl::hcntcore::ToolPathEntity> ToolpathPtr;
typedef omsl::SharedPtr<omsl::hcntcore::GeomEntity> GeomEntityPtr;
typedef omsl::SharedPtr<omsl::geom::Geom3d> Geom3dPtr;
typedef omsl::SharedPtr<omsl::geom::ToolpathTrimmingDataMemento> MementoPtr;
typedef omsl::SharedPtr<omsl::geom::ToolpathTrimmingData> TpSegTrimmDataPtr;

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {

            //3rd tab new
            OMSL_IMPL ( void ) TPAnalysisModel::OnChangePointSize ( const double& d)
            {
                if ( SignalListeningIsEnabled () )
                    omsl::hmbroadcast::EmitTPSignal ( L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCM_ChangeGraphicSize, nullptr, eCtPointSize, d );
            }

            OMSL_IMPL ( void ) TPAnalysisModel::OnChangeLineWidth ( const double& d)
            {
                if ( SignalListeningIsEnabled () )
                    omsl::hmbroadcast::EmitTPSignal ( L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCM_ChangeGraphicSize, nullptr, eCtLineWidth, d );
            }

            OMSL_IMPL ( void ) TPAnalysisModel::OnChangeVectorLength ( const double& d)
            {
                if ( SignalListeningIsEnabled () )
                    omsl::hmbroadcast::EmitTPSignal ( L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCM_ChangeGraphicSize, nullptr, eCtVectorLength, d );
            }

            OMSL_IMPL ( void ) TPAnalysisModel::OnChangeEnvMinTolRadius ( const double& d )
            {
                wchar_t t [100];
                swprintf ( t, L"OM_TMPGRMINRAD=%lf", d );
                _tputenv ( t );
            }

            OMSL_IMPL ( void ) TPAnalysisModel::OnChangeEnvTolerance ( const double& d )
            {
                wchar_t t [100];
                swprintf ( t, L"OM_TMPGRTOL=%lf", d );
                _tputenv ( t );
            }

            OMSL_IMPL ( void ) TPAnalysisModel::OnChangeEnvClrMapByCounts ( const omsl::Bool& /*b*/ )
            {
                SetupExtremaToUI ( -99 );
            }

            OMSL_IMPL ( void ) TPAnalysisModel::OnLayerVisisbilityChanged ( const omsl::Bool& b, const int tpTpType )
            {


                int val = GetValue ( tpanalysis::UI_Env_ShowAdditionalPathes );
                if ( b )
                    val |= (1 << tpTpType);
                else
                    val &= ~( 1 << tpTpType );

                SetValue ( tpanalysis::UI_Env_ShowAdditionalPathes, ( int ) val );

                if ( SignalListeningIsEnabled () == false )
                    return;

                omsl::hmbroadcast::EmitTPSignal ( L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCM_ShowHideTpType, nullptr, val, 0.0 );
            }

            OMSL_IMPL ( void ) TPAnalysisModel::OnShowLayer25Changed ( const omsl::Bool& b)
            {
                bool internB = b;
                omsl::commands::tp_analyzer::Properties* tpp = GetProperties ();
                if ( tpp )
                {
                    omsl::commands::tp_analyzer::PropertiesImpl* p = dynamic_cast < omsl::commands::tp_analyzer::PropertiesImpl* > ( tpp );

                    if ( p->IsInactive ( properties::key::ShowLayer25 ) )
                    {
                        internB = false;
                    }
                }

                if ( !SignalListeningIsEnabled () )
                {
                    SetValue ( tpanalysis::sl25_cb_min, GetValue ( tpanalysis::UI_AutoUpdateMin ) );
                    SetValue ( tpanalysis::sl25_cb_max, GetValue ( tpanalysis::UI_AutoUpdateMax ) );
                    SetValue ( tpanalysis::sl25_cb_lvl, GetValue ( tpanalysis::UI_AutoUpdateLvl ) );
                    SetValue ( tpanalysis::sl25_cb_clr, GetValue ( tpanalysis::UI_ClearanceLevel ) );
                    SetValue ( tpanalysis::sl25_in_min, GetValue ( tpanalysis::UI_feedrateValueMin ) );
                    SetValue ( tpanalysis::sl25_in_max, GetValue ( tpanalysis::UI_feedrateValueMax ) );
                    SetValue ( tpanalysis::sl25_in_lvl, GetValue ( tpanalysis::UI_feedrateLevels ) );
                    return;
                }

                SetValue ( tpanalysis::updateColorsTableAllowed,0 );

                if ( internB )
                {
                    // save old state
                    SetValue ( tpanalysis::sl25_cb_min, GetValue ( tpanalysis::UI_AutoUpdateMin ) );
                    SetValue ( tpanalysis::sl25_cb_max, GetValue ( tpanalysis::UI_AutoUpdateMax ) );
                    SetValue ( tpanalysis::sl25_cb_lvl, GetValue ( tpanalysis::UI_AutoUpdateLvl ) );
                    SetValue ( tpanalysis::sl25_cb_clr, GetValue ( tpanalysis::UI_ClearanceLevel ) );
                    SetValue ( tpanalysis::sl25_in_min, GetValue ( tpanalysis::UI_feedrateValueMin ) );
                    SetValue ( tpanalysis::sl25_in_max, GetValue ( tpanalysis::UI_feedrateValueMax ) );
                    SetValue ( tpanalysis::sl25_in_lvl, GetValue ( tpanalysis::UI_feedrateLevels ) );

                    SetValue ( tpanalysis::UI_AutoUpdateMin ,0);
                    SetValue ( tpanalysis::UI_AutoUpdateMax ,0);
                    SetValue ( tpanalysis::UI_AutoUpdateLvl ,0);
                    SetValue ( tpanalysis::UI_ClearanceLevel,0);
                }
                else // restore values
                {
                    SetValue ( tpanalysis::UI_AutoUpdateMin, GetValue ( tpanalysis::sl25_cb_min ) );
                    SetValue ( tpanalysis::UI_AutoUpdateMax, GetValue ( tpanalysis::sl25_cb_max ) );
                    SetValue ( tpanalysis::UI_AutoUpdateLvl, GetValue ( tpanalysis::sl25_cb_lvl ) );
                    SetValue ( tpanalysis::UI_ClearanceLevel, GetValue ( tpanalysis::sl25_cb_clr ) );
                    SetValue ( tpanalysis::UI_feedrateValueMin, GetValue ( tpanalysis::sl25_in_min ) );
                    SetValue ( tpanalysis::UI_feedrateValueMax, GetValue ( tpanalysis::sl25_in_max ) );
                    SetValue ( tpanalysis::UI_feedrateLevels, GetValue ( tpanalysis::sl25_in_lvl ) );
                }

                // disable all feedrate related checkboxes
                SetValue ( tpanalysis::showLayer25, (int) internB );

                OnChangeEnvUIClrMapByAnalyseData ( internB );

                //OnLayerVisisbilityChanged ( b, eCtTpType::eCt_tp );
            }

            OMSL_IMPL ( void ) TPAnalysisModel::OnShowNCPathChanged ( const omsl::Bool& b)
            {
                OnLayerVisisbilityChanged ( b, eCtTpType::eCt_ncp );
            }

            OMSL_IMPL ( void ) TPAnalysisModel::OnShowBallMillSpecPathChanged( const int& b)
            {
                int val = GetValue(tpanalysis::UI_Env_ShowAdditionalPathes);
                int oldval = val;
                switch (b)
                {
                    case properties::key::eBallMill::kJobsetting:
                    {
                        val &= ~(1 << eCtTpType::eCt_ccp);
                        val &= ~(1 << eCtTpType::eCt_UseUISetting);
                        val |= (1 << eCtTpType::eCt_tp);
                        break;
                    }
                    case properties::key::eBallMill::kCentered:
                    {
                        val &= ~(1 << eCtTpType::eCt_tp);
                        val |= (1 << eCtTpType::eCt_ccp);
                        val |= (1 << eCtTpType::eCt_UseUISetting);
                        break;
                    }
                    case properties::key::eBallMill::kTipped:
                    {
                        val &= ~(1 << eCtTpType::eCt_ccp);
                        val |= (1 << eCtTpType::eCt_tp);
                        val |= (1 << eCtTpType::eCt_UseUISetting);
                        break;
                    }
                    case properties::key::eBallMill::kBoth :
                    {
                        val |= (1 << eCtTpType::eCt_tp);
                        val |= (1 << eCtTpType::eCt_ccp);
                        val |= (1 << eCtTpType::eCt_UseUISetting);
                        break;
                    }
                }

                SetValue(tpanalysis::UI_Env_ShowAdditionalPathes, (int)val);

                if (SignalListeningIsEnabled() == false)
                    return;


                omsl::hmbroadcast::EmitTPSignal(L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCM_ShowHideTpType, nullptr, val, 0.0);

                SetupTemporaryGraphicsLabel(oldval);

            }

            OMSL_IMPL ( void ) TPAnalysisModel::OnShowClippPathChanged ( const omsl::Bool& b)
            {
                OnLayerVisisbilityChanged ( b, eCtTpType::eCt_clp );
            }

//            OMSL_IMPL ( void ) TPAnalysisModel::OnHideUpperLimitFeedratesChanged ( const omsl::Bool& /*b*/)
//            {
////                SetValue ( tpanalysis::UI_Env_HideUpperLimitFeedrates, ( int ) b );
//            }
//
//            OMSL_IMPL ( void ) TPAnalysisModel::OnHideLowerLimitFeedratesChanged ( const omsl::Bool& /*b*/ )
//            {
////                SetValue ( tpanalysis::UI_Env_HideLowerLimitFeedrates, ( int ) b );
//            }

            OMSL_IMPL ( void ) TPAnalysisModel::OnChangeEnvUIClrMapByAnalyseData ( const omsl::Bool& b )
            {
                if ( b )
                    s_ftype = 1;
                else
                    s_ftype = 0;
                ToolPathPtrgraphDispValMap::iterator it = s_data.pToolpathMap.begin ();
                for ( ; it != s_data.pToolpathMap.end (); it++ )
                {
                    ReplaceExtrema ( ( *it ).first.Get () );
                }

                SetupExtremaToUI ( -99 );
            }

            int TPAnalysisModel::OnSetupGeneralTab ( const int& UI_Tab_General )
            {
                //int vm = GetViewModeValue ();

                switch ( UI_Tab_General )
                {
                    case 0 : // TPA
                    {
                        break;
                    }
                    case 1: // FRA
                    {
                        break;
                    }
                    case 2: // settings
                    {
                        break;
                    }
                }

                return UI_Tab_General;
            }

            bool TPAnalysisModel::AddTP2Map ( ToolpathPtr& p )
            {
                ToolPathPtrgraphDispValMap::iterator it = s_data.pToolpathMap.end ();
                if ( s_data.pToolpathMap.size () )
                    it = s_data.pToolpathMap.find ( p );
                if ( it == s_data.pToolpathMap.end () )
                {
                    graphDispVal val;
                    val.dirPos1 = -1;
                    val.dirPos2 = -1;
                    val.cnt = 1;
                    val.tptype1 = -1;
                    val.tptype2 = -1;
                    val.tptypeMain = -1;
                    // get size
                    s_data.pToolpathMap [p] = val;
                }
                else
                {
                    ( *it ).second.cnt++;
                    return false;
                }

                return true;
            }

            bool TPAnalysisModel::RemoveTPfromMap ( ToolpathPtr& p )
            {
                ToolPathPtrgraphDispValMap::iterator it = s_data.pToolpathMap.find ( p );
                if ( it == s_data.pToolpathMap.end () )
                {
                    return false;
                }
                else
                {
                    s_data.pToolpathMap.erase ( it );
                }

                return true;
            }

            int TPAnalysisModel::OnChangeRangeSlider ( const int& /*d*/ )
            {
                int slider = 0;
                int dMin = GetValue ( tpanalysis::UI_sliderValueMin );
                int dMax = GetValue ( tpanalysis::UI_sliderValueMax );

                int gap = dMax - dMin;

                if ( m_waitForCadShell == true )
                    return gap;

                if ( dMax < s_data.m[slider].specialIndex [analyseData::dai_SXP1] || s_data.m[slider].specialIndex [analyseData::dai_SXP1] < dMin )
                {
                    CadEntitiesIt_T pEmptyEnts;
                    SharedPtr<hcntcore::CadEntity> pEmptyEnt;

                    //SetValue ( tpanalysis::UI_RefPoint, pEmptyEnt );
                    SetValue ( tpanalysis::UI_FirstEntities, pEmptyEnts );
                    SetValue ( tpanalysis::UI_SecondEntities, pEmptyEnts );
                    s_data.m[slider].specialIndex [analyseData::dai_SXP1] = UNKNOWNINDEX;
                    s_data.m[slider].specialIndex [analyseData::dai_SXP2] = UNKNOWNINDEX;

                    SetupPropertyBag ();
                }

                bool old = SignalListeningIsEnabled ();
                if ( old == false )
                    return gap;

                int maxmax = GetValue ( tpanalysis::UI_sliderMax_2 );

                if ( maxmax - dMax < MINIMUMSEGGAP && gap < MINIMUMSEGGAP )
                {
                    omsl::hmbroadcast::EmitTPSignal ( L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCM_LowerVisibilityIndex, nullptr, dMax - MINIMUMSEGGAP, 0.0 );
                }
                else
                {
                    omsl::hmbroadcast::EmitTPSignal ( L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCM_LowerVisibilityIndex, nullptr, dMin, 0.0 );
                }

                if ( gap < MINIMUMSEGGAP )
                    omsl::hmbroadcast::EmitTPSignal ( L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCM_UpperVisibilityIndex, nullptr, dMax + MINIMUMSEGGAP, 0.0 );
                else
                    omsl::hmbroadcast::EmitTPSignal ( L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCM_UpperVisibilityIndex, nullptr, dMax, 0.0 );

//                const int vm = GetViewModeValue ();

                SetupTemporaryGraphicsLabel ( 0 );
//                SetupTemporaryGraphicsDirection ( 0 );

                return gap;
            }

            int TPAnalysisModel::CheckLeftPick ( const SharedPtr<hcntcore::CadEntity> & pPoint ) // Slider
            {
                if ( SignalListeningIsEnabled () == false )
                    return 1;

                if ( !pPoint.Get () )
                {
                    return 0;
                }

                s_data.m[MASTER_TP].points [analyseData::dai_leftPick] = GetPoint ( pPoint, analyseData::dai_leftPick, MASTER_TP );

                SetValue ( tpanalysis::UI_sliderValueMin, s_data.m[MASTER_TP].specialIndex [analyseData::dai_leftPick] );
                OnChangeRangeSlider ( 0 );
                // to avoid double checks, reset pickpoints
                {
                    bool old = SetSignalListening ( false );
                    int val = GetValue ( tpanalysis::resetPickPointSelection );
                    SetValue ( tpanalysis::resetPickPointSelection, val++ );

                    SetSignalListening ( old );
                }

                return 1;
            }

            int TPAnalysisModel::CheckRightPick ( const SharedPtr<hcntcore::CadEntity>& pPoint ) // slider
            {
                if ( SignalListeningIsEnabled () == false )
                    return 1;

                if ( !pPoint.Get () )
                {
                    return 0;
                }

                s_data.m[MASTER_TP].points [analyseData::dai_rightPick] = GetPoint ( pPoint, analyseData::dai_rightPick, 0 );

                SetValue ( tpanalysis::UI_sliderValueMax, s_data.m[MASTER_TP].specialIndex [analyseData::dai_rightPick]+1 );
                OnChangeRangeSlider ( 0 );
                {
                    bool old = SetSignalListening ( false );
                    int val = GetValue ( tpanalysis::resetPickPointSelection );
                    SetValue ( tpanalysis::resetPickPointSelection, val++ );

                    SetSignalListening ( old );
                }

                return 1;
            }

            void TPAnalysisModel::SetupAdditionalCheckboxes ( int content )
            {
//                content = -1;
                bool ncpath = false, mainpath = false, clippath = false, layer25 = false, ccpath=false;

                if ( content&( 0x0001 << eCt_layer25 ) )
                    layer25 = true;
                if (content & (0x0001 << eCt_tp))
                    mainpath = true;
                if ( content&( 0x0001 << eCt_clp ) )
                    clippath = true;
                if ( content&( 0x0001 << eCt_ncp ) )
                    ncpath = true;
                if ( content&( 0x0001 << eCt_ccp ) )
                    ccpath = true;

                omsl::commands::tp_analyzer::Properties* tpp = GetProperties ();
                if ( tpp )
                {
                    tpp->SetStrength ( properties::key::ShowNCPath, ncpath ? omsl::propertyservice::Applicable::Strength::kOptional : omsl::propertyservice::Applicable::Strength::kInactive );
                    tpp->SetStrength ( properties::key::ShowClippPath, clippath ? omsl::propertyservice::Applicable::Strength::kOptional : omsl::propertyservice::Applicable::Strength::kInactive );
                    tpp->SetStrength ( properties::key::ShowLayer25, layer25 ? omsl::propertyservice::Applicable::Strength::kOptional : omsl::propertyservice::Applicable::Strength::kInactive );
                    tpp->SetStrength ( properties::key::ShowBallMillSpecPath, ccpath ? omsl::propertyservice::Applicable::Strength::kOptional : omsl::propertyservice::Applicable::Strength::kInactive );
                }
            }
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl