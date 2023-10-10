#pragma warning( disable : 4996 ) // for warnings: due to compare of boost::uuids
#include "TPAnalysisModel.h"
// keys
#include <Commands\tpanalysiskeys.h>

#include <hcntcore/pargeomentity.h>
#include <hcntcore/point.h>
#include <geom/arc3d.h>
#include <hcntcore/cadentitymgr.h>
#include <hcntcore/cadentityiterators.h>
#include <hcntcore/polylineithpoint.h>
#include <hcntcore/toolpathmovement.h>

using namespace omsl::command;

typedef omsl::SharedPtr<omsl::hcntcore::GeomEntity> GeomEntityPtr;
typedef omsl::SharedPtr<omsl::hcntcore::ToolPathEntity> ToolpathPtr;
typedef omsl::SharedPtr<omsl::geom::Geom3d> Geom3dPtr;

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {
            void TPAnalysisModel::StoreEntityPoint ( const CadEntitiesIt_T& pEnts, const unsigned int index, int sel )
            {
                if ( pEnts )
                {
                    pEnts->MoveToBegin ();
                    SharedPtr<hcntcore::CadEntity> pEnt ( pEnts->Get () );

                    StoreRefPoint ( pEnt, index, sel);
                    StoreArcData ( pEnt, index +analyseData::dai_ArcOffset,sel );
                    StoreArcData ( pEnt, index +analyseData::dai_ArcOffset+ analyseData::dai_ArcOffset,sel ); // necessary for current tool direction
                }
                else
                {
                    s_data.m[sel].points [index] [0] = s_data.m[sel].points [index] [1] = s_data.m[sel].points [index] [2] = 0.0;
                }
                {
                    return;
                }
            }

            int TPAnalysisModel::calcArcVector ( const omsl::geom::Arc3d* pArc, bgeo::Point3d& center, bgeo::UnitVector3d& v0, bgeo::UnitVector3d& v2 )
            {
                int ok = 0;
                if ( pArc == nullptr )
                    return -1;

                center = pArc->Center ();
                bgeo::Point3d e1 = pArc->End1 ();
                bgeo::Point3d e0 = pArc->End0 ();
                omsl::bgeo::UnitVector3d  v1;
                center.DirectionTo ( e0, v0 );
                center.DirectionTo ( e1, v1 );
                v0.Angle ( v1 );

                // create matrix based on arc plane
                omsl::bgeo::UnitVector3d nz ( pArc->PlaneNormal () );
                omsl::bgeo::UnitVector3d nx;
                omsl::bgeo::UnitVector3d ny;
                nz.BuildXYZVectors ( nx, ny );

                omsl::bgeo::RigidMotion3d arcplane ( center, nx, ny, nz );
                omsl::bgeo::RigidMotion3d arcorig ( arcplane );
                // move vector ceneter - startpoint ( radius ) to xy-plane ??
                arcplane = arcorig.Inverse ();

#ifdef DBGVER
                wchar_t vectorstring [256];

                callStackString ( L"vectors", 0 );
                swprintf ( vectorstring, L"nx : x = x=%8.4lf y=%8.4lf z=%8.4lf ", nx [0], nx [1], nx [2] );
                callStackString ( vectorstring, 0 );
                swprintf ( vectorstring, L"ny : x = x=%8.4lf y=%8.4lf z=%8.4lf ", ny [0], ny [1], ny [2] );
                callStackString ( vectorstring, 0 );
                swprintf ( vectorstring, L"nz : x = x=%8.4lf y=%8.4lf z=%8.4lf ", nz [0], nz [1], nz [2] );
                callStackString ( vectorstring, 0 );
                swprintf ( vectorstring, L"v0 : x=%8.4lf y=%8.4lf z=%8.4lf ", v0 [0], v0 [1], v0 [2] );
                callStackString ( vectorstring, 0 );
                swprintf ( vectorstring, L"v1 : x=%8.4lf y=%8.4lf z=%8.4lf ", v1 [0], v1 [1], v1 [2] );
                callStackString ( vectorstring, 0 );

                callStackString ( L"arcplaneinv", 0 );
                swprintf ( vectorstring, L"ap = x=%8.4lf y=%8.4lf z=%8.4lf ", arcplane.XUnitVector () [0], arcplane.XUnitVector () [1], arcplane.XUnitVector () [2] );
                callStackString ( vectorstring, 0 );
                swprintf ( vectorstring, L"ap = x=%8.4lf y=%8.4lf z=%8.4lf ", arcplane.YUnitVector () [0], arcplane.YUnitVector () [1], arcplane.YUnitVector () [2] );
                callStackString ( vectorstring, 0 );
                swprintf ( vectorstring, L"ap = x=%8.4lf y=%8.4lf z=%8.4lf ", arcplane.ZUnitVector () [0], arcplane.ZUnitVector () [1], arcplane.ZUnitVector () [2] );
                callStackString ( vectorstring, 0 );

                callStackString ( L"arcorig", 0 );
                swprintf ( vectorstring, L"ao = x=%8.4lf y=%8.4lf z=%8.4lf ", arcorig.XUnitVector () [0], arcorig.XUnitVector () [1], arcorig.XUnitVector () [2] );
                callStackString ( vectorstring, 0 );
                swprintf ( vectorstring, L"ao = x=%8.4lf y=%8.4lf z=%8.4lf ", arcorig.YUnitVector () [0], arcorig.YUnitVector () [1], arcorig.YUnitVector () [2] );
                callStackString ( vectorstring, 0 );
                swprintf ( vectorstring, L"ao = x=%8.4lf y=%8.4lf z=%8.4lf ", arcorig.ZUnitVector () [0], arcorig.ZUnitVector () [1], arcorig.ZUnitVector () [2] );
                callStackString ( vectorstring, 0 );

                callStackString ( L"data.motion", 0 );
                swprintf ( vectorstring, L"mo = x=%8.4lf y=%8.4lf z=%8.4lf ", s_data.m[MASTER_TP].motion.XUnitVector () [0], s_data.m[MASTER_TP].motion.XUnitVector () [1], s_data.m[MASTER_TP].motion.XUnitVector () [2] );
                callStackString ( vectorstring, 0 );
                swprintf ( vectorstring, L"mo = x=%8.4lf y=%8.4lf z=%8.4lf ", s_data.m[MASTER_TP].motion.YUnitVector () [0], s_data.m[MASTER_TP].motion.YUnitVector () [1], s_data.m[MASTER_TP].motion.YUnitVector () [2] );
                callStackString ( vectorstring, 0 );
                swprintf ( vectorstring, L"mo = x=%8.4lf y=%8.4lf z=%8.4lf ", s_data.m[MASTER_TP].motion.ZUnitVector () [0], s_data.m[MASTER_TP].motion.ZUnitVector () [1], s_data.m[MASTER_TP].motion.ZUnitVector () [2] );
                callStackString ( vectorstring, 0 );



#endif



                arcplane.ApplyToUnitVector ( v1 );
#ifdef DBGVER
                swprintf ( vectorstring, L"v1i : x=%8.4lf y=%8.4lf z=%8.4lf ", v1 [0], v1 [1], v1 [2] );
                callStackString ( vectorstring, 0 );
#endif
                // rotate radius vector 90 degree cw
                omsl::bgeo::Vector2d v2d ( v1 [0], v1 [1] );
                v2d.Rotate90DegreesCcw ();
                v2.Set ( v2d [0], v2d [1], 0.0 );

#ifdef DBGVER
                swprintf ( vectorstring, L"v2i : x=%8.4lf y=%8.4lf z=%8.4lf ", v2 [0], v2 [1], v2 [2] );
                callStackString ( vectorstring, 0 );
#endif

                arcorig.ApplyToUnitVector ( v2 );

#ifdef DBGVER
                swprintf ( vectorstring, L"v2a : x=%8.4lf y=%8.4lf z=%8.4lf ", v2 [0], v2 [1], v2 [2] );
                callStackString ( vectorstring, 0 );
#endif
                // move vector back

                arcorig.ApplyToUnitVector ( v0 );

                return ok;
            }

            void TPAnalysisModel::StoreArcData ( SharedPtr<hcntcore::CadEntity> &pCadEntity, const unsigned int index, int sel )
            {
                switch ( index )
                {
                    case analyseData::dai_CTR1:
                    case analyseData::dai_CTR2:
                    {
                        ToolpathSegmentPtr pToolpathSegment ( CastToToolpathSegment ( pCadEntity,sel ) );

                        if ( pToolpathSegment.Get () )
                        {
                            omsl::ULong i = 0;
                            omsl::ULong size = pToolpathSegment->Size ();

                            // to avoid access to unknown type
                            if ( s_data.m[sel].specialIndex [analyseData::dai_SXP1] < size && s_data.m[sel].specialIndex [analyseData::dai_SXP1] > 0 )
                            { 
                                i = s_data.m[sel].specialIndex [analyseData::dai_SXP1]-1;
                            }
                            
                            {
//                                const bgeo::Coord3d* pC = pToolpathSegment->GetPoint ( i );
                                const omsl::geom::Curve3d* pc = pToolpathSegment->GetCurve ( i ); // s_data.m[sel].specialIndex [analyseData::dai_SXP1] );
                                if ( pc != nullptr )
                                {
                                    const omsl::geom::Arc3d* pArc = ( const omsl::geom::Arc3d* ) pc->Cast ( L"Arc3d" );
                                    if ( pArc != nullptr )
                                    {
                                        // check startpoint with other point )
                                        s_data.m[sel].points [index] = pArc->Center ();
                                        bgeo::Point3d e1 = pArc->End1 ();
                                        bgeo::Point3d e0 = pArc->End0 ();
                                        omsl::bgeo::UnitVector3d v0,v1;
                                        s_data.m[sel].points [index].DirectionTo ( e0, v0 );
                                        s_data.m[sel].points [index].DirectionTo ( e1, v1 );
                                        v0.Angle ( v1 );

                                        if ( index == analyseData::dai_CTR1 )
                                        {
                                            s_data.m[sel].vectors [analyseData::dai_VR] [analyseData::dai_SXS] = pArc->Radius ();
                                            s_data.m[sel].vectors [analyseData::dai_VR] [analyseData::dai_SXE] = pArc->Radius ();
                                            s_data.m[sel].vectors [analyseData::dai_VA] [analyseData::dai_SXS] = pArc->Parameter0 ();
                                            s_data.m[sel].vectors [analyseData::dai_VA] [analyseData::dai_SXE] = pArc->Parameter1 ();
                                        }
                                        else
                                        {

                                        }
                                    }
                                }
                            }
                        }

                        break;
                    }
                    case analyseData::dai_CTR3:
                    case analyseData::dai_CTR4:
                    {
                        ToolpathSegmentPtr pToolpathSegment ( CastToToolpathSegment ( pCadEntity, sel ) );

                        if ( pToolpathSegment.Get () )
                        {
                            omsl::ULong i = 0;
                            omsl::ULong size = pToolpathSegment->Size ();

                            // to avoid access to unknown type
                            if ( s_data.m[sel].specialIndex [analyseData::dai_SXP1] < size && s_data.m[sel].specialIndex [analyseData::dai_SXP1] > 0 )
                            {
                                i = s_data.m[sel].specialIndex [analyseData::dai_SXP1]-1;
                            }

                            {
                                //                                const bgeo::Coord3d* pC = pToolpathSegment->GetPoint ( i );
                                const omsl::geom::Curve3d* pc = pToolpathSegment->GetCurve ( i ); // s_data.m[sel].specialIndex [analyseData::dai_SXP1] );
                                if ( pc != nullptr )
                                {
                                    const omsl::geom::Arc3d* pArc = ( const omsl::geom::Arc3d* ) pc->Cast ( L"Arc3d" );
                                    omsl::bgeo::UnitVector3d v0, v1, v2;
                                    if ( calcArcVector (pArc, s_data.m[sel].points [index] ,v0,v2) == 0 )
                                    {
                                        if ( index == analyseData::dai_CTR3 )
                                        {
                                            s_data.m[sel].vectors [analyseData::dai_CTR3] = bgeo::Vector3d ( v0 );
                                            s_data.m[sel].vectors [analyseData::dai_CTR4] = bgeo::Vector3d ( v2 );
                                        }
                                    }
                                }
                            }
                        }

                        break;
                    }
                }
            }

            void TPAnalysisModel::StoreRefPoint ( const SharedPtr<hcntcore::CadEntity> &pEnt, const unsigned int index, int sel )
            {
                switch ( index )
                {
                    case analyseData::dai_REFP: s_data.m [sel].specialIndex [analyseData::dai_REFP] = 0; break;// to identify if the refpoint was set or not
                    case analyseData::dai_SXP1:
                    case analyseData::dai_SXP2:
                    {
                        if ( pEnt )
                        {
                            SharedPtr<const geom::Point3d > pPt ( hcntcore::CadEntityMgr::GetPoint ( pEnt.Get () ) );

                            if ( pPt )
                            {
                                pPt->GetCoord ( s_data.m [sel].points [analyseData::dai_SXP1] [0], s_data.m [sel].points [analyseData::dai_SXP1] [1], s_data.m [sel].points [analyseData::dai_SXP1] [2] );
                            }
                            else
                            {
                                s_data.m [sel].points [analyseData::dai_SXP1] = GetPoint ( pEnt, analyseData::dai_SXP1, sel );
                            }
                        }
                        else
                        {
                            s_data.m [sel].specialIndex [analyseData::dai_SXP1] = UNKNOWNINDEX; // to identify if a point was set or not
                            s_data.m [sel].points [analyseData::dai_SXP1] [0] = s_data.m [sel].points [analyseData::dai_SXP1] [1] = s_data.m [sel].points [analyseData::dai_SXP1] [2] = 0.0;
                        }
                    }
                }
            }

            bgeo::Point3d TPAnalysisModel::GetPoint ( const SharedPtr<hcntcore::CadEntity> &ent_0, const unsigned int index, int sel )
            {
                bgeo::Point3d pt0,pt1;
                try
                {
                    if ( ent_0.Get () == nullptr )
                        return pt0;

                    WeakPtr<hcntcore::ParGeomEntity> pg ( ( hcntcore::ParGeomEntity* ) ent_0->Cast ( L"ParGeomEntity" ) );
                    if ( pg )
                    {
                        WeakPtr<hcntcore::pargeom::Geom> pParGeom ( pg->GetParGeom () );
                        if ( pParGeom.IsValid () )
                        {
                            WeakPtr<hcntcore::pargeom::Point> pParPt ( ( hcntcore::pargeom::Point* ) pParGeom->Cast ( L"Point" ) );
                            
                            if ( pParPt.IsValid () )
                            {
                                pt0 = pParPt->GeometricPoint ();
                                if ( sel == R_SEL )
                                {
                                    s_data.m [sel].specialIndex [index] = -2;
                                    s_data.m [sel].points [index] = pt0;
                                }
                            }

                            WeakPtr<hcntcore::pargeom::PolylineIthPoint > pParIdx ( ( hcntcore::pargeom::PolylineIthPoint * ) pParGeom->Cast ( L"PolylineIthPoint" ) );
                            if ( pParIdx.IsValid () )
                            {
                                int idx = pParIdx->GetIndex (); // the point index!
                                s_data.m[sel].specialIndex [index] = idx;
                            }
                            else
                            {
                                WeakPtr<hcntcore::pargeom::ToolpathMovement > pTpMove ( ( hcntcore::pargeom::ToolpathMovement * ) pParGeom->Cast ( L"ToolpathMovement" ) );
                                if ( pTpMove.IsValid () )
                                {
                                    int idx = (int)pTpMove->GetIndex (); // the point index!
                                    s_data.m[sel].specialIndex [index] = idx+1;
                                    const geom::Curve3d* pCurve = pTpMove->GeomCurve ();
                                    if ( pCurve )
                                    {
                                        const omsl::geom::Arc3d* pArc = ( const omsl::geom::Arc3d* ) pCurve->Cast ( L"Arc3d" );
                                        if ( pArc != nullptr )
                                        {
                                            pt0 = pArc->Center ();
                                            pt0 = pArc->End0 ();
                                            pt1 = pArc->End1 ();
                                        }
                                        else
                                        {
                                            pt0 = pCurve->End0 ();
                                            pt1 = pCurve->End1 ();
                                        }
                                        s_data.m[sel].points [analyseData::dai_SXS] = pt0;
                                        s_data.m[sel].points [analyseData::dai_SXE] = pt1;
                                    }
                                }
                                else
                                {
                                    WeakPtr<hcntcore::pargeom::Curve > ppCurve ( ( hcntcore::pargeom::Curve * ) pParGeom->Cast ( L"Curve" ) );
                                    if ( ppCurve.IsValid () )
                                    {
                                        const geom::Curve3d* pCurve = ppCurve->GeomCurve ();
                                        if ( pCurve )
                                        {
                                            const omsl::geom::Arc3d* pArc = ( const omsl::geom::Arc3d* ) pCurve->Cast ( L"Arc3d" );
                                            if ( pArc != nullptr )
                                            {
                                                pt0 = pArc->Center ();
                                                pt0 = pArc->End0 ();
                                                pt1 = pArc->End1 ();
                                            }
                                            else
                                            {
                                                pt0 = pCurve->End0 ();
                                                pt1 = pCurve->End1 ();
                                            }
                                            s_data.m [sel].points [analyseData::dai_SXS] = pt0;
                                            s_data.m [sel].points [analyseData::dai_SXE] = pt1;
                                            s_data.m [sel].specialIndex [index] = -2;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                catch ( ... )
                {
                }

                return pt0;
            }
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl