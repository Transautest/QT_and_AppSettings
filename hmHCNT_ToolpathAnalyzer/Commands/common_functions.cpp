#include "common_functions.h"

#include <hcntcore/geomentity.h>
#include <hcntcore/pargeomentity.h>
#include <hcntcore/polylineithpoint.h>
#include <hcntcore/toolpathmovement.h>
#include "vector"
#include "string.h"
#include <Commands\tp_signalemitter.h>
#include "../broadcast/hcntsignaltypes.h"

#include <geom/attributes.h>
#include <geom/attributerefcounter.h>
#include <..\..\..\INTERFACES\iHCNT\iTopoAttrib.h>

typedef omsl::SharedPtr<omsl::hcntcore::ToolPathEntity> ToolpathPtr;
typedef omsl::SharedPtr<omsl::hcntcore::CadEntity> CadEntityPtr;
typedef omsl::SharedPtr<omsl::hcntcore::GeomEntity> GeomEntityPtr; 
typedef omsl::SharedPtr<omsl::hcntcore::ParGeomEntity> ParGeomEntityPtr;
typedef omsl::SharedPtr<omsl::hcntcore::pargeom::Geom> ParGeomGeomPtr;
typedef omsl::SharedPtr<omsl::hcntcore::pargeom::ToolpathMovement> ParGeomTpMovementPtr;
typedef omsl::SharedPtr<omsl::geom::Geom3d> Geom3dPtr;
typedef omsl::SharedPtr<omsl::geom::TopoGeom> TopoGeomPtr;

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {

std::vector<std::wstring> s_calls;
int s_level = 0;

void clear_callStackString ()
{
    s_calls.clear ();
    s_level = 0;
}

void callStackString ( const wchar_t* s, int inout )
{
    if ( s )
    {
        wchar_t ls [200];
        wchar_t ll [] = L"                                                                                                                                                   ";
        if ( inout > 0 )
            s_level += inout;
        ll [s_level * 3] = L'\0';
        swprintf ( ls, L"%5.5d : ", s_level );
        if ( inout < 0 )
            s_level += inout;

        std::wstring cls;
        cls = ll;
        cls += ls;
        cls += s;
        s_calls.push_back ( cls );
    }
}

omsl::hcntcore::CadEntity* GetReferencedToolpath ( SharedPtr<hcntcore::CadEntity>& pEntsIn, bool withSignal )
{

    CadEntityPtr  cent0 ( pEntsIn.Get () );
    if ( !cent0.Get () )
        return nullptr;

    ToolpathPtr pTPEnt0 = cent0.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );

    if ( !pTPEnt0.Get () )
    {
        CadEntityPtr pSegmentEntity ( GetReferencedToolpathSegmentEntity ( pEntsIn, withSignal ) );
        if ( pSegmentEntity.Get () )
        {
            CadEntityPtr  cent1 ( pSegmentEntity->GetParent () );
            pTPEnt0 = cent1.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );
            if ( pTPEnt0.Get () )
                return cent1.Get ();
            else
                return nullptr;
        }
        else
            return nullptr;
    }

    return cent0.Get ();
}

omsl::geom::ToolpathSegment* GetReferencedToolpathSegment ( SharedPtr<hcntcore::CadEntity>& pEntsIn )
{
    CadEntityPtr pCadEntity = pEntsIn;

    if ( !pCadEntity.Get () )
        return nullptr;

    ToolpathSegmentPtr pSegment;

    GeomEntityPtr gentp = pEntsIn.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
    if ( gentp.Get () )
    {
        Geom3dPtr g3dp ( gentp->Geom () );
        if ( g3dp.Get () )
            pSegment = g3dp.shared_cast< omsl::geom::ToolpathSegment >( L"ToolpathSegment" );
    }

    if ( pSegment.Get () == nullptr )
    {
        SharedPtr<omsl::hcntcore::ParGeomEntity> pParGeom = pEntsIn.shared_cast< omsl::hcntcore::ParGeomEntity >( L"ParGeomEntity" );
        if ( pParGeom.Get () )
        {
            omsl::SharedPtr<omsl::hcntcore::pargeom::Geom> pGeom ( pParGeom->GetParGeom () );

            omsl::SharedPtr<omsl::hcntcore::pargeom::PolylineIthPoint> pgplipp ( pGeom.shared_cast< omsl::hcntcore::pargeom::PolylineIthPoint >( L"PolylineIthPoint" ) );
            if ( pgplipp.Get () )
            {
                pCadEntity = pgplipp->GetRefEntity ();
                if ( pCadEntity.Get () )
                {
                    GeomEntityPtr gentp2 = pCadEntity.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                    if ( gentp2.Get () )
                    {
                        Geom3dPtr g3dp2 ( gentp2->Geom () );
                        if ( g3dp2.Get () )
                        {
                            pSegment = g3dp2.shared_cast< omsl::geom::ToolpathSegment >( L"ToolpathSegment" );
                        }
                    }
                }
            }
            else
            {
                ParGeomGeomPtr pggp ( pParGeom->GetParGeom () );
                ParGeomTpMovementPtr pTpMove = pggp.shared_cast< omsl::hcntcore::pargeom::ToolpathMovement >( L"ToolpathMovement" );
                if ( pTpMove.Get () )
                {
                    pCadEntity = pTpMove->GetRefEntity ();

                    gentp = pCadEntity.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                    if ( gentp.Get () )
                    {
                        Geom3dPtr g3dp ( gentp->Geom () );
                        if ( g3dp.Get () )
                            pSegment = g3dp.shared_cast< omsl::geom::ToolpathSegment >( L"ToolpathSegment" );
                    }
                }
            }
        }
    }

    if ( !pSegment.Get () )
        pSegment = ToolpathSegmentPtr ();

    return pSegment.Get ();
}

