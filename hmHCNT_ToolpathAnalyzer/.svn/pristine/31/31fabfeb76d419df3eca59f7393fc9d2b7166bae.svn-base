#pragma warning( disable : 4996 ) // for warnings: due to compare of boost::uuids
#include "TPAnalysisModel.h"
// keys
#include <Commands\tpanalysiskeys.h>
#include <..\..\..\INTERFACES\iHCNT\iTopoAttrib.h>
#include <hcntcore/cadentityiterators.h>
#include <hcntcore/cadentitycontainers.h>
#include <hcntcore/cadentitycontainersinterfaces.h>

#include <hcntcore/polylineithpoint.h>
#include <hcntcore/toolpathmovement.h>
#include <hcntcore/pargeomentity.h>
#include <hcntcore/geomentity.h>
#include <hcntcore/cadentitymgr.h>
// Document
#include <hcntcore/document.h>
#include <hcntcore/model.h>



#include <graphicattributes/toolpathgraphicattributes.h>
#include <graphicattributes/toolpathsegmentgraphicattributes.h>
#include <graphicattributes/pointcloudgraphicattributes.h>
#include <materials/colormaterial.h>
#include <materials/colorspalette.h>
#include <materials/colorspalettefactory.h>
#include <materials/material.h>
#include <materials/materials.h>
#include <materials/materialslibrary.h>
#include <materials/materialsstatus.h>
#include <graphics/group.h>

#include "selectionservice/entityselector.h"
#include "selectionservice/interactiveselector.h"
#include "selectionservice/selectionservice.h"
#include "selectionservice/eventemitters.h"

#include <commands/tp_listener.h>
#include <commands/tp_signalemitter.h>

#include "../Commands/common_functions.h"

#include "hcntLib/containernew.h"


typedef omsl::SharedPtr<omsl::hcntcore::GeomEntity> GeomEntityPtr;
typedef omsl::SharedPtr<omsl::geom::Geom3d> Geom3dPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::GraphicAttributes> GraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::materials::ColorMaterial> ColorMaterialPtr;
typedef omsl::SharedPtr<omsl::graphics::Group> GraphicsGroupPtr;
typedef omsl::SharedPtr<omsl::graphics::Node> GraphicsNodePtr;
typedef omsl::SharedPtr<omsl::hcntcore::pargeom::PolylineIthPoint> ParGeomPolylineIthPointPtr;
typedef omsl::SharedPtr<omsl::hcntcore::ParGeomEntity> ParGeomEntityPtr;
typedef omsl::SharedPtr<omsl::hcntcore::CadEntity> CadEntityPtr;
typedef omsl::SharedPtr<omsl::hcntcore::pargeom::Geom> ParGeomGeomPtr;
typedef omsl::SharedPtr<omsl::hcntcore::pargeom::ToolpathMovement> ParGeomTpMovementPtr;

using namespace omsl::command;


namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            int TPAnalysisModel::GetViewModeValue ()
            {
                int vm = 0;
                vm |= GetValue ( tpanalysis::UI_Direction )      ? vm_SHOWDIRECTION : 0;
                //vm |= GetValue ( tpanalysis::UI_Compensation )   ? vm_SHOWCOMPPATH : 0;
                //vm |= GetValue ( tpanalysis::UI_Orientation )    ? vm_SHOWORIENTATION : 0;
                vm |= GetValue ( tpanalysis::UI_Vector )         ? vm_SHOWVECTORS : 0;
                vm |= GetValue ( tpanalysis::UI_Label )          ? vm_SHOWLABEL : 0;
                //vm |= GetValue ( tpanalysis::UI_Tool )           ? vm_SHOWTOOL : 0;
                vm |= GetValue ( tpanalysis::UI_Point )          ? vm_SHOWPOINTS : 0;
                vm |= GetValue ( tpanalysis::UI_ClearanceLevel ) ? vm_SHOWCLEARANCE : 0;
                vm |= GetValue ( tpanalysis::UI_FeedRate )       ? vm_SHOWFEEDRATEALWAYS : 0;
//                vm |= GetValue ( tpanalysis::UI_MoveLimits )     ? vm_MOVE_TP_VISIBILITYLIMITS : 0;

                return vm;
            }
                
            propertyservice::Applicable::Strength TPAnalysisModel::ActivateFeedRateMin ( const int& iMode )
            {
                propertyservice::Applicable::Strength retVal = propertyservice::Applicable::kInactive;
                if ( iMode )
                {
                    retVal = propertyservice::Applicable::kMandatory;
                }

                return retVal;
            }

            propertyservice::Applicable::Strength TPAnalysisModel::ActivateFeedRateMax ( const int& iMode )
            {
                propertyservice::Applicable::Strength retVal = propertyservice::Applicable::kInactive;
                if ( iMode )
                {
                    retVal = propertyservice::Applicable::kMandatory;
                }

                return retVal;
            }

            propertyservice::Applicable::Strength TPAnalysisModel::ActivateColorLevel ( const int& iMode )
            {
                propertyservice::Applicable::Strength retVal = propertyservice::Applicable::kInactive;
                if ( iMode )
                {
                    retVal = propertyservice::Applicable::kMandatory;
                }
                //else
                //    OnUpdateFeedrateDraw ( -44 );

                return retVal;
            }

            propertyservice::Applicable::Strength TPAnalysisModel::Activate_CB_Min_ColorSettings ( const int& iMode )
            {
                propertyservice::Applicable::Strength retVal = propertyservice::Applicable::kMandatory;
                if ( iMode )
                {
                    retVal = propertyservice::Applicable::kInactive;
                }
                else // restore values
                {
                }
                return retVal;
            }

            propertyservice::Applicable::Strength TPAnalysisModel::Activate_CB_Max_ColorSettings ( const int& iMode )
            {
                propertyservice::Applicable::Strength retVal = propertyservice::Applicable::kMandatory;
                if ( iMode )
                {
                    retVal = propertyservice::Applicable::kInactive;
                }
                else // restore values
                {
                }
                return retVal;
            }

            propertyservice::Applicable::Strength TPAnalysisModel::Activate_CB_Lvl_ColorSettings ( const int& iMode )
            {
                propertyservice::Applicable::Strength retVal = propertyservice::Applicable::kMandatory;
                if ( iMode )
                {
                    retVal = propertyservice::Applicable::kInactive;
                }
                else // restore values
                {
                }
                return retVal;
            }

            propertyservice::Applicable::Strength TPAnalysisModel::Activate_CB_Clr_ColorSettings ( const int& iMode )
            {
                propertyservice::Applicable::Strength retVal = propertyservice::Applicable::kMandatory;
                if ( iMode )
                {
                    retVal = propertyservice::Applicable::kInactive;
                }
                else // restore values
                {
                }
                return retVal;
            }

            int TPAnalysisModel::ChangeViewSettings ( const int& bFlag, const int& label )
            {
                int vm = GetViewModeValue ();
                
                // on / off / or nothings to do
                bool redraw = false;

                if ( bFlag )
                {
                    redraw = ( vm&label ) == 0x0;
                    vm |= label;
                }
                else
                {
                    redraw = ( vm&label ) != 0x0;
                    vm &= ~label;
                }

                redraw = true;

                if ( redraw ) // to avoid doubled creations
                {
                    switch ( label )
                    {
                        case vm_SHOWDIRECTION: 
                        {
                            SetupCurrentToolDirection ( vm );
                            break;
                        }
                        case vm_SHOWORIENTATION: 
                        {
                            SetupTemporaryGraphicsOrientation ( vm );
                            break;
                        }
                        case vm_SHOWVECTORS: 
                        {
                            SetupTemporaryTPGraphics ( omsl::hmbroadcast::bcs_TPCMShowGV, vm );
                            break;
                        }
                        case vm_SHOWPOINTS:
                        {
                            SetupTemporaryTPGraphics ( omsl::hmbroadcast::bcs_TPCMShowPt, vm );
                            break;
                        }
                        case vm_SHOWLABEL: 
                        {
                            SetupTemporaryGraphicsLabel ( vm );
                            break;
                        }
                    }
                }

                //SetValue ( tpanalysis::viewModeParameter, vm ); // better to set the value directly

                if ( !redraw )
                    return vm;

                switch ( label )
                {
                    case vm_SHOWCLEARANCE : // with / without clearance feeedrate
                    {
                        int allowed = GetValue ( tpanalysis::updateColorsTableAllowed );
                        if ( !allowed )
                            return 0;

                        SetupExtremaToUI ( (vm&vm_SHOWCLEARANCE) ? 0x0100 : 0x0200 );
//                        OnUpdateFeedrateDraw ( -11 );
                        break;
                    }
                    case vm_AUTOUPDATEMIN:
                    {
                        int allowed = GetValue ( tpanalysis::updateColorsTableAllowed );
                        if ( !allowed )
                            return 0;

                        SetupExtremaToUI ( ( vm&vm_SHOWCLEARANCE ) ? 0x0100 : 0x0200 );
//                        OnUpdateFeedrateDraw ( -22 );
                        break;
                    }
                    case vm_AUTOUPDATEMAX:
                    {
                        int allowed = GetValue ( tpanalysis::updateColorsTableAllowed );
                        if ( !allowed )
                            return 0;

                        SetupExtremaToUI ( ( vm&vm_SHOWCLEARANCE ) ? 0x0100 : 0x0200 );
//                        OnUpdateFeedrateDraw ( -33 );
                        break;
                    }
                    case vm_AUTOUPDATELVL:
                    {
                        int allowed = GetValue ( tpanalysis::updateColorsTableAllowed );
                        if ( !allowed )
                            return 0;
                        SetupExtremaToUI ( ( vm&vm_SHOWCLEARANCE ) ? 0x1100 : 0x1200 );
//                        OnUpdateFeedrateDraw ( -44 );
                        break;
                    }
                    case vm_SHOWFEEDRATEALWAYS :
                    {
                        if ( bFlag ) // switch on
                        {
                            SetupFeedRateColors ( vm&vm_SHOWCLEARANCE );
                        }
                        else
                        {
                            SetupMotionColors ( 0 ); // ??????
                        }
                        break;
                    }
                }


                return vm;
            }

            int TPAnalysisModel::OnUpdateFeedrateDraw ( const int& /*d*/ )
            {
                int v  = GetValue ( tpanalysis::UI_FeedRate );
                if ( v )
                {
                    int vm = GetValue ( tpanalysis::UI_ClearanceLevel ) ? vm_SHOWCLEARANCE : 0;
                    SetupFeedRateColors ( vm&vm_SHOWCLEARANCE );
                }

                return 0;
            }

            int TPAnalysisModel::OnChange_CB_Label ( const int& bFlag )
            {
                callStackString ( L"OnChange_CB_Label", 0 );

                return ChangeViewSettings ( bFlag, vm_SHOWLABEL);
            }
            
            //int TPAnalysisModel::OnChange_CB_Tool ( const int& bFlag )
            //{
            //    callStackString ( L"OnChange_CB_Tool", 0 );

            //    return ChangeViewSettings ( bFlag, vm_SHOWTOOL);
            //}

            int TPAnalysisModel::OnChange_CB_Orientation ( const int& bFlag )
            {
                callStackString ( L"OnChange_CB_Orientation", 0 );

                return ChangeViewSettings ( bFlag, vm_SHOWORIENTATION );
            }
            
            int TPAnalysisModel::OnChange_CB_Direction ( const int& bFlag )
            {
                callStackString ( L"OnChange_CB_Direction", 0 );

                return ChangeViewSettings ( bFlag, vm_SHOWDIRECTION );
            }
            
            //OMSL_IMPL (void) TPAnalysisModel::OnChange_CB_Direction1 ( const omsl::Bool& bFlag )
            //{
            //    SetValue ( tpanalysis::UI_Direction, (int) bFlag );
            //}

            //OMSL_IMPL (void) TPAnalysisModel::OnChange_CB_Label1 ( const omsl::Bool& bFlag )
            //{
            //    SetValue ( tpanalysis::UI_Label, (int) bFlag );
            //}

            //OMSL_IMPL (void) TPAnalysisModel::OnChange_CB_Feedrate1 ( const omsl::Bool& bFlag )
            //{
            //    SetValue ( tpanalysis::UI_FeedRate, (int) bFlag );
            //}

            int TPAnalysisModel::OnChange_CB_Vector ( const int& bFlag )
            {
                return ChangeViewSettings ( bFlag, vm_SHOWVECTORS );
            }
            
            int TPAnalysisModel::OnChange_CB_Point ( const int& bFlag )
            {
                return ChangeViewSettings ( bFlag, vm_SHOWPOINTS );
            }
            
            //int TPAnalysisModel::OnChange_CB_Compensation ( const int& bFlag )
            //{
            //    callStackString ( L"OnChange_CB_Compensation", 0 );

            //    return ChangeViewSettings ( bFlag, vm_SHOWCOMPPATH );
            //}

            int TPAnalysisModel::OnChange_CB_Feedrate ( const int& bFlag )
            {
                return ChangeViewSettings ( bFlag, vm_SHOWFEEDRATEALWAYS );
            }

            int TPAnalysisModel::OnChange_CB_Clearance ( const int& bFlag )
            {
                return ChangeViewSettings ( bFlag, vm_SHOWCLEARANCE );
            }

            int TPAnalysisModel::OnChange_CB_AutoUpdateMin ( const int& bFlag )
            {
                return ChangeViewSettings ( bFlag, vm_AUTOUPDATEMIN );
            }

            int TPAnalysisModel::OnChange_CB_AutoUpdateMax ( const int& bFlag )
            {
                return ChangeViewSettings ( bFlag, vm_AUTOUPDATEMAX );
            }

            int TPAnalysisModel::OnChange_CB_AutoUpdateLvl ( const int& bFlag )
            {
                if ( bFlag == 0 )
                    SetValue ( tpanalysis::useExactFeedrates, 1 );
                else
                    SetValue ( tpanalysis::useExactFeedrates, 0 );
                return ChangeViewSettings ( bFlag, vm_AUTOUPDATELVL );
            }


            int TPAnalysisModel::OnColorStepCount ( const Int& /*nLevels*/ )
            {
                int allowed = GetValue ( tpanalysis::updateColorsTableAllowed );
                if ( !allowed )
                    return 0;

                SetupExtremaToUI ( -300 );

                return 0;
            }

            int TPAnalysisModel::OnColorTargetMin ( const Double & /*dMin*/ )
            {
                int allowed = GetValue ( tpanalysis::updateColorsTableAllowed );
                if ( !allowed )
                    return 0;

                //double dmax = GetValue ( tpanalysis::UI_colorMapMax_2 );
                //double dmaxmax = GetValue ( tpanalysis::UI_colorMapCMax_3 );
                //if ( GetValue ( tpanalysis::UI_ClearanceLevel ) != 0 && dmax < dmaxmax )
                //    dmax = dmaxmax;
                //if ( dmax < dMin )
                //{
                //    SetValue ( tpanalysis::UI_feedrateValueMin , dmax ) );
                //    return 0;
                //}

                SetupExtremaToUI ( -200 );

                return 0;
            }

            int TPAnalysisModel::OnColorTargetMax ( const Double & /*dMax*/ )
            {
                int allowed = GetValue ( tpanalysis::updateColorsTableAllowed );
                if ( !allowed )
                    return 0;

                //double dmin = GetValue ( tpanalysis::UI_colorMapMin_2 );
                //if ( dmin > dMax )
                //{
                //    SetValue ( tpanalysis::UI_feedrateValueMax, dmin ) );
                //    return 0;
                //}

                SetupExtremaToUI ( -100 );

                return 0;
            }


            //int TPAnalysisModel::OnChangeColorSettings ( const int& bFlag )
            //{
            //    SetupExtremaToUI ( bFlag );
            //    return 0;
            //}

            SharedPtr<propertyservice::PropertyBag>  TPAnalysisModel::OnChangeMoveNr ( const int& moveNr )
            {
                SharedPtr<propertyservice::PropertyBag> pBag0;
                if ( s_data.m[MASTER_TP].pointVec.size () == 0 )
                {
                    return pBag0;
                }
                if ( moveNr == s_data.m[MASTER_TP].specialIndex [analyseData::dai_SpinBoxValue] )
                {
                    return pBag0;
                }

                if ( s_data.m[MASTER_TP].specialIndex [analyseData::dai_SpinBoxValue] < moveNr )
                    s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1]++;
                else
                    s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1]--;

                if ( GetValue(tpanalysis::UI_sliderValueMin ) > s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] )
                    s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] = GetValue ( tpanalysis::UI_sliderValueMin );
                if ( GetValue ( tpanalysis::UI_sliderValueMax ) < s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] )
                    s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] = GetValue ( tpanalysis::UI_sliderValueMax );

                // checksegments
                m_withUpdate = false;
                bool changeselection = false;

                m_withUpdate = true;

                if ( s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] < 0 )
                    s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] = 0;
                if ( s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] >= s_data.m[MASTER_TP].pointVec.size () )
                    s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] = ( int ) s_data.m[MASTER_TP].pointVec.size ()-1 ;


                s_data.m[MASTER_TP].specialIndex [analyseData::dai_SpinBoxValue] = moveNr;

                // segment and other should be correct

                CadEntitiesIt_T pEmptyEnts;

                try
                {
                    pEmptyEnts = GetValue ( tpanalysis::UI_FirstEntities ); 
                    OMSL_FOREACH ( pEmptyEnts )
                    {
                        CadEntityPtr pEnts1 ( pEmptyEnts->Get () );
                        ParGeomEntityPtr pgep ( ( omsl::hcntcore::ParGeomEntity *) pEnts1->Cast(L"ParGeomEntity") );
                        if ( !pgep.Get () )
                            continue;
                        if ( changeselection == true )
                            break;

                        SharedPtr<const hcntcore::pargeom::Point> parGeomPoint;
                        parGeomPoint = hcntcore::CadEntityMgr::GetParGeomPoint ( pEnts1.Get() );
                        omsl::SharedPtr<omsl::hcntcore::pargeom::PolylineIthPoint> pgplipp ( parGeomPoint.shared_cast< omsl::hcntcore::pargeom::PolylineIthPoint >( L"PolylineIthPoint" ) );
                        if ( pgplipp.Get () )
                        {
                            pgplipp->SetIndex ( s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] );

                            hcntcore::Model* model = ( hcntcore::Model* )m_pDocument->Cast ( L"Model" );

                            model->CadEntityMgr ()->CreateCadEntityGraphic ( pEnts1.Get () );

                            GraphicsGroupPtr pn (pgep->GetGraphicNode ());

                            pn->HighLight ( omsl::graphics::Group::HLColor, 0, omsl::kTrue );

                            SharedPtr<selectionservice::InteractiveSelector> pEntSel ( omsl::selectionservice::Get ()->GetCurrentSelector ( m_pDocument.Get () ) );
                            SharedPtr<selectionservice::Selection> pSelection ( pEntSel->GetSelections () );

                            selectionservice::SelectionEmitter ().Emit ( this, pSelection.get () );
                            break;
                        }
                        else
                        {
                            ParGeomGeomPtr pggp ( pgep->GetParGeom () );
                            ParGeomTpMovementPtr pTpMove = pggp.shared_cast< omsl::hcntcore::pargeom::ToolpathMovement >( L"ToolpathMovement" );
                            if ( pTpMove.Get () )
                            {
                                if ( s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] )
                                    s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1]--;

                                pEnts1 = GetReferencedToolpathSegmentEntity ( pEnts1, false );
                                pTpMove = omsl::hcntcore::pargeom::ToolpathMovement::CreateObject ( pEnts1.Get (), s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] );

                                if ( pTpMove.Get () )
                                {
                                    hcntcore::Model* model = ( hcntcore::Model* )m_pDocument->Cast ( L"Model" );

                                    SharedPtr<hcntcore::CadEntity> movementEntity ( model->CadEntityMgr ()->CreateParGeomEntity ( pTpMove.Get () ) );

                                    movementEntity->GetGraphicAttributes ()->SetMaterial ( pEnts1->GetGraphicAttributes ()->GetMaterial () );
                                    movementEntity->GetGraphicAttributes ()->SetGraphicType ( pEnts1->GetGraphicAttributes ()->GetGraphicType () );
                                    movementEntity->GetGraphicAttributes ()->SetGraphicWidth ( pEnts1->GetGraphicAttributes ()->GetGraphicWidth () );
                                    SharedWStringPtr str ( pEnts1->GetLayerName () );
                                    movementEntity->SetLayerName ( str->Get () );


                                    CadEntitiesIt_T pNewEnts;
                                    // clear selections
                                    if ( 1 )
                                    {
                                        std::vector<omsl::SharedPtr<omsl::hcntcore::CadEntity> > newEnts;
                                        CadEntityPtr hcnt_cent_pgbcp = movementEntity.shared_cast< omsl::hcntcore::CadEntity >( L"CadEntity" );
                                        newEnts.push_back ( hcnt_cent_pgbcp );

                                        container::hcntcore::VectorCadEntityPtrConcreteForward *pIt = VCECF_NEW ( newEnts);
                                        pNewEnts = pIt;


                                        SetValue ( tpanalysis::UI_FirstEntities, pNewEnts );


                                        SharedPtr<selectionservice::InteractiveSelector> pEntSel ( omsl::selectionservice::Get ()->GetCurrentSelector ( m_pDocument.Get () ) );
                                        SharedPtr<selectionservice::Selection> pSelection ( pEntSel->GetSelections () );

                                        selectionservice::SelectionEmitter ().Emit ( this, pSelection.get () );

                                        CheckSelection ( GetValue ( tpanalysis::UI_FirstEntities ), GetValue ( tpanalysis::UI_SecondEntities ) );
                                        return Compute ( GetValue ( tpanalysis::UI_FirstEntities ), GetValue ( tpanalysis::UI_SecondEntities ) );
                                    }
                                }
                                break;

                            }
                            continue;
                        }
                    }

                    return Compute ( GetValue ( tpanalysis::UI_FirstEntities ), GetValue ( tpanalysis::UI_SecondEntities ) );
                }
                catch ( ... )
                {
                }

                return pBag0;
                // The grid is reset because the rule is called
            }

            void TPAnalysisModel::OnChangeMoveNr2 ( const int& /*moveNr*/ )
            {
                if ( s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] < 0 || s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] >= s_data.m[MASTER_TP].pointVec.size () )
                {
                    return;
                }

                {
                    bgeo::Coord3d pt0 = s_data.m[MASTER_TP].pointVec [s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1]];
                    // for geometry
                    s_data.m[MASTER_TP].points [analyseData::dai_SXP1] = omsl::bgeo::Point3d ( pt0 );

                    SetupPropertyBag ();
                }
            }

            tpanalysis::compTPAMode  TPAnalysisModel::checkTPAMode ( int sel )
            {
                // to do : 
                // check selection 1 if exist
                if ( sel != U_SEL )
                {
                    return s_data.m [sel].m_compTPAMode;
                }

                if ( s_data.m[R_SEL].specialIndex [analyseData::dai_SXP1] == UNKNOWNINDEX || s_data.m[R_SEL].specialIndex [analyseData::dai_SXP1] == UNSPECIFICINDEX )
                {
                    if ( s_data.m[L_SEL].specialIndex [analyseData::dai_SXP1] != UNKNOWNINDEX )
                    {
                        if ( s_data.m [R_SEL].m_compTPAMode == tpanalysis::cm_Geometry )
                        {
                            if ( s_data.m [L_SEL].m_compTPAMode == tpanalysis::cm_Point )
                                return tpanalysis::cm_Point_Geometry;
                            else
                                return tpanalysis::cm_Movement_Geometry;
                        }

                        return s_data.m [L_SEL].m_compTPAMode;
                    }
                    else
                    {
                        return s_data.m [L_SEL].m_compTPAMode;
                    }
                }
                else
                {
                    if ( s_data.m[L_SEL].specialIndex [analyseData::dai_SXP1] != UNKNOWNINDEX )
                    {
                        if ( s_data.m [L_SEL].m_compTPAMode != s_data.m [R_SEL].m_compTPAMode )
                        {
                            return tpanalysis::cm_Point_Movement;
                        }

                        if ( s_data.m[R_SEL].m_compTPAMode == tpanalysis::cm_Point )
                            return tpanalysis::cm_Point_Point;
                        else
                            return tpanalysis::cm_Movement_Movement;
                    }
                    else
                    {
                        return s_data.m [R_SEL].m_compTPAMode;
                    }
                }
                // check selection 2 if exist
                //return tpanalysis::cm_Toolpath;
            }


            void TPAnalysisModel::SetInternalSelection ( SharedPtr<hcntcore::CadEntity>& pEntsIn, int sel )
            {
                CadEntityPtr pCadEntity = pEntsIn;
                
                // default if empty
                s_data.m [sel].m_compTPAMode = tpanalysis::cm_TPALast;

                if ( !pCadEntity.Get () )
                    return;
                
                // default if cadentity
                s_data.m [sel].m_compTPAMode = tpanalysis::cm_Geometry;

                ToolpathSegmentPtr pSegment;

                GeomEntityPtr gentp = pEntsIn.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                if ( gentp.Get () )
                {
                    Geom3dPtr g3dp ( gentp->Geom () );
                    if ( g3dp.Get () )
                    {
                        pSegment = g3dp.shared_cast< omsl::geom::ToolpathSegment >( L"ToolpathSegment" );
                    }
                }

                if ( pSegment.Get () == nullptr )
                {
                    SharedPtr<omsl::hcntcore::ParGeomEntity> pParGeom = pEntsIn.shared_cast< omsl::hcntcore::ParGeomEntity >( L"ParGeomEntity" );
                    if ( pParGeom.Get () )
                    {
                        ParGeomGeomPtr pGeom ( pParGeom->GetParGeom () );

                        omsl::SharedPtr<omsl::hcntcore::pargeom::PolylineIthPoint> pgplipp ( pGeom.shared_cast< omsl::hcntcore::pargeom::PolylineIthPoint >( L"PolylineIthPoint" ) );
                        if ( pgplipp.Get () )
                        {
                            /////// point to create the Tooltip ????
                            s_data.m[sel].m_compTPAMode = tpanalysis::cm_Point;
                        }
                        else
                        {
                            ParGeomTpMovementPtr pTpMove = pGeom.shared_cast< omsl::hcntcore::pargeom::ToolpathMovement >( L"ToolpathMovement" );
                            if ( pTpMove.Get () )
                            {
                                s_data.m[sel].m_compTPAMode = tpanalysis::cm_Movement;
                                return;
                            }
                        }
                    }
                }
            }
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
