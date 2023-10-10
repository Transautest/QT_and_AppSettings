#pragma warning ( disable : 4996 ) // for warnings: due to compare of boost::uuids
#include "TPAnalysisModel.h"
                                  // keys
#include <Commands\tpanalysiskeys.h>

#include <hcntcore/document.h>
#include <hcntcore\model.h>
#include <hcntcore/cadentityiterators.h>
#include <geom/geometriciterators.h>

#include <hcntcore/cadentitymgr.h>
#include <geom/line3d.h>
#include <geom/curve3d.h>
#include <geom/face.h>
#include <geom/surface.h>
#include <geom/PolyLine.h>
#include <geom/extrema.h>
#include <hcntcore/pargeomentity.h>
#include <hcntcore/curve.h>
#include <hcntcore\referencesystemmgr.h>
#include <hcntcore\referencesystem.h>

#include <hcntcore/polylineithpoint.h>

#include <geom/attributes.h>
#include <geom/attributerefcounter.h>
#include <..\..\..\INTERFACES\iHCNT\iTopoAttrib.h>

#include <hcntcore/application.h>

#include "../Commands/common_functions.h"
#include <objtable/objitemadapter.h>
#include <bgeo/bgeotools.h>

typedef omsl::SharedPtr<omsl::hcntcore::ToolPathEntity> ToolPathEntityPtr;
typedef omsl::SharedPtr<omsl::hcntcore::GeomEntity> GeomEntityPtr;
typedef omsl::SharedPtr<omsl::geom::Geom3d> Geom3dPtr;

