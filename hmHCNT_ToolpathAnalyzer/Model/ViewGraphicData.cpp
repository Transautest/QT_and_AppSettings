#pragma warning( disable : 4996 ) // for warnings: due to compare of boost::uuids
#include "TPAnalysisModel.h"
// keys
#include <Commands\tpanalysiskeys.h>
#include <Commands\tpanalysepropertieskeys.h>
#include <hcntcore/document.h>
#include <hcntcore\model.h>

#include <propertyservice/propertybag/minmaxvalidatort.h>
#include <hcntcore/cadentityiterators.h>
#include <hcntcore/pargeomentity.h>
#include <hcntcore/workplaneentity.h>

#include <geom/geometriciterators.h>
#include <geom/geom3d.h>
#include <bgeo/coord3diterators.h>
#include <hcntcore/cadentitymgr.h>
#include <geom/line3d.h>
#include <geom/arc3d.h>
#include <hcntcore/polylineithpoint.h>

#include <..\..\..\INTERFACES\iHCNT\iTopoAttrib.h>
#include <graphicattributes/toolpathgraphicattributes.h>
#include <graphicattributes/toolpathsegmentgraphicattributes.h>
#include <graphicattributes/pointcloudgraphicattributes.h>
#include <graphicattributes/graphicattributesfactory.h>
#include <materials/colormaterial.h>
#include <materials/colorspalette.h>
#include <materials/colorspalettefactory.h>
#include <materials/material.h>
#include <materials/materials.h>
#include <materials/materialslibrary.h>
#include <materials/materialsstatus.h>
#include <measureunit\converter.h>
#include <units\dimension.h>
#include <geom/revolvedsurface.h>
#include <hcntcore/toolpathmovement.h>
#include <bgeo/axis3dvectorcontainers.h>
#include <hcntcore/referencesystemfree.h>
#include <bgeo/point3d.h>
#include <drafting/arrow.h>
#include <hcntcore/application.h>
#include <gui/gui.h>
#include <gui/guihandshake.h>
//#include "../Commands/tpanalyseproperties.h"
//#include "../Commands/tpanalysepropertiesimpl.h"
#include "../Commands/common_functions.h"
#include "../Commands/tp_listener.h"
#include "../Commands/tp_signalemitter.h"

#include <math.h>
#include <graphics/group.h>
#include <hcNTLib/ConnectionHlpTT.h>
#include <hcNTLib/GraphicsHlp.h>

#include <hcntLib/TopoAttribHlp.h>
#include <hcntLib/ToolPathHlp.h>
#include <hcntLib/PropertyServiceHlp.h>
#include <containers\mapcontainer.h>
#include <containers/defaultiteratorsreadonly.h>
#include <tchar.h>

#include <qstring.h>
#include "translate.h"

#include "hcntLib/containernew.h"



using omsl::linguistic::QT_TRANSLATE_NOOP;

typedef omsl::SharedPtr<omsl::graphics::Group> GraphicsGroupPtr;
typedef omsl::SharedPtr<omsl::graphics::Node> GraphicsNodePtr;

using namespace omsl::command;

typedef omsl::SharedPtr<omsl::hcntcore::GeomEntity> GeomEntityPtr;
typedef omsl::SharedPtr<omsl::geom::Geom3d> Geom3dPtr;
typedef omsl::SharedPtr<omsl::hcntcore::ParGeomEntity> ParGeomEntityPtr;
typedef omsl::SharedPtr<omsl::hcntcore::pargeom::Geom> ParGeomGeomPtr;
typedef omsl::SharedPtr<omsl::hcntcore::pargeom::ToolpathMovement> ParGeomTpMovementPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::GraphicAttributes> GraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::ToolpathGraphicAttributes> ToolpathGraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::ToolpathSegmentGraphicAttributes> ToolpathSegmentGraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::graphicattributes::PointCloudGraphicAttributes> PointCloudGraphicAttributesPtr;
typedef omsl::SharedPtr<omsl::hcntcore::CadEntity> CadEntityPtr;
typedef omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> CadEntityForwardPtr;

typedef omsl::container::MapContainerWrapper<omsl::ULong, omsl::ULong, std::map<omsl::ULong, omsl::ULong>, omsl::iterator::UlongUlongMapIteratorForwardReadOnly> ULULMap;
typedef omsl::SharedPtr<ULULMap> ULULMapPtr;

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            void TPAnalysisModel::SetupTemporaryGraphics ( )
            {
                const int vm = GetViewModeValue ();

                SetupCurrentToolDirection ( vm );

                //tpanalysis::compTPAMode resMode = checkTPAMode ( U_SEL );
                switch ( checkTPAMode (U_SEL) )
                {
                    case tpanalysis::cm_Toolpath:
                    {
                        SetupTemporaryGraphicsCompensation ( vm );
                        break;
                    }
                    default:
                    {
                        SetupTemporaryGraphicsCompensation ( vm );
                        SetupTemporaryGraphicsOrientation ( vm );
                        break;
                    }
                }
            }

            //void TPAnalysisModel::SetupMeasurementGraphic ( const int viewModeParameter )
            //{

            //    tpanalysis::compTPAMode resMode = checkTPAMode ( U_SEL );

            //    SetupTemporary_Measure_Graphic ();

            //    switch ( resMode )
            //    {
            //        //case tpanalysis::cm_Point_Point:
            //        //{
            //        //    SetupTemporary_P_P_Graphic ( viewModeParameter );
            //        //    break;
            //        //}
            //        //case tpanalysis::cm_Movement_Movement: // not implmented yet
            //        //{
            //        //    SetupTemporary_M_M_Graphic ( viewModeParameter );
            //        //    break;
            //        //}
            //        //case tpanalysis::cm_Point_Movement: // not implmented yet
            //        //{
            //        //    SetupTemporary_P_M_Graphic ( viewModeParameter );
            //        //    break;
            //        //}
            //        default: // nothing to do
            //        {
            //            break;
            //        }
            //    }
            //}

            void TPAnalysisModel::SetupCurrentToolDirection ( const int viewModeParameter )
            {
                #define GH_PIT2 6.283185307179586476925286766559

                std::vector<bgeo::Axis3d> vGraphicsAxisC;
                tpanalysis::AxisIt_T pGraphicsAxis;

                SetValue ( tpanalysis::TGr_ReferenceArrowC, pGraphicsAxis );

                if ( viewModeParameter&vm_SHOWDIRECTION )
                {
                    for ( int sel = MASTER_TP; sel < ALL_SEL; sel++ )
                    {
                        if ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] > 0 && s_data.m [sel].specialIndex [analyseData::dai_SXP1] < s_data.m [sel].pointVec.size () - 1 )
                        {
                            size_t scndPos = s_data.m [sel].specialIndex [analyseData::dai_SXP1] - 1;
                            omsl::bgeo::Point3d lsp ( s_data.m [sel].pointVec [scndPos] ),
                                lep ( s_data.m [sel].pointVec [s_data.m [sel].specialIndex [analyseData::dai_SXP1]] );
                            //size_t s = s_data.m[MASTER_TP].pointVec.size ();
                            while ( lsp.IsEquiv ( lep ) && scndPos > 0 )
                            {
                                scndPos--;
                                lep.Set ( s_data.m [sel].pointVec [scndPos] );
                            }

                            omsl::bgeo::UnitVector3d uv ( lsp, lep );

                            int type = s_data.m [sel].typeVec [scndPos]; //  s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1]];

                            if ( type == analyseData::dai_tArc ) // curve before is an arc
                            {
                                uv.Set ( s_data.m [sel].vectors [analyseData::dai_CTR4] );
                            }
#ifdef DBGVER
                            wchar_t vectorstring [256];
                            swprintf ( vectorstring, L"uv = x=%8.4lf y=%8.4lf z=%8.4lf ", uv [0], uv [1], uv [2] );
                            callStackString ( vectorstring, 0 );
#endif
                            vGraphicsAxisC.push_back ( bgeo::Axis3d ( lep, uv ) );
                        }
                    }

                    if ( vGraphicsAxisC.size () )
                    {
                        omsl::container::bgeo::VectAxis3dConcreteForward *pIt = VA3DCF_NEW (vGraphicsAxisC);
                        pGraphicsAxis = pIt;
//                        pIt->GetContainerReference ().swap ( vGraphicsAxisC );
                        SetValue ( tpanalysis::TGr_ReferenceArrowC, pGraphicsAxis );
                    }
                }
            }


#ifdef XXXXXX
            void TPAnalysisModel::SetupTemporaryGraphicsDirection ( const int viewModeParameter, omsl::hcntcore::CadEntity* entity )
            {
                #define GH_PIT2 6.283185307179586476925286766559

                //tpanalysis::AxisIt_T pGraphicsAxisS = GetValue ( tpanalysis::TGr_ReferenceArrowS);
                //tpanalysis::AxisIt_T pGraphicsAxisE = GetValue ( tpanalysis::TGr_ReferenceArrowE);

                bool old = SetSignalListening ( false );

                if ( viewModeParameter&vm_SHOWDIRECTION )
                {
                    int maxmax = GetValue ( tpanalysis::UI_sliderMax_2 );

                    int sindex1 = GetValue ( tpanalysis::UI_sliderValueMin );
                    int sindex2 = GetValue ( tpanalysis::UI_sliderValueMax );
                    int gap = sindex2 - sindex1;

                    // to avoid mingap
                    if ( maxmax - sindex2 < MINIMUMSEGGAP && gap < MINIMUMSEGGAP )
                    {
                        sindex1 = sindex2 - MINIMUMSEGGAP;
                    }
                    else if ( gap < MINIMUMSEGGAP )
                        sindex2+=MINIMUMSEGGAP;


                    CadEntityPtr cent ( entity );

                    ToolpathPtr pToolpath = cent.shared_cast < omsl::hcntcore::ToolPathEntity > ( L"ToolPathEntity" );

                    if ( pToolpath.Get () ) // selected an Toolpath
                    {
                        std::vector<bgeo::Axis3d> vGraphicsAxisS;
//                        std::vector<bgeo::Axis3d> vGraphicsAxisE;

                        if ( pGraphicsAxisS.Get () && pGraphicsAxisS->Size () )
                        {
//                            int s = ( int ) pGraphicsAxisS->Size ();
                            for ( OMSL_EACH ( pGraphicsAxisS ) )
                            {
                                vGraphicsAxisS.push_back ( pGraphicsAxisS.Get ()->Get () );
                            }
                            //for ( OMSL_EACH ( pGraphicsAxisE ) )
                            //{
                            //    vGraphicsAxisE.push_back ( pGraphicsAxisE.Get ()->Get () );
                            //}
                        }

                        ToolPathPtrgraphDispValMap::iterator it = s_data.m[0].pToolpathMap.find ( pToolpath );

                        omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> ents ( pToolpath->GetCadEntities () );
                        if ( ents.Get () )
                        {
                            int tl2 = sindex2, tl1 = sindex1;
                            ToolpathSegmentPtr sSeg;
                            ToolpathSegmentPtr lSeg;

                            omsl::bgeo::Point3d p1, p2;
                            omsl::bgeo::UnitVector3d uv1, uv2;
                            bool visible = true;
                            int stat = hcNTLib::ToolPathGetVisibleStartEndVector ( pToolpath, tl1, tl2, p1, p2, uv1, uv2, visible );

                            if ( stat == 0 )
                            {
                                ( *it ).second.dirPos1 = -1;
                                ( *it ).second.dirPos2 = -1;
                                SetSignalListening ( old );
                                return;
                            }

                            ( *it ).second.dirPos1 = tl1;
                            ( *it ).second.dirPos2 = tl2;


                            if ( ( stat & 0x0001 ) == 0x0001 )
                                vGraphicsAxisS.push_back ( bgeo::Axis3d ( p1, uv1 ) );
                            //if ( ( stat & 0x0002 ) == 0x0002 )
                            //    vGraphicsAxisE.push_back ( bgeo::Axis3d ( p2, uv2 ) );

                            tpanalysis::AxisIt_T pGraphicsAxis;

                            if ( vGraphicsAxisS.size () )
                            {
                                omsl::container::bgeo::VectAxis3dConcreteForward *pIt = VA3DCF_NEW ();
                                pGraphicsAxis = pIt;
                                pIt->GetContainerReference ().swap ( vGraphicsAxisS );
                                SetValue ( tpanalysis::TGr_ReferenceArrowS, pGraphicsAxis );
                            }
                            //if ( vGraphicsAxisE.size () )
                            //{
                            //    omsl::container::bgeo::VectAxis3dConcreteForward *pIt = VA3DCF_NEW ();
                            //    pGraphicsAxis = pIt;
                            //    pIt->GetContainerReference ().swap ( vGraphicsAxisE );
                            //    SetValue ( tpanalysis::TGr_ReferenceArrowE, pGraphicsAxis );
                            //}
                        }
                    }
                }

                SetSignalListening ( old );
            }


            void TPAnalysisModel::SetupTemporaryGraphicsDirection
            (
                const int // viewModeParameter
            )
            {
                tpanalysis::AxisIt_T pGraphicsAxis;

                bool old = SignalListeningIsEnabled ();

                SetSignalListening ( false );

                SetValue ( tpanalysis::TGr_ReferenceArrowS, pGraphicsAxis );
                //SetValue ( tpanalysis::TGr_ReferenceArrowE, pGraphicsAxis );

                if ( GetValue ( tpanalysis::UI_Direction ) )
                {
                    // loopTPMap
                    ToolPathPtrgraphDispValMap::const_iterator cit;
                    for ( cit = s_data.m[0].pToolpathMap.cbegin (); cit != s_data.m[0].pToolpathMap.cend (); cit++ )
                    {
                        if ( ( *cit ).second.tptype == ( eCt_tp ) )
                            SetupTemporaryGraphicsDirection ( vm_SHOWDIRECTION, ( *cit ).first.Get () );
                    }
                }

                SetSignalListening ( old );
            }
