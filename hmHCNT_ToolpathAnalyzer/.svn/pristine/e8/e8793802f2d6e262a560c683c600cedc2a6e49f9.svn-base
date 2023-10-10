#include "stdafx.h"
#include "TPAnalysisImpl.h"
#include "tp_simpleCommands.h"
//document
#include <hcntcore/document.h>
#include <hcntcore/toolpathentity.h>
#include <geom/attributes.h>
#include <hcntcore/cadentityiterators.h>
#include <graphicattributes/toolpathgraphicattributes.h>
#include <graphicattributes/toolpathsegmentgraphicattributes.h>
#include <graphicattributes/pointcloudgraphicattributes.h>
#include <geom/toolpathsegment.h>
#include <string.h>

#include "selectionservice/entityselector.h"
#include "selectionservice/interactiveselector.h"
#include "selectionservice/selectionservice.h"

using namespace omsl::command;

namespace omsl
{
namespace commands
{
namespace tp_analyzer
{
    typedef omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> CadEntityForwardPtr;
    typedef omsl::SharedPtr<omsl::graphicattributes::GraphicAttributes> GraphicAttributesPtr;
    typedef omsl::SharedPtr<omsl::graphicattributes::ToolpathGraphicAttributes> ToolpathGraphicAttributesPtr;
    typedef omsl::SharedPtr<omsl::graphicattributes::ToolpathSegmentGraphicAttributes> ToolpathSegmentGraphicAttributesPtr;
    typedef omsl::SharedPtr<omsl::graphicattributes::PointCloudGraphicAttributes> PointCloudGraphicAttributesPtr;
    typedef omsl::SharedPtr<omsl::geom::TopoGeom> TopoGeomPtr;
    typedef omsl::SharedPtr<omsl::geom::ToolpathSegment> ToolpathSegmentPtr;
    typedef omsl::SharedPtr<omsl::hcntcore::ToolPathEntity> ToolpathPtr;


    int S_showG1
    (
        ToolpathPtr& /*pToolpath*/
    ,   bool /*vis*/
    )
    {
        return 0;
    }

    int S_showG0
    (
        ToolpathPtr& /*pToolpath*/
        , bool /*vis*/
    )
    {
        return 0;
    }

    int S_showGV
    (
        ToolpathPtr& /*pToolpath*/
        , bool /*vis*/
    )
    {
        return 0;
    }

    int S_showGA
    (
        ToolpathPtr& /*pToolpath*/
        , bool /*vis*/
    )
    {
        return 0;
    }

    int S_showPt
    (
        ToolpathPtr& /*pToolpath*/
        , bool /*vis*/
    )
    {
        return 0;
    }

