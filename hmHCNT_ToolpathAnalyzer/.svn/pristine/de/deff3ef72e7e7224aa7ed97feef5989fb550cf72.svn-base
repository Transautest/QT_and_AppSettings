#include <propertyservice/propertybag/propertytypedbag.h>
#include "tp_speedanalysisgui.h"
// keys
#include <Commands\tp_speedanalysiskeys.h>
// command context
#include <propertyservice/context/context.h>
#include <command/command.h>
// Document
#include <hcntcore\document.h>
#include <guitools\automaticguibinding.h>
#include <intool\intoolsmanager.h>
#include <intool\selector.h>
#include <selectionservice/filter.h>
#include <selectionservice/filters.h>
#include <selectionservice/entityfilter.h>
#include <selectionservice/interactiveselector.h>
#include <selectionservice/selectionservice.h>
#include <selectionservice/snap.h>
#include <selectionservice/entityfilterutilities.h>
#include <gui\guihandshake.h>
#include <hcntcore/application.h>
#include <selectionservice/utils.h>
#include <commands\common_functions.h>

using namespace omsl::command;
using namespace omsl::commands::tp_analyzer;

namespace omsl
{
    namespace commands
    {
        namespace tp_analyzer
        {

            OMSL_COMMANDGUI_FACTORY ( TPSpeedAnalysis )
            {
                return new TPSpeedAnalysisGUI ( pParent, pCommand, pDocument );
            }

            TPSpeedAnalysisGUI::TPSpeedAnalysisGUI ( void* pParent, omsl::command::Command* pCommand, omsl::hcntcore::Document* pDocument ) : m_pCommand ( pCommand )
            {
                clear_callStackString ();
                callStackString ( L"TPSpeedAnalysisGUI", 1 );
                OMSL_INITIALIZE_REFCOUNTER ( TPSpeedAnalysisGUI );
                m_pDocument = pDocument;
                m_pParent = ( omsl::gui::HandShaker* )pParent;
                callStackString ( L"TPSpeedAnalysisGUI", -1 );
            }

            OMSL_IMPL ( omsl::Bool ) TPSpeedAnalysisGUI::Initialize ()
            {
                callStackString ( L"Initialize", 1 );
                //! Add a translation file
                //! \param name as const wchar_t*: the file name.
                //! \remarks the file is searched in (installation)\files\language. If name is an absoulute path,
                //! the file is loaded as is, just adding extension policy.
                //! To the file name is added the locale string (e.g. en, de, it, en_UK) as extension and the extension .qm
                //! myfilename => myfilename.en.qm 
                std::wstring languagePath ( L"%MYDIR%/files/language/ToolpathAnalyser/tpanalyser" );
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
                            languagePath = t;
                            //                                s += L"\\toolpathanalyser\\files\\commands\\tp.analysis.description";
                            languagePath += L"\\files\\language\\ToolpathAnalyser\\tpanalyser";
                        }
                    }
                }

                (void) omsl::gui::AddTranslationFile ( languagePath.data () );