#endif

            void TPAnalysisModel::SetupTemporaryGraphicsCompensation 
            (
                const int // viewModeParameter 
            )
            {
            }

            void TPAnalysisModel::SetupTemporaryGraphicsOrientation ( const int /*viewModeParameter*/ )
            {
#ifdef XXXX
                tpanalysis::TopoGeomIt_T pGraphicsL;
                tpanalysis::TopoGeomIt_T pGraphicsR;

                if ( viewModeParameter&vm_SHOWORIENTATION && s_data.m[MASTER_TP].pointVec.size () > 0 )
                {
                    std::vector<SharedPtr<geom::TopoGeom> > vGraphicsL;
                    std::vector<SharedPtr<geom::TopoGeom> > vGraphicsR;
                   
                    if ( s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] > 0 )
                    {
                        switch ( s_data.m[MASTER_TP].typeVec [s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] - 1] )
                        {
                            case analyseData::dai_tUnknown:
                            case analyseData::dai_tLine:
                            {
                                SharedPtr<geom::Line3d> pLine;
                                pLine = geom::Line3d::CreateFrom2Points ( omsl::bgeo::Point3d ( s_data.m[MASTER_TP].pointVec [s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1]] ), omsl::bgeo::Point3d ( s_data.m[MASTER_TP].pointVec [s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] - 1] ) );
                                vGraphicsL.push_back ( SharedPtr<geom::TopoGeom> ( pLine.Get () ) );
                                break;
                            }
                            case analyseData::dai_tArc:
                            case analyseData::dai_tCurve:
                            default:
                            {
                                const omsl::geom::TopoGeom* pc = s_data.m[MASTER_TP].m_pTpSeg->GetCurve ( s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1]-1 );
                                if ( pc )
                                vGraphicsL.push_back ( SharedPtr<geom::TopoGeom> ( pc->Clone () ) );
                                break;
                            }
                        }

                        if ( !vGraphicsL.empty () )
                        {
                            container::geom::VectTopoGeomPtrConcreteForward *pIt = VTGCF_NEW ();
                            pGraphicsL = pIt;
                            pIt->GetContainerReference ().swap ( vGraphicsL );
                        }
                    }

                    if ( s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] < s_data.m[MASTER_TP].pointVec.size ()-1 )
                    {
                        switch ( s_data.m[MASTER_TP].typeVec [s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1]] )
                        {
                            case analyseData::dai_tUnknown:
                            case analyseData::dai_tLine:
                            {
                                SharedPtr<geom::Line3d> pLine;
                                pLine = geom::Line3d::CreateFrom2Points ( omsl::bgeo::Point3d ( s_data.m[MASTER_TP].pointVec [s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1]] ), omsl::bgeo::Point3d ( s_data.m[MASTER_TP].pointVec [s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] + 1] ) );
                                vGraphicsR.push_back ( SharedPtr<geom::TopoGeom> ( pLine.Get () ) );
                                break;
                            }
                            case analyseData::dai_tArc:
                            case analyseData::dai_tCurve:
                            default:
                            {
                                const omsl::geom::TopoGeom* pc = s_data.m[MASTER_TP].m_pTpSeg->GetCurve ( s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] );
                                if ( pc )
                                vGraphicsR.push_back ( SharedPtr<geom::TopoGeom> ( pc->Clone ()) );
                                break;
                            }
                        }

                        if ( !vGraphicsR.empty () )
                        {
                            container::geom::VectTopoGeomPtrConcreteForward *pIt = VTGCF_NEW ();
                            pGraphicsR = pIt;
                            pIt->GetContainerReference ().swap ( vGraphicsR );
                        }
                    }
                }

                SetValue ( tpanalysis::TGr_OrientationVectorsL, pGraphicsL );
                SetValue ( tpanalysis::TGr_OrientationVectorsR, pGraphicsR );
#endif
            }

            void TPAnalysisModel::SetupTemporaryTPGraphics ( const int type, const int viewModeParameter )
            {
                // to avoid endless calls
                if ( SignalListeningIsEnabled () == false )
                    return;

//                bool res = true;
                //bool updateGraphics = false;

                bool old = SetSignalListening ( false );

                switch ( type )
                {
                    case omsl::hmbroadcast::bcs_TPCMShowGV:
                    {
                        // toggle
                        auto vm = GetValue ( tpanalysis::UI_Vector );

                        if ( vm == 1 ) // default from jobsettings
                        omsl::hmbroadcast::EmitTPSignal ( L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCMShowGV, nullptr, -1 , 0.0 );
                        else
                        omsl::hmbroadcast::EmitTPSignal ( L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCMShowGV, nullptr, viewModeParameter&vm_SHOWVECTORS ? 1 : 0, 0.0 );
                        //omsl::hmbroadcast::EmitGVIcon ( b_GVIcon == true ? 1 : 0);

                        break;
                    }
                    case omsl::hmbroadcast::bcs_TPCMShowPt:
                    {
                        // toggle
                        auto vm = GetValue ( tpanalysis::UI_Point );
                        if ( vm == 1 ) // default from jobsettings
                        omsl::hmbroadcast::EmitTPSignal ( L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCMShowPt, nullptr, -1 , 0.0 );
                        else
                        omsl::hmbroadcast::EmitTPSignal ( L"UPDATE_TPA_DATA", omsl::hmbroadcast::bcs_TPCMShowPt, nullptr, viewModeParameter&vm_SHOWPOINTS ? 1 : 0 , 0.0 );
                        //omsl::hmbroadcast::EmitPTIcon (  b_PTIcon == true ? 1 : 0);

                        break;
                    }
                }

                SetSignalListening ( old );
            }