    int S_processToolpathFunction
    (
        SharedPtr<hcntcore::Model>& pModel
    ,   int type
    )
    {
        omsl::SharedPtr<omsl::selectionservice::EntitySelector> m_pSelector;
        m_pSelector = omsl::selectionservice::EntitySelector::Create ( pModel.Get () );
        
        omsl::selectionservice::Selection* pSelection = m_pSelector->SelectAll ();

        CadEntityForwardPtr ent_it ( pSelection->GetEntities () );
        for ( OMSL_EACH ( ent_it ) )
        {
            SharedPtr<hcntcore::CadEntity> pEnt ( ent_it->Get () );
            if ( !pEnt.Get () )
                continue;
            ToolpathPtr pToolpath;
            pToolpath = pEnt.shared_cast< omsl::hcntcore::ToolPathEntity >( L"ToolPathEntity" );
            if ( !pToolpath.Get () )
                continue;

            if ( type == tpv_Hide )
            {
                pToolpath->SetHidden ( false );
                continue;
            }

            if ( type == tpv_ShowAll )
            {
                pToolpath->SetHidden ( false );
                continue;
            }

            GraphicAttributesPtr gattrp1 ( pToolpath->GetGraphicAttributes () );
            if ( !gattrp1.Get () )
                continue;
            ToolpathGraphicAttributesPtr tggattrp1 = gattrp1.shared_cast< omsl::graphicattributes::ToolpathGraphicAttributes >( L"ToolpathGraphicAttributes" );
            if ( !tggattrp1.Get () )
                continue;

            omsl::SharedPtr<omsl::iterator::hcntcore::CadEntityForward> ents ( pToolpath->GetCadEntities () );
            if ( !ents.Get () )
                continue;


            omsl::graphicattributes::Status stat;
            stat = omsl::graphicattributes::OK;

            for ( OMSL_EACH ( ents ) )
            {
                omsl::SharedPtr<omsl::hcntcore::CadEntity> cep ( ents->Get () );
                if ( !cep.Get () )
                    continue;

                GraphicAttributesPtr gattrp ( cep->GetGraphicAttributes () );
                if ( !gattrp.Get () )
                    continue;

                ToolpathSegmentGraphicAttributesPtr tggattrp = gattrp.shared_cast<omsl::graphicattributes::ToolpathSegmentGraphicAttributes>( L"ToolpathSegmentGraphicAttributes" );
                if ( !tggattrp.Get () )
                    continue;

                //omsl::materials::ColorsPalette* p = tggattrp->GetColorsPalette ();

                if ( (type&tpv_Normal) == tpv_Normal )
                {
                    if ( tggattrp->GetShowNormals () == omsl::False )
                        stat = tggattrp->SetShowNormals ( omsl::True );
                    else
                        stat = tggattrp->SetShowNormals ( omsl::False );
                }
                if ( ( type&tpv_Points ) == tpv_Points )
                {
                    if ( tggattrp->GetShowPoints () == omsl::False )
                        stat = tggattrp->SetShowPoints ( omsl::True ); 
                    else
                        stat = tggattrp->SetShowPoints ( omsl::False );  
                }
                if ( ( type&tpv_Vector ) == tpv_Vector )
                {
                    if ( tggattrp->GetShowNormals () == omsl::False )
                        stat = tggattrp->SetShowNormals ( omsl::True );
                    else
                        stat = tggattrp->SetShowNormals ( omsl::False );
                }
                if ( ( type&tpv_G0_only ) == tpv_G0_only )
                {
                    S_showG0 ( pToolpath, false ); break;
                }
                if ( ( type&tpv_G1_only ) == tpv_G1_only )
                {
                    S_showG1 ( pToolpath, false ); break;
                }
                if ( ( type&tpv_ShowAll ) == tpv_ShowAll )
                {
                    S_showGA ( pToolpath, false ); break;
                }
            }

            pToolpath->UpdateGraphicAttributes ( gattrp1.Get () );
        }

        return 0;
    }

OMSL_COMMAND_FACTORY_NAMED ( tp_analyzer, TP_Visibility )
{
    return new TP_VisibilityImpl ( pApplication, pDocument );
}

TP_VisibilityImpl::TP_VisibilityImpl ( omsl::hcntcore::Application* pApplication, omsl::hcntcore::Document* pDocument ) : CommandOneShotImpl ( pApplication )
{
    m_pDocument = pDocument;

    if ( m_pDocument )
    {
        m_pModel = ( hcntcore::Model* )m_pDocument->Cast ( L"Model" );
    }
}

OMSL_IMPL ( omsl::Bool ) TP_VisibilityImpl::CanRun ( int argc, void* argv [] )
{
    //TODO
    if ( m_pModel )
    {
        m_type = tpv_unknown;
        for ( int i = 0; i < argc ; i++ )
        {
            if ( argv [i] )
            {
                wchar_t* a = ( wchar_t* ) argv [i];
                if ( a && wcslen ( a ) > 3 )
                {
                    if (      wcscmp ( a, L"tpv_Hide" )    == 0 ) m_type |= tpv_Hide;
                    else if ( wcscmp ( a, L"tpv_ShowAll" ) == 0 ) m_type |= tpv_ShowAll;
                    else if ( wcscmp ( a, L"tpv_Points" )  == 0 ) m_type |= tpv_Points;
                    else if ( wcscmp ( a, L"tpv_Vector" )  == 0 ) m_type |= tpv_Vector;
                    else if ( wcscmp ( a, L"tpv_Normal" )  == 0 ) m_type |= tpv_Normal;
                    else if ( wcscmp ( a, L"tpv_G0_only" ) == 0 ) m_type |= tpv_G0_only;
                    else if ( wcscmp ( a, L"tpv_G1_only" ) == 0 ) m_type |= tpv_G1_only;
                }
            }
        }

        if ( m_type != tpv_unknown )
            return omsl::True;
    }
    return omsl::False;
}
OMSL_IMPL ( void ) TP_VisibilityImpl::InternalRun ( int /*argc*/, void* /*argv*/ [] )
{
}

OMSL_IMPL ( void ) TP_VisibilityImpl::InternalApply (bool /*resetAfterApply*/)
{
    S_processToolpathFunction ( m_pModel, m_type );
}

}  // namespace tp_analyzer
}  // namespace commands
}  // namespace omsl
