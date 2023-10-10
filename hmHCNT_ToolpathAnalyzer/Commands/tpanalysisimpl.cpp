#include "stdafx.h"
#include "TPAnalysisImpl.h"
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
#include "tpanalysiskeys.h"

#include "graphics\view.h"
#include "modelgraphic\graphicmodelviewer.h"
#include <hcntcore/toolpathentity.h>
#include <gui/gui.h>
#include <gui/guihandshake.h>
#include "tpanalysepropertiesimpl.h"
#include <command/commandpolicyt.h>
#include <string.h>

using namespace omsl::command;

namespace omsl {
namespace commands {
namespace tp_analyzer {

    static bool s_withView = false;
    static double s_topRed = 0.,
        s_topGreen = 0.,
        s_topBlue = 0.,
        s_bottomRed = 0.,
        s_bottomGreen = 0.,
        s_bottomBlue = 0.;

    bool s_checkColors ( SharedPtr<graphics::View> pView )
    {
        if ( pView && s_withView == true )
        {
            double topRed,
                topGreen,
                topBlue,
                bottomRed,
                bottomGreen,
                bottomBlue;
            /*bool withView = */pView->GetBackGroundRGBColor ( topRed, topGreen, topBlue, bottomRed, bottomGreen, bottomBlue ) ? false : true;
            double d1 = 30.0 / 255.0;
            double d2 = 188.0 / 255.0;
            if (    fabs ( d1 - topRed ) > 0.0001
                 || fabs ( d1 - topGreen ) > 0.0001
                 || fabs ( d1 - topBlue ) > 0.0001
                 || fabs ( d2 - bottomRed ) > 0.0001
                 || fabs ( d2 - bottomGreen ) > 0.0001
                 || fabs ( d2 - bottomBlue ) > 0.0001
                 )
                return false; // don´t restore wrong colors;
            return true;
        }
        return false;
    }
    class MyPolicy :
        public omsl::command::CommandPolicyT<MyPolicy>
    {
    public:
        MyPolicy ( omsl::command::Command* command ) : omsl::command::CommandPolicyT<MyPolicy> ( command )
        {}
        ~MyPolicy ( void )
        {}

    protected:
        OMSL_METHOD ( void ) InternalGetActions ( const omsl::command::Command* rCommand, const omsl::command::Command* iCommand, omsl::command::CommandPolicy::Action& rAction, omsl::command::CommandPolicy::Action& iAction )
        {
            if ( GetCommand () )
            {
                if ( rCommand == GetCommand () )
                {
                    rAction = omsl::command::CommandPolicy::kStay;
                    if ( !wcscmp ( iCommand->GetName ( omsl::command::Command::kFullName ), GetCommand ()->GetName ( omsl::command::Command::kFullName ) ) )
                        iAction = omsl::command::CommandPolicy::kCancel;
                    else
                        iAction = omsl::command::CommandPolicy::kRun;
                }
                else
                {
                    if ( !wcscmp ( rCommand->GetName ( omsl::command::Command::kFullName ), GetCommand ()->GetName ( omsl::command::Command::kFullName ) ) )
                    {
                        rAction = omsl::command::CommandPolicy::kStay;
                        iAction = omsl::command::CommandPolicy::kCancel;
                    }
                }
            }
            return;
        }
    };


OMSL_COMMAND_FACTORY_NAMED(tp_analyzer, TPAnalysis )
{
  return new TPAnalysisImpl(pGUI, pApplication, pDocument);
}
TPAnalysisImpl::TPAnalysisImpl(void* pGUI, omsl::hcntcore::Application* pApplication, hcntcore::Document* pDocument)
  : CommandImpl(pApplication)
{
  m_pDocument = pDocument;
  m_pRootGUI = pGUI;


  if ( pDocument ) {
    m_pModel = CreateModel ( OmslGetDllName(), L"TPAnalysis", pDocument );
    m_pCommandGUI = CreateGUI(OmslGetDllName(), pGUI, L"TPAnalysis", omsl::LockPtr<TPAnalysisImpl>( this ).Get( ), pDocument );

//    m_pCommandGUI = gui::automatic::CreateObject(pGUI, omsl::LockPtr<TPAnalysisImpl>(this).Get(), pDocument );
    m_pPreview = previews::automatic::CreateObject(pDocument);
  }  

  m_policy = new MyPolicy ( this );
}


OMSL_IMPL( propertyservice::Context* ) TPAnalysisImpl::GetContext( )
{
  return m_pModel ? m_pModel->GetContext() : NULL;
}

omsl::Bool TPAnalysisImpl::CanRun( int , void* [] )
{
  if ( m_pModel ) 
  {
    if (!m_pModel->Initialize())
      return omsl::False;
  }
  else
    return omsl::False;
return omsl::True;
}

OMSL_IMPL ( void ) TPAnalysisImpl::InternalRun ( int , void*  [] )
{
  //TODO
    omsl::commands::tp_analyzer::Properties* tpp = GetProperties ();
    if ( tpp )
    {

        omsl::commands::tp_analyzer::PropertiesImpl* p = dynamic_cast < omsl::commands::tp_analyzer::PropertiesImpl* > ( tpp );
        if ( p )
            p->SetModel ( (TPAnalysisModel*) m_pModel.Get () );

        SharedPtr<propertyservice::PropertyBag> pBagPtr ( tpp->GetBag () );
        if ( !pBagPtr->IsEmpty () )
        {
            m_pModel->GetContext ()->SetValue ( tpanalysis::UI_Settings, pBagPtr );
        }
    }

    omsl::propertyservice::Context* pContext = m_pModel->GetContext ();
    if ( pContext )
    {
    }



    if( m_pCommandGUI )
      if( !m_pCommandGUI->Initialize() )
        return;
    if( m_pPreview )
      if( !previews::utils::InitPreview(m_pModel->GetContext(),m_pPreview, m_pRootGUI) )
        return;

    if ( m_pDocument )
    {
        WeakPtr<modelgraphic::GraphicModelViewer> pViewer ( m_pDocument->GetGraphicModelViewer () );
        if ( pViewer )
        {
            WeakPtr<graphics::View> pView ( pViewer->GetView () );
            if ( pView )
            {
                s_withView = pView->GetBackGroundRGBColor ( s_topRed, s_topGreen, s_topBlue, s_bottomRed, s_bottomGreen, s_bottomBlue ) ? false : true;

                double d2 = 188.0 / 255.0;
                double d1 = 30.0 / 255.0;
                s_withView = pView->SetBackGroundRGBColor ( d1, d1, d1, d2, d2, d2 ) ? false : true;
            }
        }
    }
  return;
}

OMSL_IMPL ( void ) TPAnalysisImpl::InternalExit ()
{
    //TODO
    if ( s_withView )
    {
        if ( m_pDocument )
        {
            WeakPtr<modelgraphic::GraphicModelViewer> pViewer ( m_pDocument->GetGraphicModelViewer () );
            if ( pViewer )
            {
                WeakPtr<graphics::View> pView ( pViewer->GetView () );
                if ( pView && s_checkColors ( pView ) )
                {
                    s_withView = pView->SetBackGroundRGBColor ( s_topRed, s_topGreen, s_topBlue, s_bottomRed, s_bottomGreen, s_bottomBlue ) ? false : true;
                }
            }
        }
    }
    return;
}

OMSL_IMPL(void) TPAnalysisImpl::InternalApply(bool resetAfterApply)
{
  //TODO
  if( m_pModel )
      m_pModel->Apply(resetAfterApply);
  return;
}

OMSL_IMPL ( omsl::command::CommandPolicy* ) TPAnalysisImpl::GetPolicy () const
{
    return m_policy.get ();
}

}  // namespace tp_analyzer
}  // namespace commands
}  // namespace omsl