#ifdef XXXXX
            int TPAnalysisModel::removeDirection ( hcntcore::CadEntity* entity )
            {
                int res = -1;
                CadEntityPtr cent ( entity );

                ToolpathPtr pToolpath = cent.shared_cast < omsl::hcntcore::ToolPathEntity > ( L"ToolPathEntity" );

                if ( pToolpath.Get () ) // selected an Toolpath
                {
                    ToolPathPtrgraphDispValMap::iterator it = s_data.m[0].pToolpathMap.find ( pToolpath );

                    omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> ents ( pToolpath->GetCadEntities () );
                    if ( ents.Get () )
                    {
                        // get startvector

                        ents->MoveToBegin ();
                        if ( ents->IsAtEnd () )
                            return res;

                        {
                            omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );

                            if ( cep.Get () )
                            {
                                int sindex = GetValue ( tpanalysis::UI_sliderValueMin );

                                if ( it != s_data.m[0].pToolpathMap.end () )
                                    sindex = ( *it ).second.dirPos1;

                                ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep ) );

                                if ( pSegment.Get () && sindex < pSegment->GetPoints ()->Size () )
                                {
                                    omsl::bgeo::Point3d lsp ( *( pSegment->GetPoint ( sindex ) ) );

//                                    tpanalysis::AxisIt_T pGraphicsAxisS = GetValue ( tpanalysis::TGr_ReferenceArrowS );
                                    //tpanalysis::AxisIt_T pGraphicsAxisE = GetValue ( tpanalysis::TGr_ReferenceArrowE );

                                    //std::vector<bgeo::Axis3d> vGraphicsAxisS;
                                    //std::vector<bgeo::Axis3d> vGraphicsAxisE;

                                    if ( pGraphicsAxisS.Get () && pGraphicsAxisS->Size () )
                                    {
                                        res = 0;
//                                        int s = pGraphicsAxisS->Size ();
                                        //pGraphicsAxisE->MoveToBegin ();
                                        for ( OMSL_EACH ( pGraphicsAxisS ) )
                                        {
                                            double d = pGraphicsAxisS->Get ().Origin().Distance ( lsp );
                                            if ( d < 0.001 && res == 0 )
                                            {
                                                res = 1;
                                                //pGraphicsAxisE->Next ();
                                                continue;
                                            }

                                            vGraphicsAxisS.push_back ( pGraphicsAxisS.Get ()->Get () );
                                            ////vGraphicsAxisE.push_back ( pGraphicsAxisE.Get ()->Get () );
                                            //pGraphicsAxisE->Next ();
                                        }
                                    }

                                    tpanalysis::AxisIt_T pGraphicsAxis;

                                    if ( vGraphicsAxisS.size () )
                                    {
                                        omsl::container::bgeo::VectAxis3dConcreteForward *pIt = VA3DCF_NEW ();
                                        pGraphicsAxis = pIt;
                                        pIt->GetContainerReference ().swap ( vGraphicsAxisS );
                                        SetValue ( tpanalysis::TGr_ReferenceArrowS, pGraphicsAxis );
                                    }
                                    else
                                    {
                                        tpanalysis::AxisIt_T pGraphicsAxisNUll;
                                        SetValue ( tpanalysis::TGr_ReferenceArrowS, pGraphicsAxisNUll );
                                    }

                                    //if ( vGraphicsAxisE.size () )
                                    //{
                                    //    omsl::container::bgeo::VectAxis3dConcreteForward *pIt = VA3DCF_NEW ();
                                    //    pGraphicsAxis = pIt;
                                    //    pIt->GetContainerReference ().swap ( vGraphicsAxisE );
                                    //    SetValue ( tpanalysis::TGr_ReferenceArrowE, pGraphicsAxis );
                                    //}
                                    //else
                                    //{
                                    //    tpanalysis::AxisIt_T pGraphicsAxisNUll;
                                    //    SetValue ( tpanalysis::TGr_ReferenceArrowE, pGraphicsAxisNUll );
                                    //}

                                }
                            }
                        }
                    }
                }
                return res;
            }
