#include "stdafx.h"
#include "TP_SpeedAnalysisImpl.h"
#include <propertyservice/propertybag/propertytypedbag.h>
// keys
//#include "../include/tpanalysiskeys.h"
//document
#include <hcntcore/document.h>
// automatic GUI
#include <lockptr.h>
#include <command/automaticcommandgui.h>
// automatic Preview
#include <command/automaticcommandpreview.h>
#include <propertyservice/context/context.h>
#include "commandhandshake.h"
#include "tp_speedanalysiskeys.h"

#include "graphics\view.h"
#include "modelgraphic\graphicmodelviewer.h"
#include <hcntcore/toolpathentity.h>

using namespace omsl::command;

namespace omsl {
namespace commands {
namespace tp_analyzer {

    static bool withView = false;
    static double topRed,
        topGreen,
        topBlue,
        bottomRed,
        bottomGreen,
        bottomBlue;


OMSL_COMMAND_FACTORY_NAMED ( tp_analyzer, TPSpeedAnalysis )
{
    return new TPSpeedAnalysisImpl ( pGUI, pApplication, pDocument );
}

TPSpeedAnalysisImpl::TPSpeedAnalysisImpl ( void* pGUI, omsl::hcntcore::Application* pApplication, hcntcore::Document* pDocument )
    : CommandImpl ( pApplication )
{
    m_pDocument = pDocument;
    m_pRootGUI = pGUI;

    if ( pDocument )
    {
        m_pModel = CreateModel ( OmslGetDllName (), L"TPSpeedAnalysis", pDocument );
        m_pCommandGUI = CreateGUI ( OmslGetDllName (), pGUI, L"TPSpeedAnalysis", omsl::LockPtr<TPSpeedAnalysisImpl> ( this ).Get (), pDocument );
        m_pPreview = previews::automatic::CreateObject ( pDocument );
    }
}


OMSL_IMPL ( propertyservice::Context* ) TPSpeedAnalysisImpl::GetContext ()
{
    return m_pModel ? m_pModel->GetContext () : NULL;
}

omsl::Bool TPSpeedAnalysisImpl::CanRun ( int, void* [] )
{
    if ( m_pModel )
    {
        if ( !m_pModel->Initialize () )
            return omsl::False;
    }
    else
        return omsl::False;
    return omsl::True;
}

OMSL_IMPL ( void ) TPSpeedAnalysisImpl::InternalRun ( int, void* [] )
{
    omsl::propertyservice::Context* pContext = m_pModel->GetContext ();
    if ( pContext )
    {
    }

    if ( m_pCommandGUI )
        if ( !m_pCommandGUI->Initialize () )
            return;
    if ( m_pPreview )
        if ( !previews::utils::InitPreview ( m_pModel->GetContext (), m_pPreview, m_pRootGUI ) )
            return;
    //TODO


    if ( m_pDocument )
    {
        WeakPtr<modelgraphic::GraphicModelViewer> pViewer ( m_pDocument->GetGraphicModelViewer () );
        if ( pViewer )
        {
            WeakPtr<graphics::View> pView ( pViewer->GetView () );
            if ( pView )
            {
                withView = pView->GetBackGroundRGBColor ( topRed, topGreen, topBlue, bottomRed, bottomGreen, bottomBlue ) ? false : true;
                if ( pView )
                {
                    withView = pView->SetBackGroundRGBColor ( 188.0 / 255.0, 188.0 / 255.0, 188.0 / 255.0 , 30.0 / 255.0, 30.0 / 255.0, 30.0 / 255.0 ) ? false : true;

                }
            }
        }
    }
    return;
}

OMSL_IMPL ( void ) TPSpeedAnalysisImpl::InternalExit ()
{
    //TODO
    if ( withView )
    {
        if ( m_pDocument )
        {
            WeakPtr<modelgraphic::GraphicModelViewer> pViewer ( m_pDocument->GetGraphicModelViewer () );
            if ( pViewer )
            {
                WeakPtr<graphics::View> pView ( pViewer->GetView () );
                if ( pView )
                {
                    withView = pView->SetBackGroundRGBColor ( topRed, topGreen, topBlue, bottomRed, bottomGreen, bottomBlue ) ? false : true;
                }
            }
        }
    }
    return;
}

OMSL_IMPL ( void ) TPSpeedAnalysisImpl::InternalApply ()
{
    //TODO
    if ( m_pModel )
        m_pModel->Apply ();
    return;
}


}  // namespace tp_analyzer
}  // namespace commands
}  // namespace omsl