omsl::hcntcore::CadEntity* GetReferencedToolpathSegmentEntity ( SharedPtr<hcntcore::CadEntity>& pEntsIn, bool withSignal )
{
    CadEntityPtr pCadEntity = pEntsIn;

    if ( !pCadEntity.Get () )
        return nullptr;

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
            omsl::SharedPtr<omsl::hcntcore::pargeom::Geom> pGeom ( pParGeom->GetParGeom () );

            omsl::SharedPtr<omsl::hcntcore::pargeom::PolylineIthPoint> pgplipp ( pGeom.shared_cast< omsl::hcntcore::pargeom::PolylineIthPoint >( L"PolylineIthPoint" ) );
            if ( pgplipp.Get () )
            {
/////// point to create the Tooltip ????
                if ( withSignal == true )
                {
                    int idx = pgplipp->GetIndex (); // the point index!
                    omsl::hmbroadcast::EmitTPSignal ( L"TPCM_ShowIndexToolTip", omsl::hmbroadcast::bcs_TPCM_ShowIndexToolTip, pEntsIn.Get (), idx, 0.0 );
                }

                pCadEntity = pgplipp->GetRefEntity ();
                if ( pCadEntity.Get () )
                {
                    GeomEntityPtr gentp2 = pCadEntity.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                    if ( gentp2.Get () )
                    {
                        Geom3dPtr g3dp2 ( gentp2->Geom () );
                        if ( g3dp2.Get () )
                        {
                            pSegment = g3dp2.shared_cast< omsl::geom::ToolpathSegment >( L"ToolpathSegment" );
                        }
                    }
                }
            }
            else
            {
                ParGeomGeomPtr pggp ( pParGeom->GetParGeom () );
                ParGeomTpMovementPtr pTpMove = pggp.shared_cast< omsl::hcntcore::pargeom::ToolpathMovement >( L"ToolpathMovement" );
                if ( pTpMove.Get () )
                {
                    /////// point to create the Tooltip ????
                    if ( withSignal == true )
                    {
                        int idx = (int)pTpMove->GetIndex ()+1; // the movement index! point index is 1 more
                        omsl::hmbroadcast::EmitTPSignal ( L"TPCM_ShowIndexToolTip", omsl::hmbroadcast::bcs_TPCM_ShowIndexToolTip, pEntsIn.Get (), idx, 0.0 );
                    }

                    pCadEntity = pTpMove->GetRefEntity ();

                    gentp = pCadEntity.shared_cast< omsl::hcntcore::GeomEntity >( L"GeomEntity" );
                    if ( gentp.Get () )
                    {
                        Geom3dPtr g3dp ( gentp->Geom () );
                        if ( g3dp.Get () )
                            pSegment = g3dp.shared_cast< omsl::geom::ToolpathSegment >( L"ToolpathSegment" );
                    }
                }
            }
        }
    }

    if ( !pSegment.Get () )
        pCadEntity = CadEntityPtr ();

    return pCadEntity.Get ();
}

ToolpathData* GetToolpathData ( SharedPtr<omsl::geom::Attribute> pAttr )
{
    if ( pAttr.Get () )
    {
        omsl::geom::AttributeRefCounter* pAttrRef = ( omsl::geom::AttributeRefCounter* ) pAttr->Cast ( L"AttributeRefCounter" );
        if ( pAttrRef )
        {
            const RefCounter * pRef = pAttrRef->GetValue ();
            if ( pRef )
            {
                //                                    const omPofTechData* data = nullptr;
                ToolpathData* ptpData = ( ToolpathData * ) pRef->Cast ( L"ToolpathData" );
                return ptpData;
            }
        }
    }

    return nullptr;
}

ToolpathData* GetToolpathData ( ToolpathSegmentPtr pSegment )
{
    TopoGeomPtr pTopo = pSegment.shared_cast< omsl::geom::TopoGeom >( L"TopoGeom" );
    if ( pTopo && pTopo->GetAttribs () )
    {
        SharedPtr<omsl::geom::Attribute> pAttr ( pTopo->GetAttribs ()->Get ( L"HM_ToolpathData" ) );

        return GetToolpathData ( pAttr );
    }
    return nullptr;
}

        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