#endif
            int TPAnalysisModel::removeLabel ( hcntcore::CadEntity* entity )
            {
                int res = -1;
                CadEntityPtr cent ( entity );

                ToolpathPtr pToolpath = cent.shared_cast <omsl::hcntcore::ToolPathEntity> ( L"ToolPathEntity" );

                if ( !pToolpath.Get () ) // selected an Toolpath
                    return res;

                omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> ents ( pToolpath->GetCadEntities () );
                if ( !ents.Get () )
                {
                    return res;
                }

                ents->MoveToBegin ();
                if ( ents->IsAtEnd () )
                    return res;

                omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );

                if ( !cep.Get () )
                    return res;

                ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep, U_SEL ) );

                TopoGeomPtr pTopo = pSegment.shared_cast< omsl::geom::TopoGeom >( L"TopoGeom" );
                if ( pTopo && pTopo->GetAttribs () )
                {
                    omsl::SharedWStringPtr str1;

                    ToolpathData* ptpData = GetToolpathData ( pSegment );
                    if ( ptpData )
                    {
                        if ( ptpData->GetInternalTpType () == ( int ) eCt_clp )
                            return 0;

                        str1 = omsl::WStringImpl::Create ( ptpData->Jobname () );
                    }

//                    const omsl::bgeo::Coord3d* p0 = pSegment->GetPoint ( 0 );
                    SharedPtr<container::VectWCharTConcreteForward> pTxts ( VWCF_NEW () );
                    SharedPtr<container::bgeo::VectCoord3dConcreteForward> pTxtPts ( VC3DCF_NEW () );
//                    SharedPtr<container::geom::VectTopoGeomPtrConcreteForward> vGraphics ( VTGCF_NEW () );

//                    tpanalysis::TopoGeomIt_T pGraphics = GetValue ( tpanalysis::TGr_LabelVectors );
                    tpanalysis::Coord3dIt_T  pTextPoints = GetValue ( tpanalysis::TGr_TextPoints );
                    tpanalysis::WCharIt_T    pTexts = GetValue ( tpanalysis::TGr_Texts );

                    if ( !pTextPoints.Get () || pTextPoints->Size () == 0 )
                        return 0;

                    pTexts->MoveToBegin ();
                    pTextPoints->MoveToBegin ();

                    omsl::bgeo::Coord3d p3 ( *pSegment->GetPoint ( 0 ) );

                    p3 [0] += 0.5;
                    p3 [1] += 0.5;
                    p3 [2] += 0.5;

                    res = 0;
                    pTexts->MoveToBegin ();
                    for ( OMSL_EACH ( pTextPoints ) )
                    {
//                        double d = pTextPoints->Get ().Distance ( p3 );
                        if ( str1.Get () && str1->GetSize () > 0 && wcscmp ( str1->Get (), pTexts->Get () ) == 0 )
                        {
                            res = 1;
                            pTexts->Next ();
//                            pGraphics->Next ();
                            continue;
                        }

                        pTxts->PushBack ( pTexts.Get ()->Get () );
                        pTxtPts->PushBack ( pTextPoints.Get ()->Get () );
//                        vGraphics->PushBack ( pGraphics.Get ()->Get () );

                        pTexts->Next ();
//                        pGraphics->Next ();
                    }
                    if ( res == 1 )
                    {
                        pTexts = pTxts.Get ();
                        pTextPoints = pTxtPts.get ();
//                        pGraphics = vGraphics.get ();
                        SetValue ( tpanalysis::TGr_TextPoints, pTextPoints );
                        SetValue ( tpanalysis::TGr_Texts, pTexts );
//                        SetValue ( tpanalysis::TGr_LabelVectors, pGraphics );
                    }
                }


                return res;
            }


            void TPAnalysisModel::createLabel (   ToolpathPtr pToolpath
                               , SharedPtr<container::VectWCharTConcreteForward>& pTxts 
                               , SharedPtr<container::bgeo::VectCoord3dConcreteForward>& pTxtPts
                               , SharedPtr<container::geom::VectTopoGeomPtrConcreteForward>& //vGraphics
                             )
            {
                GraphicAttributesPtr gattrp ( pToolpath->GetGraphicAttributes () );
                if ( !gattrp.Get () )
                {
                    return;
                }
                ToolpathGraphicAttributesPtr tggattrp = gattrp.shared_cast< omsl::graphicattributes::ToolpathGraphicAttributes >( L"ToolpathGraphicAttributes" );
                if ( !tggattrp.Get () )
                {
                    return;
                }


                int sindex = GetValue ( tpanalysis::UI_sliderValueMin );
                
                int tl2= DEFAULTSEGMAX ,tl1 = sindex;
                ToolpathSegmentPtr sSeg;
                ToolpathSegmentPtr lSeg;
                bool visible = true;
                if ( (hcNTLib::ToolPathGetNearestIndex ( pToolpath, tl1, tl2, sSeg, lSeg ,visible )&0x0001) == 0x0001 )
                {
                    sindex = tl1;
                }

                if ( !sSeg.Get () )
                    return;

                if ( sindex >= sSeg->Size () )
                    return;

                TopoGeomPtr pTopo = sSeg.shared_cast< omsl::geom::TopoGeom >( L"TopoGeom" );
                if ( pTopo && pTopo->GetAttribs () )
                {
                    omsl::bgeo::Point3d  p1 ( *sSeg->GetPoint ( sindex ) );

                    p1 [0] += 0.5;
                    p1 [1] += 0.5;
                    p1 [2] += 0.5;

                    tpanalysis::Coord3dIt_T  pTextPoints = GetValue ( tpanalysis::TGr_TextPoints );
                    if ( pTextPoints.Get () && pTextPoints->Size () )
                    {
                        bool pdouble = false;
                        while ( pdouble == false )
                        {
                            for ( OMSL_EACH ( pTextPoints ) )
                            {
                                omsl::bgeo::Point3d  p2 ( pTextPoints->Get () );
                                double d = p2.Distance ( p1 );
                                if ( d < 0.001 )
                                {
                                    // same point, do not add the label
                                    pdouble = true;
                                    break;
                                }
                            }
                            if ( pdouble == true )
                            {
                                p1 [0] += 0.5;
                                p1 [1] += 0.5;
                                p1 [2] += 0.5;
                                pTextPoints->MoveToBegin ();
                                pdouble = false;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                    // add vector
                    // skipped : line color could confuse customer
                    //SharedPtr<geom::Line3d> pLine = geom::Line3d::CreateFrom2Points ( omsl::bgeo::Point3d ( *p0 ), p1 );

                    //vGraphics->PushBack ( pLine.Get () );
                    ToolpathData* ptpData = GetToolpathData ( sSeg );
                    if ( ptpData )
                    {
                        if ( ptpData->GetInternalTpType () != eCt_tp && ptpData->GetInternalTpType () != eCt_ccp ) // label only for master
                            return;

                        omsl::SharedWStringPtr str1 ( omsl::WStringImpl::Create ( ptpData->Jobname () ) );
                        pTxts->PushBack ( str1->Get () );
                        pTxtPts->PushBack ( p1 );
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    return;
                }
            }

            void TPAnalysisModel::SetupTemporaryArcMovements( const int viewModeParameter, int type )
            {
                bool old = SetSignalListening ( false );
                tpanalysis::TopoGeomIt_T pGraphics;
                SetValue ( tpanalysis::TGr_MovementArcMarkers, pGraphics );

                if ( viewModeParameter&vm_SHOWLABEL )
                {
                    int sel = 0;
                    //CadEntityPtr cent ( entity );

                    //ToolpathPtr pToolpath = cent.shared_cast < omsl::hcntcore::ToolPathEntity > ( L"ToolPathEntity" );
                    std::vector<SharedPtr<geom::TopoGeom> > vGraphics;

                    if ( type == 0 ) // cleanup
                    {
                        return;
                    }

                    for ( sel = MASTER_TP ; sel < ALL_SEL; sel++ )
                    {
                        // ToDo define object                     
                        if ( s_data.m [sel].m_compTPAMode == tpanalysis::cm_Point )
                            continue;
                        if ( type == -sel )
                            continue;
                        if ( s_data.m [sel].pointVec.size () > 0 )
                        {
                            int ix = s_data.m [sel].specialIndex [analyseData::dai_SXP1] - 1;
                            int typ = 999;
                            if ( ix >= 0 )
                                typ = s_data.m [sel].typeVec [ix];

                            if ( typ == analyseData::dai_tArc ) // selected an Toolpath
                            {
                                SharedPtr<geom::Line3d> pLine1 = geom::Line3d::CreateFrom2Points ( s_data.m [sel].points [analyseData::dai_CTR1], s_data.m [sel].points [analyseData::dai_SXS] );
                                vGraphics.push_back ( SharedPtr<geom::TopoGeom> ( pLine1.Get () ) );
                                SharedPtr<geom::Point3d> pPoint = geom::Point3d::CreateFromBGeoPoint ( s_data.m [sel].points [analyseData::dai_CTR1] );
                                vGraphics.push_back ( SharedPtr<geom::TopoGeom> ( pPoint.Get () ) );
                                SharedPtr<geom::Line3d> pLine2 = geom::Line3d::CreateFrom2Points ( s_data.m [sel].points [analyseData::dai_CTR1], s_data.m [sel].points [analyseData::dai_SXE] );
                                vGraphics.push_back ( SharedPtr<geom::TopoGeom> ( pLine2.Get () ) );
                            }
                        }
                    }
                    
                    if ( !vGraphics.empty () )
                    {
                        container::geom::VectTopoGeomPtrConcreteForward *pIt = VTGCF_NEW (vGraphics);
                        pGraphics = pIt;
                        //pIt->GetContainerReference ().swap ( vGraphics );
                        SetValue ( tpanalysis::TGr_MovementArcMarkers, pGraphics );
                    }
                }

                SetSignalListening ( old );

//                SetValue ( tpanalysis::TGr_MovementCenterMarkers, pPoints );

            }

            void TPAnalysisModel::SetupTemporaryGraphicsLabel ( const int viewModeParameter, omsl::hcntcore::CadEntity* entity )
            {
                tpanalysis::TopoGeomIt_T pGraphics = GetValue ( tpanalysis::TGr_LabelVectors );
                tpanalysis::Coord3dIt_T  pTextPoints = GetValue ( tpanalysis::TGr_TextPoints );
                tpanalysis::WCharIt_T    pTexts = GetValue ( tpanalysis::TGr_Texts );
                
                bool old = SetSignalListening ( false );

                if ( viewModeParameter&vm_SHOWLABEL )
                {
                    CadEntityPtr cent ( entity );

                    ToolpathPtr pToolpath = cent.shared_cast <omsl::hcntcore::ToolPathEntity> ( L"ToolPathEntity" );

                    if ( pToolpath.Get () ) // selected an Toolpath
                    {
                        SharedPtr<container::VectWCharTConcreteForward> pTxts ( VWCF_NEW () );
                        SharedPtr<container::bgeo::VectCoord3dConcreteForward> pTxtPts ( VC3DCF_NEW () );
                        SharedPtr<container::geom::VectTopoGeomPtrConcreteForward> vGraphics ( VTGCF_NEW () );
                        
                        if ( pTextPoints.Get () && pTextPoints->Size () > 0 )
                        {
//                            int s = pGraphics->Size ();
                            for ( OMSL_EACH ( pGraphics ) )
                            {
                                vGraphics->PushBack ( pGraphics.Get ()->Get () );
                            }
                            for ( OMSL_EACH ( pTextPoints ) )
                            {
                                pTxtPts->PushBack ( pTextPoints.Get ()->Get () );
                            }
                            for ( OMSL_EACH ( pTexts ) )
                            {
                                pTxts->PushBack ( pTexts.Get ()->Get () );
                            }
                        }

                        createLabel ( pToolpath, pTxts, pTxtPts, vGraphics );

                        pTexts = pTxts.Get ();
                        pTextPoints = pTxtPts.get ();
                        pGraphics = vGraphics.get ();
                    }
                    else
                    {
                        SetSignalListening ( old );
                        return;
                    }
                }

                SetSignalListening ( old );

                SetValue ( tpanalysis::TGr_TextPoints, pTextPoints );
                SetValue ( tpanalysis::TGr_Texts, pTexts );
                SetValue ( tpanalysis::TGr_LabelVectors, pGraphics );
            }

            void TPAnalysisModel::SetupTemporaryGraphicsLabel ( const int oldfilter )
            {
                tpanalysis::TopoGeomIt_T pGraphics;
                tpanalysis::Coord3dIt_T  pTextPoints;
                tpanalysis::WCharIt_T    pTexts;
                SetValue ( tpanalysis::TGr_TextPoints, pTextPoints );
                SetValue ( tpanalysis::TGr_Texts, pTexts );
                SetValue ( tpanalysis::TGr_LabelVectors, pGraphics );

                if ( GetValue ( tpanalysis::UI_Label ) )
                {
                    int tpval = GetValue(tpanalysis::UI_Env_ShowAdditionalPathes);
                    int filterval = (tpval & (1 << eCt_ccp) | (1 << eCt_tp));
                    // loopTPMap
                    ToolPathPtrgraphDispValMap::const_iterator cit;
                    for ( cit = s_data.pToolpathMap.cbegin (); cit != s_data.pToolpathMap.cend (); cit++ )
                    {
                        if ((tpval & (1<<eCt_UseUISetting)))
                        {
                            if ( (*cit).second.tptype1 != -1 && ( filterval & (1 << (*cit).second.tptype1) ) )
                            {
                                removeLabel((*cit).first.Get());
//                                if (!((oldfilter & filterval) & (1 << (*cit).second.tptype1)))
                                {
                                    SetupTemporaryGraphicsLabel(vm_SHOWLABEL, (*cit).first.Get());
                                }
                            }
                            if ((*cit).second.tptype2 != -1 && (filterval & (1 << (*cit).second.tptype2)))
                            {
                                removeLabel((*cit).first.Get());
//                                if (!((oldfilter & filterval) & (1 << (*cit).second.tptype2)))
                                {
                                    SetupTemporaryGraphicsLabel(vm_SHOWLABEL, (*cit).first.Get());
                                }
                            }
                        }
                        else if ((*cit).second.tptypeMain == int(eCt_tp))
                        {
                            removeLabel((*cit).first.Get());
                            SetupTemporaryGraphicsLabel(vm_SHOWLABEL, (*cit).first.Get());
                        }
                    }
                }
            }

            int TPAnalysisModel::HighlightToolPath ( const SharedPtr<iterator::hcntcore::CadEntityForward>& pEntsIt1, const SharedPtr<iterator::hcntcore::CadEntityForward>& pEntsIt2 )
            {
                for ( OMSL_EACH ( pEntsIt1 ) )
                {
                    SharedPtr<hcntcore::CadEntity> pEnt ( pEntsIt1->Get () );
                    if ( !pEnt.Get () )
                        continue;
                    ToolpathPtr pToolpath;
                    pToolpath = pEnt.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );

                    HighlightToolPath2 ( pToolpath, analyseData::dai_Index1stTpEnt, 0 );
                }
                for ( OMSL_EACH ( pEntsIt2 ) )
                {
                    SharedPtr<hcntcore::CadEntity> pEnt ( pEntsIt2->Get () );
                    if ( !pEnt.Get () )
                        continue;
                    ToolpathPtr pToolpath;
                    pToolpath = pEnt.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );

                    HighlightToolPath2 ( pToolpath, analyseData::dai_Index2ndTpEnt, 1 );
                    break;
                }
                return 0;
            }

            int TPAnalysisModel::HighlightToolPath2 ( const SharedPtr < hcntcore::ToolPathEntity>& pTPEnt, const int index, unsigned int sel )
            {
                int counts = 0;
                //SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPntVec;
                //SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPntVec1;
                //SharedPtr<omsl::container::bgeo::VectCoord3dConcreteForward> pPntVec2;

                if ( m_withUpdate == false )
                {
                    m_withUpdate = true;

                    return 0;
                }

                tpanalysis::Coord3dIt_T pPoint;

                int vm = GetViewModeValue ();

                //SetValue ( tpanalysis::TGr_PreviewPoints_Ent1, pPoint );
                //SetValue ( tpanalysis::TGr_PreviewPoints_Ent2, pPoint );

                bool old = SetSignalListening ( false );

                std::vector<unsigned int> tviewVec;
                // restore settings

                if ( s_data.pSelectedTP [index].Get () != pTPEnt.Get () )
                {
                    ToolpathPtr pToolpath = s_data.pSelectedTP [index];
                    if ( pToolpath.Get () ) // selected an Toolpath
                    {
                        GraphicAttributesPtr gattrp1 ( pToolpath->GetGraphicAttributes () );
                        if ( !gattrp1.Get () )
                        {
                            SetSignalListening ( old );
                            return -1;
                        }
                        ToolpathGraphicAttributesPtr tggattrp1 = gattrp1.shared_cast< omsl::graphicattributes::ToolpathGraphicAttributes >( L"ToolpathGraphicAttributes" );
                        if ( !tggattrp1.Get () )
                        {
                            SetSignalListening ( old );
                            return-1;
                        }
                        omsl::graphicattributes::Status stat;

                        omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> ents ( pToolpath->GetCadEntities () );
                        if ( !ents.Get () )
                        {
                            SetSignalListening ( old );
                            return -1;
                        }
                        //                            int err = 0;
                        stat = omsl::graphicattributes::OK;
                        int i = 0;
                        for ( OMSL_EACH ( ents ) )
                        {
                            omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );

                            if ( !cep.Get () )
                                continue;

                            ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep, sel ) );

                            GraphicAttributesPtr gattrp ( cep->GetGraphicAttributes () );
                            if ( !gattrp.Get () )
                                continue;

                            ToolpathSegmentGraphicAttributesPtr tggattrp = gattrp.shared_cast< omsl::graphicattributes::ToolpathSegmentGraphicAttributes >( L"ToolpathSegmentGraphicAttributes" );
                            if ( !tggattrp.Get () )
                                continue;

                            // reset colors
                            tggattrp->GetColorsPalette ()->SetColor ( 0, const_cast < omsl::materials::ColorMaterial* > ( tggattrp->GetColorsPalette ()->GetColor ( 4 ) ) );
                            tggattrp->GetColorsPalette ()->SetColor ( 1, const_cast < omsl::materials::ColorMaterial* > ( tggattrp->GetColorsPalette ()->GetColor ( 5 ) ) );

                            // reset original data

                            if ( i < s_data.m[sel].pointSizeVec.size () )
                            {
                                PointCloudGraphicAttributesPtr pcgattr ( tggattrp->GetPointsAttributes () );

                                if ( pcgattr.Get () )
                                    pcgattr->SetGraphicWidth ( ( float ) s_data.m[sel].pointSizeVec [i] );
                            }

                            if ( i < s_data.m[sel].viewVec.size () )
                            {
                                stat = tggattrp->SetShowPoints ( ( s_data.m[sel].viewVec [i] & analyseData::dai_viewOrigPoints ) ? omsl::True : omsl::False );
                                stat = tggattrp->SetShowNormals ( ( s_data.m[sel].viewVec [i] & analyseData::dai_viewOrigNormals ) ? omsl::True : omsl::False );
                            }

                            cep->UpdateGraphicAttributes ( gattrp.Get () );

                            i++;
                        }

                        pToolpath->UpdateGraphicAttributes ( gattrp1.Get () );
                        pToolpath.Get ()->SetUpdateNeeded ( omsl::True );
                    }
                }

                // to store temporary viewdata
                
                s_data.pSelectedTP [index] = pTPEnt;

                if ( !pTPEnt.Get () )
                {
                    ResetPreviewData ( analyseData::dai_Index1stTpEnt,sel );
                    ResetPreviewData ( analyseData::dai_Index1stTpSeg,sel );

//                    SetupTemporaryGraphicsDirection ( vm );
                    SetupCurrentToolDirection ( vm );
                    SetupTemporaryTPGraphics ( omsl::hmbroadcast::bcs_TPCMShowGV, vm );
                    SetupTemporaryTPGraphics ( omsl::hmbroadcast::bcs_TPCMShowPt, vm );
                    SetSignalListening ( old );
                    return 0;
                }
                else
                {
                    ResetPreviewData ( analyseData::dai_Index1stTpEnt,sel );
                }

                if ( pTPEnt.Get () )
                {
                    ToolpathPtr pToolpath = pTPEnt;

                    GraphicAttributesPtr gattrp1 ( pToolpath->GetGraphicAttributes () );
                    if ( !gattrp1.Get () )
                    {
                        SetSignalListening ( old );
                        return -1;
                    }
                    ToolpathGraphicAttributesPtr tggattrp1 = gattrp1.shared_cast< omsl::graphicattributes::ToolpathGraphicAttributes >( L"ToolpathGraphicAttributes" );
                    if ( !tggattrp1.Get () )
                    {
                        SetSignalListening ( old );
                        return -1;
                    }
                    omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> ents ( pToolpath->GetCadEntities () );
                    if ( !ents.Get () )
                    {
                        SetSignalListening ( old );
                        return -1;
                    }
                    //                            int err = 0;
                    omsl::graphicattributes::Status stat;
                    stat = omsl::graphicattributes::OK;
                    int i = 0;
                    for ( ents->MoveToBegin (); !ents->IsAtEnd (); ents->Next () )
                    {
                        omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );
                        if ( !cep.Get () )
                            continue;

                        ToolpathSegmentPtr pSegment ( CastToToolpathSegment ( cep, sel ) );

                        GraphicAttributesPtr gattrp ( cep->GetGraphicAttributes () );
                        if ( !gattrp.Get () )
                            continue;

                        ToolpathSegmentGraphicAttributesPtr tggattrp = gattrp.shared_cast< omsl::graphicattributes::ToolpathSegmentGraphicAttributes >( L"ToolpathSegmentGraphicAttributes" );
                        if ( !tggattrp.Get () )
                            continue;

                        //omsl::materials::ColorsPalette* p = tggattrp->GetColorsPalette ();

                        //const omsl::materials::ColorMaterial* c0 = tggattrp->GetColorsPalette ()->GetColor ( 0 );
                        //const omsl::materials::ColorMaterial* c1 = tggattrp->GetColorsPalette ()->GetColor ( 1 );
                        //const omsl::materials::ColorMaterial* c2 = tggattrp->GetColorsPalette ()->GetColor ( 2 );
                        //const omsl::materials::ColorMaterial* c3 = tggattrp->GetColorsPalette ()->GetColor ( 3 );
                        //const omsl::materials::ColorMaterial* c4 = tggattrp->GetColorsPalette ()->GetColor ( 4 );
                        //const omsl::materials::ColorMaterial* c5 = tggattrp->GetColorsPalette ()->GetColor ( 5 );

                        tggattrp->GetColorsPalette ()->SetColor ( 0, const_cast < omsl::materials::ColorMaterial* > ( tggattrp->GetColorsPalette ()->GetColor ( 2 ) ) );
                        tggattrp->GetColorsPalette ()->SetColor ( 1, const_cast < omsl::materials::ColorMaterial* > ( tggattrp->GetColorsPalette ()->GetColor ( 3 ) ) );

                        //const omsl::materials::ColorMaterial* c10 = tggattrp->GetColorsPalette ()->GetColor ( 0 );
                        //const omsl::materials::ColorMaterial* c11 = tggattrp->GetColorsPalette ()->GetColor ( 1 );
                        //const omsl::materials::ColorMaterial* c12 = tggattrp->GetColorsPalette ()->GetColor ( 2 );
                        //const omsl::materials::ColorMaterial* c13 = tggattrp->GetColorsPalette ()->GetColor ( 3 );

                        // store the original viewmodes
                        int view = 0;
                        view |= tggattrp->GetShowPoints () == omsl::True ? analyseData::dai_viewOrigPoints : 0;
                        view |= tggattrp->GetShowNormals () == omsl::True ? analyseData::dai_viewOrigNormals : 0;

                        s_data.m[sel].viewVec.push_back ( view );


                        if ( vm&vm_SHOWVECTORS )
                        { // modify palette ???
                            tggattrp->SetShowNormals ( omsl::True );
                        }
                        else
                        {// modify palette ???
                            tggattrp->SetShowNormals ( omsl::False );
                        }

                        PointCloudGraphicAttributesPtr pcgattr ( tggattrp->GetPointsAttributes ( omsl::kTrue ) );

                        if ( pcgattr.Get () )
                        {
                            double width = pcgattr->GetGraphicWidth ();

                            s_data.m[sel].pointSizeVec.push_back ( width );

                            // not necessary yet                                    pcgattr->SetGraphicWidth ( width*2.0 );
                                                                //omsl::materials::Material* pm = pcgattr->GetMaterial ();

                                                                //pm->ApplyDiffuse ( const_cast < omsl::materials::ColorMaterial* > ( c13 ) );
                                                                //float r, g, b;
                                                                //c12->GetAll ( r, g, b );
                                                                //pcgattr->SetMaterial ( pm );
                        }

                        // set the temporary view modes

                        if ( vm&vm_SHOWPOINTS )
                        { // modify palette ???
                            stat = tggattrp->SetShowPoints ( omsl::True );
                        }
                        else
                        {// modify palette ???
                            stat = tggattrp->SetShowPoints ( omsl::False );
                        }

                        cep->UpdateGraphicAttributes ( gattrp.Get () );

                        i++;
                    }

                    pToolpath->UpdateGraphicAttributes ( gattrp1.Get () );

                    pToolpath.Get ()->SetUpdateNeeded ( omsl::True );
                }


                // end fill data

                //tpanalysis::Coord3dIt_T pPointIt ( pPntVec.get () );
                //// only if new data

                //pPointIt = pPntVec1.get ();
                //pPointIt = pPntVec2.get ();

                SetValue ( tpanalysis::DummyParameter, ( int ) counts );