                if ( Load () )
                {
                    callStackString ( L"Initialize", -1 );
                    return omsl::False;
                }

                
                callStackString ( L"Initialize", -1 );
                return omsl::True;
            }

            int TPSpeedAnalysisGUI::Load ()
            {
                callStackString ( L"Load", 1 );
                WeakPtr<guitools::AutomaticGuiBinding> pBind ( ( guitools::AutomaticGuiBinding* )
                                                               m_pParent->Init ( m_pCommand.get () )->Cast ( L"AutomaticGuiBinding" ) );
                if ( !pBind )
                {
                    callStackString ( L"Load", -1 );
                    return -1;
                }
                m_pIntoolMgr = pBind->GetIntoolManager ();
                
                m_pIntoolMgr->SetActiveIntool ( tpanalysis::UI_Toolpath );
                
                WeakPtr<intool::Intool> pIntool ( m_pIntoolMgr->GetIntool ( tpanalysis::UI_Toolpath ) );

//                m_pIntoolMgr->SetActiveIntool ( tpanalysis::UI_FirstEntities );

//                WeakPtr<intool::Intool> pIntool ( m_pIntoolMgr->GetIntool ( tpanalysis::UI_FirstEntities ) );
                if ( pIntool )
                {

                    WeakPtr<intool::Selector> pSelector ( ( intool::Selector* ) pIntool->Cast ( L"Selector" ) );

                    if ( pSelector )
                    {
                        pSelector->SetAutomaticPreselection ( omsl::True );
                    }
                }

//                SetupTPathSelector ();

                callStackString ( L"Load", -1 );
                return 0;
            }

            void TPSpeedAnalysisGUI::SetupTPathSelector () // iMode )
            {
                callStackString ( L"SetupTPathSelector", 1 );
                bool stepToNextButton = false;
                    
                // unhighlight preselections
                {
                    WeakPtr<intool::Intool> pIntool ( m_pIntoolMgr->GetIntool ( tpanalysis::UI_Toolpath ) );
                    if ( pIntool )
                    {
                        WeakPtr<intool::Selector> pSelector ( ( intool::Selector* ) pIntool->Cast ( L"Selector" ) );
                        if ( pSelector )
                        {
                            WeakPtr<selectionservice::InteractiveSelector> pISelector ( pSelector->GetInteractiveSelector () );
                            if ( pISelector && pISelector->GetSelections () )
                            {
                                pISelector->GetSelections ()->SetHighlightEnabled ( omsl::False );
                                pISelector->GetSelections ()->SetHighlight ( omsl::selectionservice::kHighlightOff );
                                pISelector->SetHighlight ( omsl::False );
                            }
                        }
                    }
                }

                // selection from outside
                // try to get the preselected selector data and to set into the internal selector
                {

                    WeakPtr<selectionservice::InteractiveSelector> pSelector2 ( omsl::selectionservice::Get ()->GetCurrentSelector ( m_pDocument.Get () ) );

                    if ( pSelector2.Get () )
                    {
                        omsl::SharedPtr<omsl::selectionservice::Selection> pSelection2 =
                            omsl::selectionservice::Get ()->GetIdleSelectionReset ( m_pDocument.Get (), omsl::selectionservice::kHighlightOff );

                        SharedPtr<selectionservice::EntityFilter> pCurrFilter2 ( selectionservice::EntityFilter::Create ().get () );
                        if ( pCurrFilter2.Get () )
                        {
                            /// result : all objects from type topogeom
                            SharedPtr< iterator::hcntcore::CadEntityForward > ppp2 = pSelection2->GetEntities ();

                            pCurrFilter2->Reset ();
                            pCurrFilter2->SetMode ( L"ToolPathEntity", selectionservice::EntityFilter::kEnable );
                            omsl::selectionservice::Get ()->GetSnap ()->GetTypes ()->SetValue ( L"PolylineIthPoint", omsl::False );

                            pSelection2->ApplyFilter ( pCurrFilter2.Get (), omsl::selectionservice::kHighlightOff, omsl::selectionservice::kHighlightOff );
                            pSelection2->SetHighlight ( omsl::selectionservice::kHighlightOff );

                            SharedPtr< iterator::hcntcore::CadEntityForward > ppp = pSelection2->GetEntities ();

                                m_pCommand->GetContext ()->SetValue ( tpanalysis::UI_Toolpath, ppp );
                                m_pCommand->GetContext ()->SetValue ( tpanalysis::ToolpathCounter, ppp->Size () );

                            if ( ppp->Size () > 0 ) // current result ToolpathEntities --> should be TopoGeom 
                            {

                                stepToNextButton = true;
                            }
                        }
                    }
                }

                m_pIntoolMgr->SetActiveIntool ( tpanalysis::UI_Toolpath );

                // selection settings inside my tool

                WeakPtr<intool::Intool> pIntool ( m_pIntoolMgr->GetIntool ( tpanalysis::UI_Toolpath ) );

                if ( !pIntool )
                {
                    callStackString ( L"SetupTPathSelector pIntool", -1 );
                    return;
                }

                WeakPtr<intool::Selector> pSelector ( ( intool::Selector* ) pIntool->Cast ( L"Selector" ) );

                if ( !pSelector )
                {
                    callStackString ( L"SetupTPathSelector pSelector", -1 );
                    return;
                }

                WeakPtr<selectionservice::InteractiveSelector> pISelector ( pSelector->GetInteractiveSelector () );

                if ( !pISelector )
                {
                    callStackString ( L"SetupTPathSelector pISelector", -1 );
                    return;
                }

                selectionservice::ObjectTypes_t selTypes = selectionservice::kEntities;
                pISelector->SetObjectTypes ( selTypes );
                pISelector->SetHighlight ( omsl::selectionservice::kHighlightOff );

                if ( pISelector && pISelector->GetSelections () )
                {
                    pISelector->GetSelections ()->SetHighlightEnabled ( omsl::False );
                    pISelector->SetHighlight ( omsl::False );
                    pISelector->GetSelections ()->SetHighlight ( omsl::selectionservice::kHighlightOff );
//                    pISelector->GetSelections ()->Reset ( omsl::selectionservice::kHighlightOff );
                }
                

                WeakPtr<selectionservice::EntityFilter> pCurrFilter ( selectionservice::GetEntityFilterUtilities ()->GetEntityFilter ( pISelector->GetFilters () ) );

                if ( pCurrFilter )
                {
                    pCurrFilter->Reset ();
                    pCurrFilter->SetMode ( L"ToolPathEntity", selectionservice::EntityFilter::kEnable );
                    omsl::selectionservice::Get ()->GetSnap ()->GetTypes ()->SetValue ( L"PolylineIthPoint", omsl::False );
                }

                // end of intool settings
                callStackString ( L"SetupTPathSelector", -1 );
            }
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