using namespace omsl::command;
using namespace omsl::commands::tp_analyzer;

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            SharedPtr<propertyservice::PropertyBag> TPAnalysisModel::CreatePropertyBag ()
            {
                SharedPtr<propertyservice::PropertyBag> pBag0 = propertyservice::PropertyBag::Create ();
                // ToDo setup the path via setup / install
                if ( pBag0.Get () == nullptr )
                {
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
                                s += L"\\files\\commands\\ToolpathAnalyser\\tp.analysis.description";
                                m_descriptionPath = omsl::WStringImpl::Create ( s.c_str () );
                            }
                        }
                    }
                }

                if ( m_descriptionPath && m_descriptionPath.Get () )
                    pBag0->SetName ( m_descriptionPath.Get ()->Get () );

                return pBag0;
            }

            bool TPAnalysisModel::ComputeSelection ( const CadEntitiesIt_T& pEnts, int sel )
            {
                if ( pEnts && pEnts->Size () )
                {
                    ToolpathSegmentPtr p_segment;
                    pEnts->MoveToBegin ();
                    SharedPtr<hcntcore::CadEntity> pEnt ( pEnts->Get () );

                    p_segment = CastToToolpathSegment ( pEnt, sel );

                    if ( p_segment.Get () == nullptr )
                    {
                        m_withUpdate = true;
                        return false;
                    }

                    s_data.m[sel].m_pAttr = nullptr;

                    TopoGeomPtr pTopo = p_segment.shared_cast< omsl::geom::TopoGeom >( L"TopoGeom" );
                    if ( pTopo && pTopo->GetAttribs () )
                    {

                        s_data.m[sel].m_pAttr = pTopo->GetAttribs ()->Get ( L"HM_ToolpathData" );
                        if ( s_data.m[sel].m_pAttr )
                        {
                            ToolpathData* ptpData = GetToolpathData ( s_data.m[sel].m_pAttr );
                            if ( ptpData )
                            {
                                s_data.m[sel].pof_frame = ptpData->Frame ();
                                s_data.m[sel].pof_ncs = ptpData->NCS ();
                            }
                        }
                    }

                    // to skip internal updates because of incomplete calculations at this point
                    // call is necessary to setup the matrix before calculate the selected point
                    m_withUpdate = false;
                    OnSetupCoordinateSystem ( GetValue ( tpanalysis::UI_RefSystemType ) );
                    m_withUpdate = true;

                    ComputeOnPoints ( pEnts, sel );
                    return true;
                }
                else
                {
                    ResetPreviewData ( analyseData::dai_Index1stTpSeg, sel );
                }

                return false;
            }

            int TPAnalysisModel::CalculateNearestPoint ( int resMode )
            {
                bgeo::Point3d pt0[ALL_SEL];
                bgeo::Coord3d pt1[ALL_SEL];
                bgeo::Coord3d pt2[ALL_SEL];
                bgeo::Coord3d ptref[ALL_SEL];
                SharedPtr<geom::Curve3d> pcurve [ALL_SEL];
                SharedPtr<geom::TopoGeom> ptopo [ALL_SEL];

                bool valid [ALL_SEL] = { false,false };

                for ( int sel = L_SEL; sel < ALL_SEL; sel++ )
                {
                    int mode = ( int ) s_data.m [sel].m_compTPAMode;

                    switch ( mode )
                    {
                        case tpanalysis::cm_Point:
                        {
                            if ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] != UNKNOWNINDEX )
                            {
                                pt0[sel] = s_data.m [sel].points [analyseData::dai_SXP1];
                                valid [sel] = true;
                            }
                            break;
                        }
                        case tpanalysis::cm_Movement:
                        {
                            if ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] != UNKNOWNINDEX )
                            {
                                bgeo::Point3d npt;

                                SharedPtr<geom::Geom3d> geom;
                                omsl::SharedPtr<omsl::hcntcore::GeomEntity> sep = s_data.m [sel].m_origCadEnt.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                                if ( !sep.Get () )
                                {
                                    SharedPtr<geom::Curve3d> c3 = s_data.m [sel].m_pTpSeg.shared_cast< omsl::geom::Curve3d >( L"Curve3d" );
                                    if ( c3.Get () )
                                    {
                                        const omsl::geom::Curve3d* pc = s_data.m [sel].m_pTpSeg->GetCurve ( s_data.m [sel].specialIndex [analyseData::dai_SXP1] - 1 ); // s_data.m[sel].specialIndex [analyseData::dai_SXP1] );
                                        if ( pc ) // it´s an arc
                                            pcurve[sel] = const_cast <omsl::geom::Curve3d*> (pc);
                                        else
                                        {
                                            SharedPtr<omsl::hcntcore::ParGeomEntity> pParGeom = s_data.m [sel].m_origCadEnt.shared_cast< omsl::hcntcore::ParGeomEntity >( L"ParGeomEntity" );
                                            if ( pParGeom.Get () )
                                            {
                                                omsl::SharedPtr<omsl::hcntcore::pargeom::Geom> ppGeom ( pParGeom->GetParGeom () );
                                                omsl::SharedPtr<omsl::hcntcore::pargeom::Curve> crv = ppGeom.shared_cast< omsl::hcntcore::pargeom::Curve >( L"Curve" );
                                                if ( crv.Get () )
                                                {
                                                    SharedPtr<geom::Curve3d> cr3 ( const_cast < omsl::geom::Curve3d* > ( crv->GeomCurve () ) );
                                                    if ( cr3.Get () )
                                                    {
                                                        pcurve [sel] = cr3;
                                                        valid [sel] = true;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                        valid [sel] = true;
                                    }
                                }
                            }
                            break;
                        }
                        case tpanalysis::cm_Geometry:
                        {
                            SharedPtr<geom::Geom3d> geom;
                            SharedPtr<geom::TopoGeom> topo;

                            omsl::SharedPtr<omsl::hcntcore::GeomEntity> sep = s_data.m [sel].m_origCadEnt.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                            if ( sep.Get () )
                            {
                                topo = sep->TopoGeom ();
                                geom = sep->Geom ();
                            }
                            else
                            {
                                SharedPtr<omsl::hcntcore::ParGeomEntity> pParGeom = s_data.m [sel].m_origCadEnt.shared_cast< omsl::hcntcore::ParGeomEntity >( L"ParGeomEntity" );
                                if ( pParGeom.Get () )
                                {
                                    SharedPtr<omsl::hcntcore::pargeom::Geom> ppGeom ( pParGeom->GetParGeom () );

                                    omsl::SharedPtr<omsl::hcntcore::pargeom::Curve> crv  = ppGeom.shared_cast< omsl::hcntcore::pargeom::Curve >( L"Curve" );
                                    if ( crv.Get () )
                                    {
                                        SharedPtr<geom::Curve3d> c3 ( const_cast < omsl::geom::Curve3d* > ( crv->GeomCurve () ) );
                                        if ( c3.Get () )
                                        {
                                            pcurve [sel] = c3;
                                            valid [sel] = true;
                                            break;
                                        }
                                    }
                                    omsl::SharedPtr<omsl::hcntcore::pargeom::Point> pt = ppGeom.shared_cast< omsl::hcntcore::pargeom::Point >( L"Point" );
                                    if ( pt.Get () )
                                    {
                                        pt0 [sel] = pt->GeometricPoint ();
                                        if ( resMode == tpanalysis::cm_Point_Geometry )
                                            resMode = tpanalysis::cm_Point_Point;
                                        else if ( resMode == tpanalysis::cm_Movement_Geometry )
                                            resMode = tpanalysis::cm_Point_Movement;
                                        valid [sel] = true;
                                        if ( resMode == tpanalysis::cm_Point_Geometry )
                                            resMode = tpanalysis::cm_Point_Point;
                                        else if ( resMode == tpanalysis::cm_Movement_Geometry )
                                            resMode = tpanalysis::cm_Point_Movement;
                                        break;
                                    }
                                }
                            }

                            if ( geom.Get () )
                            {
                                if ( topo.Get () )
                                {
                                    ptopo [sel] = topo;
                                }

                                SharedPtr<geom::Curve3d> c3 = geom.shared_cast< omsl::geom::Curve3d >( L"Curve3d" );
                                if ( c3.Get () )
                                {
                                    pcurve [sel] = c3;
                                    valid [sel] = true;
                                    break;
                                }

                                SharedPtr<geom::Face> f3 = geom.shared_cast< omsl::geom::Face >( L"Face" );
                                if ( f3.Get () )
                                {
                                    if ( !topo.Get () )
                                        ptopo [sel] = f3.Get ();
                                    valid [sel] = true;
                                    break;
                                }

                                SharedPtr<geom::Surface> s3 = geom.shared_cast< omsl::geom::Surface >( L"Surface" );
                                if ( s3.Get () )
                                {
                                    if ( !topo.Get () )
                                        ptopo [sel] = s3.Get ();
                                    valid [sel] = true;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }

                if ( valid [L_SEL] == false || valid [R_SEL] == false )
                    return -1;

                switch ( resMode )
                {
                    case tpanalysis::cm_Point_Point: // data are stored before
                    {
                        s_data.m [L_SEL].points [analyseData::dai_SXPC] = pt0 [L_SEL];
                        s_data.m [R_SEL].points [analyseData::dai_SXPC] = pt0 [R_SEL];

                        break;
                    }
                    case tpanalysis::cm_Point_Movement:
                    {
                        int selP = ( s_data.m [L_SEL].m_compTPAMode == tpanalysis::cm_Point ) ? L_SEL : R_SEL;
                        int selC = ( s_data.m [L_SEL].m_compTPAMode == tpanalysis::cm_Point ) ? R_SEL : L_SEL;

                        if ( selP == selC )
                            return -1;

                        bgeo::Point3d npt;
                        double t;
                        bgeo::Status st;

                        npt = bgeo::Point3d ( pcurve[selC]->NearestPoint ( pt0[selP], t, st ) );
                        if ( st != bgeo::Status::OK )
                            return -1;

                        s_data.m [selP].points [analyseData::dai_SXPC] = pt0 [selP];
                        s_data.m [selC].points [analyseData::dai_SXPC] = npt;

                        break;
                    }
                    case tpanalysis::cm_Point_Geometry :
                    {
                        s_data.m [L_SEL].points [analyseData::dai_SXPC] = pt0 [L_SEL];

                        bgeo::Point3d npt;
                        double t;
                        double distance;
                        omsl::bgeo::Coord2d uv;
                        bgeo::Status st = bgeo::Status::OK;

                        if ( ptopo[R_SEL].Get () )
                        {
                            SharedPtr<geom::Face> f3 = ptopo[R_SEL].shared_cast< omsl::geom::Face >( L"Face" );
                            if ( f3.Get () )
                            {
                                distance = f3->DistanceFromPoint ( pt0 [L_SEL], st , &npt, &uv );
                                if ( st != bgeo::Status::OK )
                                    return -1;
                                s_data.m [R_SEL].points [analyseData::dai_SXPC] = npt;
                                s_data.m [R_SEL].specialIndex [analyseData::dai_SXP1] = UNSPECIFICINDEX;
                                break;
                            }

                            SharedPtr<geom::Surface> s3 = ptopo[R_SEL].shared_cast< omsl::geom::Surface >( L"Surface" );
                            if ( s3.Get () )
                            {
                                distance = s3->DistanceFromPoint ( pt0 [L_SEL], st, &npt, &uv, false );
                                if ( st != bgeo::Status::OK )
                                    return -1;
                                s_data.m [R_SEL].points [analyseData::dai_SXPC] = npt;
                                s_data.m [R_SEL].specialIndex [analyseData::dai_SXP1] = UNSPECIFICINDEX;
                                break;
                            }

                            npt = bgeo::Point3d ( pcurve [R_SEL]->NearestPoint ( pt0 [L_SEL], t, st ) );
                            if ( st != bgeo::Status::OK )
                                return -1;

                            s_data.m [L_SEL].points [analyseData::dai_SXPC] = pt0 [L_SEL];
                            s_data.m [R_SEL].points [analyseData::dai_SXPC] = npt;
                        }
                        else
                        {
                            npt = bgeo::Point3d ( pcurve [R_SEL]->NearestPoint ( pt0 [L_SEL], t, st ) );
                            if ( st != bgeo::Status::OK )
                                return -1;

                            s_data.m [L_SEL].points [analyseData::dai_SXPC] = pt0 [L_SEL];
                            s_data.m [R_SEL].points [analyseData::dai_SXPC] = npt;
                        }

                        if ( st == omsl::bgeo::Status::OK )
                            s_data.m [R_SEL].specialIndex [analyseData::dai_SXP1] = UNSPECIFICINDEX;
                        else
                            s_data.m [R_SEL].specialIndex [analyseData::dai_SXP1] = UNKNOWNINDEX;

                        break;
                    }
                    case tpanalysis::cm_Movement_Movement:
                    {
                        bgeo::UnitVector3d linDir [2];
                        bgeo::Point3d start [2];
                        bgeo::Point3d msrpt [3];
                        omsl::bgeo::Status st = omsl::bgeo::Status::OK;

                        double dist;

                        bool isLinL = pcurve [L_SEL]->IsLinear ( start [L_SEL], linDir [L_SEL], 1.0e-6 );
                        bool isLinR = pcurve [R_SEL]->IsLinear ( start [R_SEL], linDir [R_SEL], 1.0e-6 );

                        if ( isLinL == true && isLinR == true )
                        {
                            bgeo::Coord3d pt [2];
                            dist = bgeo::MinSegmentDist ( pcurve [L_SEL]->End0 (), pcurve [L_SEL]->End1 ()
                                                          , pcurve [R_SEL]->End0 (), pcurve [R_SEL]->End1 ()
                                                          , 1.0e-6, pt [L_SEL], pt [R_SEL] );

                            s_data.m [L_SEL].points [analyseData::dai_SXPC] = bgeo::Point3d ( pt [L_SEL] );
                            s_data.m [R_SEL].points [analyseData::dai_SXPC] = bgeo::Point3d ( pt [R_SEL] );
                        }
                        else
                        {
                            omsl::SharedPtr<geom::ExtremaMin> ex = omsl::geom::ExtremaMin::Create ( pcurve [L_SEL].Get (), pcurve [R_SEL].Get (), 1.0e-6 );

                            st = ex->Perform ();

                            if ( !st )
                            {
                                int n = ex->NOfSolutions ();

                                for ( ULong jm = 0; jm != n; jm++ )
                                {
                                    ex->Point0 ( jm, msrpt [0] );
                                    ex->Point1 ( jm, msrpt [1] );
                                    if ( msrpt [0].IsCoinc ( msrpt [1], 1.0e-6 ) )
                                    {
                                        msrpt[2] = bgeo::MidPoint ( msrpt [0], msrpt [1] );
                                        st = bgeo::OK;  break;
                                    }
                                }

                                s_data.m [L_SEL].points [analyseData::dai_SXPC] = msrpt [0];
                                s_data.m [R_SEL].points [analyseData::dai_SXPC] = msrpt [1];
                            }
                        }

                        if ( st != omsl::bgeo::Status::OK )
                            s_data.m [R_SEL].specialIndex [analyseData::dai_SXP1] = UNKNOWNINDEX;

                        break;
                    }
                    case tpanalysis::cm_Movement_Geometry:
                    {
                        bgeo::UnitVector3d linDir [2];
                        bgeo::Point3d start  [2];
                        bgeo::Point3d msrpt [3];
                        omsl::bgeo::Status st = omsl::bgeo::Status::OK;
                        double dist;
                        if ( ptopo[R_SEL].Get () )
                        {
                            ptopo[L_SEL] = pcurve [L_SEL].Get () ;
                            omsl::SharedPtr<geom::ExtremaMin> ex = omsl::geom::ExtremaMin::Create ( ptopo [L_SEL].Get (), ptopo [R_SEL].Get (), 1.0e-6 );

                            st = ex->Perform ();

                            if ( !st )
                            {
                                int n = ex->NOfSolutions ();

                                for ( ULong jm = 0; jm != n; jm++ )
                                {
                                    ex->Point0 ( jm, msrpt [0] );
                                    ex->Point1 ( jm, msrpt [1] );
                                    if ( msrpt [0].IsCoinc ( msrpt [1], 1.0e-6 ) )
                                    {
                                        msrpt [2] = bgeo::MidPoint ( msrpt [0], msrpt [1] );
                                        st = bgeo::OK;  break;
                                    }
                                }

                                s_data.m [L_SEL].points [analyseData::dai_SXPC] = msrpt [0];
                                s_data.m [R_SEL].points [analyseData::dai_SXPC] = msrpt [1];
                            }
                        }
                        else
                        {
                            bool isLinL = pcurve [L_SEL]->IsLinear ( start [L_SEL], linDir [L_SEL], 1.0e-6 );
                            bool isLinR = pcurve [R_SEL]->IsLinear ( start [R_SEL], linDir [R_SEL], 1.0e-6 );

                            if ( isLinL == true && isLinR == true )
                            {
                                bgeo::Coord3d pt [2];
                                dist = bgeo::MinSegmentDist ( pcurve [L_SEL]->End0 (), pcurve [L_SEL]->End1 ()
                                                              , pcurve [R_SEL]->End0 (), pcurve [R_SEL]->End1 ()
                                                              , 1.0e-6, pt [L_SEL], pt [R_SEL] );

                                s_data.m [L_SEL].points [analyseData::dai_SXPC] = bgeo::Point3d ( pt [L_SEL] );
                                s_data.m [R_SEL].points [analyseData::dai_SXPC] = bgeo::Point3d ( pt [R_SEL] );
                            }
                            else
                            {
                                omsl::SharedPtr<geom::ExtremaMin> ex = omsl::geom::ExtremaMin::Create ( pcurve [L_SEL].Get (), pcurve [R_SEL].Get (), 1.0e-6 );

                                st = ex->Perform ();

                                if ( !st )
                                {
                                    int n = ex->NOfSolutions ();

                                    for ( ULong jm = 0; jm != n; jm++ )
                                    {
                                        ex->Point0 ( jm, msrpt [0] );
                                        ex->Point1 ( jm, msrpt [1] );
                                        if ( msrpt [0].IsCoinc ( msrpt [1], 1.0e-6 ) )
                                        {
                                            msrpt [2] = bgeo::MidPoint ( msrpt [0], msrpt [1] );
                                            st = bgeo::OK;  break;
                                        }
                                    }

                                    s_data.m [L_SEL].points [analyseData::dai_SXPC] = msrpt [0];
                                    s_data.m [R_SEL].points [analyseData::dai_SXPC] = msrpt [1];
                                }
                            }
                        }
                        if ( st == omsl::bgeo::Status::OK )
                            s_data.m [R_SEL].specialIndex [analyseData::dai_SXP1] = UNSPECIFICINDEX;
                        else
                            s_data.m [R_SEL].specialIndex [analyseData::dai_SXP1] = UNKNOWNINDEX;

                        break;
                    }
                    default: // nothing to do
                    {
                        break;
                    }
                }

                return 0;
            }

            SharedPtr<propertyservice::PropertyBag> TPAnalysisModel::Compute ( const CadEntitiesIt_T& pEnts1,
                                                                               const CadEntitiesIt_T& pEnts2 )
            {
                SharedPtr<propertyservice::PropertyBag> pBag0 = CreatePropertyBag ();

                if ( !pEnts1.Get () || !pEnts1->Size () )
                {
                    s_data.reset ( L_SEL );
                    s_data.pPnt_C2ndSegment = nullptr;
                    s_data.pPnt_L1stSegment = nullptr;
                    s_data.pPnt_L2ndSegment = nullptr;
                    s_data.pPnt_R2ndSegment = nullptr;
                    s_data.pPnt_R3rdSegment = nullptr;
                    SetupTemporaryArcMovements ( 0, 0 );
                }
                if ( !pEnts2.Get () || !pEnts2->Size () )
                {
                    s_data.reset ( R_SEL );
                }

                bool _1stSelOk = ComputeSelection ( pEnts1, L_SEL );
                bool _2ndSelOk = ComputeSelection ( pEnts2, R_SEL );

                if ( _1stSelOk && ( _2ndSelOk || ( pEnts2.Get () && pEnts2->Size () ) ) )
                {
                    ComputeOnEnts ( pEnts1, pEnts2 );
//                    SetupPropertyBag ( pBag0, U_SEL );
                }
//                else
                {
//                    SetupPropertyBag ( pBag0, L_SEL );
//                    SetupPropertyBag ( pBag0, R_SEL );
                }

                SetupTemporaryGraphics ();

                tpanalysis::compTPAMode resMode = checkTPAMode ( U_SEL );

                int res = CalculateNearestPoint ( resMode );

                switch ( resMode )
                {
                    case tpanalysis::cm_Point:
                    {
                        SetupPropertyBagFor_P ( pBag0 );
                        SetupPropertyBagFor_T ( pBag0, L_SEL, true );
                        break;
                    }
                    case tpanalysis::cm_Movement:
                    {
                        SetupPropertyBagFor_M ( pBag0 );
                        SetupPropertyBagFor_T ( pBag0, L_SEL, true );
                        break;
                    }
                    case tpanalysis::cm_Point_Point:
                    {
                        if ( res == 0 )
                        {
                            SetupPropertyBagFor_P_P ( pBag0 );
                            SetupPropertyBagFor_T ( pBag0, L_SEL, false );
                            SetupPropertyBagFor_T ( pBag0, R_SEL, false );
                            SetupPropertyBagFor_P_O ( pBag0 );
                        }
                        break;
                    }
                    case tpanalysis::cm_Movement_Movement: 
                    {
                        SetupPropertyBagFor_P_P ( pBag0 );
                        SetupPropertyBagFor_T ( pBag0, L_SEL, false );
                        SetupPropertyBagFor_T ( pBag0, R_SEL, false );
                        SetupPropertyBagFor_P_O ( pBag0 );
                        break;
                    }
                    case tpanalysis::cm_Point_Movement: 
                    {
                        SetupPropertyBagFor_P_P ( pBag0 );
                        SetupPropertyBagFor_T ( pBag0, R_SEL, false );
                        SetupPropertyBagFor_P_O ( pBag0 );
                        break;
                    }
                    case tpanalysis::cm_TPALast: break;
                    default: // nothing to do
                    {
                        SetupPropertyBagFor_P_P ( pBag0 );
                        SetupPropertyBagFor_T ( pBag0, L_SEL, false );
                        SetupPropertyBagFor_P_O ( pBag0 );
                        break;
                    }
                }

                SetupTemporary_Measure_Graphic ();

                m_withUpdate = true;
                return pBag0;
            }

            void TPAnalysisModel::ComputeOnEnts
            (
                  const CadEntitiesIt_T& pEnts1       // [in] selected object1
                , const CadEntitiesIt_T& pEnts2       // [in] selected object2
            )
            {
                StoreEntityPoint ( pEnts1, analyseData::dai_SXP1, L_SEL );
                StoreEntityPoint ( pEnts2, analyseData::dai_SXP2, R_SEL );

                // Compute necessary things
                ComputeAnalyseData ( pEnts1, L_SEL );
                ComputeAnalyseData ( pEnts2, R_SEL );
                // show / create graphic
                SetupTemporaryGraphics ();
                // show / calculate measurements
            }

            void TPAnalysisModel::CalcBaseDataForTempGraphic ( int sel )
            {
                bgeo::Coord3d pt_0 = s_data.m[sel].points [analyseData::dai_SXP1];
                bgeo::Vector3d Xx = s_data.m[L_SEL].points [analyseData::dai_REFP] - s_data.m[sel].points [analyseData::dai_SXP1];
                if ( !Xx.IsZero () )
                {
                    bgeo::Point3d pCenter;
                    bgeo::Point3d p1, p2, p3, p4, p5;

                    bgeo::UnitVector3d X ( Xx );
                    bgeo::UnitVector3d Z ( 1., 0., 0. );
                    if ( X.IsParal ( Z, 0.01 ) )
                    {
                        Z.Set ( 0., 1., 0. );
                        if ( X.IsParal ( Z, 0.01 ) )
                        {
                            Z.Set ( 0., 0., 1. );
                        }
                    }
                    bgeo::UnitVector3d Y;
                    bgeo::CrossAndNormalize ( Z, X, Y );
                    pCenter = bgeo::Point3d ( pt_0, 0.001 );
                    p1 = bgeo::Point3d ( pt_0 + Y*10., 0.01 );
                    p2 = bgeo::Point3d ( pt_0 + X*5., 0.01 );
                    p3 = bgeo::Point3d ( pt_0 + Z*5., 0.01 );
                    p4.Set ( p2 );
                    p5.Set ( p3 );
                    s_data.m[sel].points [analyseData::dai_SXC] = pCenter;
                    s_data.m[sel].points [analyseData::dai_DIR1] = p1;
                    s_data.m[sel].points [analyseData::dai_DIR2] = p2;
                }
            }

            void TPAnalysisModel::ComputeOnPoints ( const CadEntitiesIt_T& pEnts1, int sel )
            {
                StoreEntityPoint ( pEnts1, analyseData::dai_SXP1, sel );

                ComputeAnalyseData ( pEnts1, sel );

                // show / create graphic
                SetupTemporaryGraphics ();
                // show / calculate measurements
            }

            void TPAnalysisModel::ComputeAnalyseData ( const CadEntitiesIt_T& pEnts1, int sel )
            {
                ToolpathSegmentPtr pToolpathSegment;

                // set pickpoint
                CalcBaseDataForTempGraphic ( sel );

                {
                    pEnts1->MoveToBegin ();
                    SharedPtr<hcntcore::CadEntity> pEnt ( pEnts1->Get () );

                    SetInternalSelection ( pEnt, sel );
                }

                s_data.m[sel].vectors [0] = bgeo::Vector3d ( 1, 0, 0 );
                s_data.m[sel].vectors [1] = bgeo::Vector3d ( 0, 1, 0 );
                s_data.m[sel].vectors [2] = bgeo::Vector3d ( 0, 0, 1 );
                s_data.m[sel].vectors [3] = bgeo::Vector3d ( 1, 0, 0 );
                s_data.m[sel].vectors [4] = bgeo::Vector3d ( 0, 1, 0 );
                s_data.m[sel].vectors [5] = bgeo::Vector3d ( 0, 0, 1 );
                s_data.m[sel].vectors [6] = bgeo::Vector3d ( 1, 0, 0 );
                //s_data.vectors [7] = bgeo::Vector3d ( 0, 1, 0 );
                //s_data.vectors [8] = bgeo::Vector3d ( 0, 0, 1 );

                s_data.pPnt_L2ndSegment = SharedPtr<container::bgeo::VectCoord3dConcreteForward> ();
                s_data.pPnt_C2ndSegment = SharedPtr<container::bgeo::VectCoord3dConcreteForward> ();
                s_data.pPnt_R2ndSegment = SharedPtr<container::bgeo::VectCoord3dConcreteForward> ();
                s_data.pPnt_L1stSegment = SharedPtr<container::bgeo::VectCoord3dConcreteForward> ();
                s_data.pPnt_R3rdSegment = SharedPtr<container::bgeo::VectCoord3dConcreteForward> ();

                s_data.m[sel].specialIndex [analyseData::dai_SXS] = s_data.m[sel].specialIndex [analyseData::dai_SXC] = s_data.m[sel].specialIndex [analyseData::dai_SXE] = 0;
                s_data.m[sel].specialIndex [analyseData::dai_SXE] = s_data.m[sel].specialIndex [analyseData::dai_SXS] + analyseData::dai_2ndSegmentSize;
//                s_data.specialIndex [analyseData::dai_1stSeg] = 0;
//                s_data.specialIndex [analyseData::dai_Index1stTpSeg] = 0;
//                s_data.specialIndex [analyseData::dai_3rdSeg] = s_data.specialIndex [analyseData::dai_SXE] + 1;

                switch ( checkTPAMode (sel) )
                {
                    case tpanalysis::cm_Point_Point:
                    case tpanalysis::cm_Point_Movement:
                    case tpanalysis::cm_Point_Geometry:
                    case tpanalysis::cm_Movement_Movement:
                    case tpanalysis::cm_Movement_Geometry:
                    {
                        break;
                    }
                    case tpanalysis::cm_Toolpath:
                    {
                        pEnts1->MoveToBegin ();
                        SharedPtr<hcntcore::CadEntity> pEnt ( pEnts1->Get () );

                        ToolpathPtr pToolpath;

                        pToolpath = pEnt.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );

                        if ( pToolpath.Get () ) // selected an Toolpath
                        {
                            // todo 
                            // 1. getdocument
                            // cast to hcntdoc ?? class omHCNTDocImp : public omIBaseDocument1
                            // find cadshell object
                            // get job info
                            // aks HM document


                            // make no sense in case of Toolpath, no collection
                            CadEntitiesIt_T ents ( pToolpath->GetCadEntities () );

                            if ( !ents.Get () )
                                return;
                            for ( ents->MoveToBegin (); !ents->IsAtEnd (); ents->Next () )
                            {
                                omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );
                                pToolpathSegment = CastToToolpathSegment ( cep, sel );
                                if ( !pToolpathSegment.Get () )
                                    continue;
                                fillToolpathPoints ( pToolpathSegment, sel );
                            }
                        }
                        break;
                    }
                    case tpanalysis::cm_Geometry:
                    {
                        s_data.m [sel].specialIndex [analyseData::dai_SXP1] = UNSPECIFICINDEX;
                        break;
                    }
                    case tpanalysis::cm_Point:
                    case tpanalysis::cm_Movement:

                    {
                        pEnts1->MoveToBegin ();
                        SharedPtr<hcntcore::CadEntity> pEnt ( pEnts1->Get () );

                        pToolpathSegment = CastToToolpathSegment ( pEnt, sel );

                        if ( pToolpathSegment.Get () )
                        {
                            fillToolpathPoints ( pToolpathSegment, sel );
                        }

                        break;
                    }
                    default :
                    {
                        s_data.m[sel].typeVec.push_back ( analyseData::dai_tUnknown );
                    }
                }
            }

            int TPAnalysisModel::OnSetupCoordinateSystem ( const int& UI_BtnGroup_RefSystemType )
            {
                tpanalysis::Motion_T pGraphicsRefSys;

                SetValue ( tpanalysis::WorkplaneEnabled, false );
                SetValue ( tpanalysis::TGr_ReferenceSystem, pGraphicsRefSys );


                if ( !m_pDocument || !m_pDocument.Get () )
                {
                    return 0;
                }

                int sel_cs = 2;
                // get the current reference system
                if ( s_data.pSelectedTP [MASTER_TP].Get () )
                {
                    sel_cs = UI_BtnGroup_RefSystemType;
                }

                switch ( sel_cs )
                {
                    case cs_NCS:
                    {
                        s_data.m[MASTER_TP].motion = s_data.m[MASTER_TP].pof_ncs.Inverse ();
                        break;
                    }
                    case cs_Frame: // default
                    {
                        s_data.m[MASTER_TP].motion = s_data.m[MASTER_TP].pof_frame.Inverse ();
                        break;
                    }
                    case cs_World: // default
                    {
                        hcntcore::Model* model = ( hcntcore::Model* )m_pDocument->Cast ( L"Model" );
                        SharedPtr<hcntcore::workplane::ReferenceSystem> rsys ( model->GetReferenceSystemMgr ()->GetCurrentReferenceSystem () );
                        s_data.m[MASTER_TP].motion = rsys->GetMotion ().Inverse ();
                        break;
                    }
                    default:
                    {
                        s_data.m[MASTER_TP].motion = omsl::bgeo::RigidMotion3d ();
                        break;
                    }
                }
                

                SetValue ( tpanalysis::TGr_ReferenceSystem, s_data.m[MASTER_TP].motion.Inverse () );
                if ( sel_cs != cs_World )
                    SetValue ( tpanalysis::WorkplaneEnabled, true );
                else
                    SetValue ( tpanalysis::WorkplaneEnabled, false );

                // get the current reference system
                if ( !s_data.pSelectedTP [MASTER_TP].Get () )
                {
                    return 0;
                }

                if ( m_withUpdate )
                    OnChangeMoveNr2 ( s_data.m[MASTER_TP].specialIndex [analyseData::dai_SXP1] );

                return 1;
            }

            int TPAnalysisModel::OnChangeReferencePoint ( const SharedPtr<hcntcore::CadEntity> &pPoint )
            {
                StoreRefPoint ( pPoint, analyseData::dai_REFP, MASTER_TP );
                SetupPropertyBag ();

                return 1;
            }

        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