//                SetValue ( tpanalysis::ToolpathCounter, ( int ) s_data.m[sel].pTpSelectionVec->Size () );

                SetSignalListening ( old );

//                SetupTemporaryGraphicsLabel ( viewModeParameter );
//                SetupTemporaryGraphicsDirection ( viewModeParameter );

                SetupTemporaryTPGraphics ( omsl::hmbroadcast::bcs_TPCMShowGV, vm );
                SetupTemporaryTPGraphics ( omsl::hmbroadcast::bcs_TPCMShowPt, vm );
                SetupCurrentToolDirection ( vm );
                return counts;
            }

            void TPAnalysisModel::SetupTemporary_Measure_Graphic ()
            {
                tpanalysis::TopoGeomIt_T pGraphics;
                tpanalysis::Coord3dIt_T  pTextPoints;
                tpanalysis::WCharIt_T    pTexts;

                // clear all
                SetValue ( tpanalysis::Msr_DistPointsTexts, pTexts );
                SetValue ( tpanalysis::Msr_TextPoints, pTextPoints );
                SetValue ( tpanalysis::Msr_DistanceTexts, pTexts );
                SetValue ( tpanalysis::Msr_DistanceTextPoints, pTextPoints );
                SetValue ( tpanalysis::Msr_DistanceLine, pGraphics );
                SetValue ( tpanalysis::Msr_DistPoints, pTextPoints );

                if ( s_data.m [L_SEL].specialIndex [analyseData::dai_SXP1] == UNKNOWNINDEX )
                    return;

                if ( s_data.m [R_SEL].specialIndex [analyseData::dai_SXP1] == UNKNOWNINDEX )
                    return;

                // distance text
                {
                    SharedPtr<container::VectWCharTConcreteForward> pTxts ( VWCF_NEW () );
                    SharedPtr<container::bgeo::VectCoord3dConcreteForward> pTxtPts ( VC3DCF_NEW () );

                    pTextPoints = GetValue ( tpanalysis::Msr_TextPoints );
                    pTexts = GetValue ( tpanalysis::Msr_DistPointsTexts );

                    double pm = s_data.m [L_SEL].points [analyseData::dai_SXPC].Distance (s_data.m [R_SEL].points [analyseData::dai_SXPC]);

                    if ( pm < 1.0e-3 )
                        return;

                    // conversion to inch if necessary
                    SharedPtr<measureunit::Converter> pConverter;
                    hcntcore::Model* model = ( hcntcore::Model* )m_pDocument->Cast ( L"Model" );
                    pConverter = omsl::measureunit::Converter::CreateGeneric ( omsl::units::Dimension::Length (), model );
                    pConverter->SetDecimals ( 3 );
                    // fill vector with rounded values
                    pConverter->SetInner ( pm );
                    const wchar_t* s2 = pConverter->Get ();

                    pTxts->PushBack ( s2 );

                    pTxtPts->PushBack ( (s_data.m [L_SEL].points [analyseData::dai_SXPC]+s_data.m [R_SEL].points [analyseData::dai_SXPC])/2.0 );

                    pTexts = pTxts.Get ();
                    pTextPoints = pTxtPts.get ();

                    SetValue ( tpanalysis::Msr_DistanceTextPoints, pTextPoints );
                    SetValue ( tpanalysis::Msr_DistanceTexts, pTexts );
                }

                // texts , points for start / end
                {
                    SharedPtr<container::VectWCharTConcreteForward> pTxts ( VWCF_NEW () );
                    SharedPtr<container::bgeo::VectCoord3dConcreteForward> pTxtPts ( VC3DCF_NEW () );
                    SharedPtr<container::bgeo::VectCoord3dConcreteForward> pDistPts ( VC3DCF_NEW () );

                    pTextPoints = GetValue ( tpanalysis::Msr_TextPoints );
                    pTexts = GetValue ( tpanalysis::Msr_DistPointsTexts );

                    omsl::SharedWStringPtr str1 ( omsl::WStringImpl::Create ( L"1" ) );
                    pTxts->PushBack ( str1->Get () );
                    omsl::SharedWStringPtr str2 ( omsl::WStringImpl::Create ( L"2" ) );
                    pTxts->PushBack ( str2->Get () );

                    //double xoffset = 0.0;
                    //double yoffset = 0.0;
                    //double _cvh = 1.0;
                    typedef omsl::SharedPtr<omsl::modelgraphic::GraphicModelViewer> GraphicModelViewerPtr;
                    typedef omsl::SharedPtr<omsl::graphics::View> GraphicsViewPtr;
                    omsl::bgeo::Point3d  p1 ( s_data.m [L_SEL].points [analyseData::dai_SXPC] );
                    omsl::bgeo::Point3d  p2 ( s_data.m [R_SEL].points [analyseData::dai_SXPC] );
                    p1 [0] += 0.5;
                    p1 [1] += 0.5;
                    p1 [2] += 0.5;
                    p2 [0] -= 0.5;
                    p2 [1] -= 0.5;
                    p2 [2] -= 0.5;

                    pTxtPts->PushBack ( p1 );
                    pTxtPts->PushBack ( p2 );
                    pDistPts->PushBack ( s_data.m [L_SEL].points [analyseData::dai_SXPC] );
                    pDistPts->PushBack ( s_data.m [R_SEL].points [analyseData::dai_SXPC] );

                    pTexts = pTxts.Get ();
                    pTextPoints = pTxtPts.get ();

                    SetValue ( tpanalysis::Msr_TextPoints, pTextPoints );
                    SetValue ( tpanalysis::Msr_DistPointsTexts, pTexts );

                    pTextPoints = pDistPts.get ();
                    SetValue ( tpanalysis::Msr_DistPoints, pTextPoints );
                }
                // distance line
                {
                    std::vector<SharedPtr<geom::TopoGeom> > vGraphics;

                    SharedPtr<geom::Line3d> pLine1 = geom::Line3d::CreateFrom2Points ( s_data.m [L_SEL].points [analyseData::dai_SXPC], s_data.m [R_SEL].points [analyseData::dai_SXPC] );
                    vGraphics.push_back ( SharedPtr<geom::TopoGeom> ( pLine1.Get () ) );
                    if ( !vGraphics.empty () )
                    {
                        container::geom::VectTopoGeomPtrConcreteForward *pIt = VTGCF_NEW (vGraphics);
                        pGraphics = pIt;
//                        pIt->GetContainerReference ().swap ( vGraphics );
                        SetValue ( tpanalysis::Msr_DistanceLine, pGraphics );
                    }
                }
            }
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
