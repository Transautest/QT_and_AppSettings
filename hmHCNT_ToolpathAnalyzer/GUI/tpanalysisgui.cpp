#include <propertyservice/propertybag/propertytypedbag.h>
#include <propertyservice/typednametdef.h>

#include <portability.h>
#include <guitools/eventemitters.h>

#include "../Commands/tpanalysepropertiesimpl.h"

#include "ctkRangeSlider.h"
#include <qgroupbox.h>
#include <qabstractbutton.h>
#include <gui/selector.h>
#include <qlayout.h>

#include "tpanalysisgui.h"
// keys
#include <Commands\tpanalysiskeys.h>
// command context
#include <propertyservice/context/context.h>
#include <command/command.h>
#include <command/commandsignal.h>
//#include <commands/>
// Document
#include <hcntcore\document.h>

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

#include <commands/tp_GUI_listener.h>
#include <commands/tp_signalemitter.h>
#include <measureunit\converter.h>
#include <measureunit\utility.h>
#include <units\dimension.h>

#include <propertyservice\propertybag\modality.h>
#include <propertyservice\events.h>
#include <connection/connectort.h>

#include "translate.h"

using namespace omsl::command;
using namespace omsl::commands::info;
using namespace omsl::commands::tp_analyzer;

using omsl::linguistic::QT_TRANSLATE_NOOP;

//extern void ( *s_pFctIntervalChanged )( int, int );
omsl::WeakPtr<omsl::command::Command>            s_pCommand;

static int s_defined = 0;

namespace omsl
{
    namespace propertyservice
    {
        class Modality;
    }
}

namespace omsl
{
    namespace commands
    {
        const wchar_t* commands::tp_analyzer::TPAnalysisGuiHelper::SelectorKey ()
        {
            return L"tpanal"; // tpanalysis::selector.Name;
        }

        namespace info
        {
            const wchar_t* QT_HISTOGRAM_NAME = L"Histogram";
            static const wchar_t* QT_LISTITEMCOLORVALUES_NAME = L"ColorSteps";
            static const double kLowerGrey = 0.4;
            static const double kUpperGrey = 0.88;
            static const Rgb kPlanarColor ( 1., 0.68, 0.78 );// 0.7, 0.4, 0.6 );
            const wchar_t* QT_RANGESLIDER_NAME = L"UI_DblSldr_Limits";
            const wchar_t* QTLIMITGROUPBOX_NAME = L"ToolPathLimits";
            const wchar_t* QTSELECTGROUPBOX_NAME = L"UI_GBox_Sel_Entities";
            const wchar_t* QTBUTTONFIRST_NAME = L"UI_Btn_FirstEntities";
            //namespace analysis
            //{
            //    OMSL_PRIVATE_COMMAND_KEY_U ( info.Analysis, UpdateMeasureUnits, Bool, L"Update measure units" );
            //}  // namespace draftanalysis 

        }

        namespace tp_analyzer
        {

            OMSL_COMMANDGUI_FACTORY ( TPAnalysis )
            {
                return new TPAnalysisGUI ( pParent, pCommand, pDocument );
            }

            TPAnalysisGUI::TPAnalysisGUI ( void* pParent, omsl::command::Command* pCommand, omsl::hcntcore::Document* pDocument ) 
            : m_pCommand ( pCommand )
            , m_pHelper (nullptr)
            , m_pConverter (nullptr)
            {
                clear_callStackString ();

                OMSL_INITIALIZE_REFCOUNTER ( TPAnalysisGUI );
                m_pDocument = pDocument;
                m_pParent = ( omsl::gui::HandShaker* )pParent;

                m_pConverter = omsl::measureunit::Converter::CreateGeneric ( omsl::units::Dimension::Length (), nullptr );
                m_pConverter->SetUnit ( L"mm" );
                m_postfix = m_pConverter->GetPostfix ();
                m_pConverter->SetPostfix ( omsl::False );
                omsl::hmbroadcast::RegisterSender ();
                omsl::hmbroadcast::RegisterGuiListener ();

                s_defined++;
            }

            TPAnalysisGUI::~TPAnalysisGUI ()
            {
                m_pConverter->SetPostfix ( m_postfix );

                omsl::hmbroadcast::RemoveSender ();
                omsl::hmbroadcast::RemoveGuiListener ();
                int i = 0;
                i++;
                s_defined--;
            }

            OMSL_IMPL ( omsl::Bool ) TPAnalysisGUI::Initialize ()
            {
                if ( s_defined > 1 )
                    return false;

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

            int TPAnalysisGUI::Load ()
            {
                callStackString ( L"Load", 1 );
                s_pCommand = m_pCommand.Get ();

                typedef std::shared_ptr< omsl::connection::ConnectorBase > Connector_t;
                Connector_t connector;
                connector.reset ( new omsl::connection::ConnectorT< propertyservice::events::ModalityStarted, TPAnalysisGUI >
                    ( this, &TPAnalysisGUI::OnModalityStarted ) );
                connector->AddPermanent ();


                m_pBind = ( ( guitools::AutomaticGuiBinding* ) m_pParent->Init ( m_pCommand.get () )->Cast ( L"AutomaticGuiBinding" ) );

                connector->Disconnect ();
                connector->Remove ();

                if ( !m_pBind )
                {
                    callStackString ( L"Load", -1 );
                    return -1;
                }

                HWND wId = m_pBind->GetObjectId ( info::QTLIMITGROUPBOX_NAME );
                if ( wId )
                {
                    QWidget *pWid = QWidget::find ( ( WId ) wId );
                    if ( pWid )
                    {
                        HWND wIdqsl = m_pBind->GetObjectId ( info::QT_RANGESLIDER_NAME );
                        if ( wIdqsl )
                        {
                            QWidget *pWidqsl = QWidget::find ( ( WId ) wIdqsl );
                            QSlider *pSlider = qobject_cast < QSlider* > ( pWidqsl );
                            if ( pWid )
                            {
                                pRangeSlider = new ctkRangeSlider ( Qt::Horizontal, pWid );
                                pRangeSlider->setObjectName ( QStringLiteral ( "UI_DblSldr_Limits" ) );
                                QSize sz = pSlider->baseSize ();

                                pRangeSlider->setBaseSize ( sz );
                                sz = pSlider->size ();
                                pRangeSlider->setBaseSize ( sz );
                                pRangeSlider->setFixedSize ( sz );
                                pRangeSlider->setLayout ( pSlider->layout() );
                                pRangeSlider->setSizePolicy ( pSlider->sizePolicy () );
                                pRangeSlider->setGeometry ( pSlider->geometry () );
                                pRangeSlider->setMinimumSize ( pSlider->minimumSize () );
                                pRangeSlider->setMaximumSize ( pSlider->maximumSize () );
                                pRangeSlider->setStyle ( pSlider->style () );
                                pRangeSlider->setBackgroundRole ( pSlider->backgroundRole () );
                                pRangeSlider->setStyleSheet ( pSlider->styleSheet () );
                                //s_pFctIntervalChanged = OnSliderSetMinMax;
//                                pWid->connect( pRangeSlider, SIGNAL ( ctkRangeSlider::valuesChanged ( int, int ) ), pRangeSlider, SLOT ( omsl::tp_analyzer::TPAnalysisGUI::rangeSliderIntervalChange ( int, int ) ) );
                            }
                            pSlider->hide ();
                        }
                    }
                }

                WeakPtr<omsl::command::CommandSignal> commandsignal = m_pCommand.weak_cast< command::CommandSignal >( L"CommandSignal" );
                m_vConnections.push_back ( SharedPtr<boostwrapper::Connection> ( commandsignal->ConnectAction ( command::CommandSignal::kApply, command::CommandSignal::Action_T::Slot ( &TPAnalysisGUI::OnApply ), this ) ) );
                m_vConnections.push_back ( SharedPtr<boostwrapper::Connection> ( commandsignal->ConnectAction ( command::CommandSignal::kExit, command::CommandSignal::Action_T::Slot ( &TPAnalysisGUI::OnExit ), this ) ) );

                m_vConnections.push_back ( ( SharedPtr<boostwrapper::Connection> ( m_pCommand->GetContext ()->ConnectValueChange ( tpanalysis::UI_sliderMin_2, &TPAnalysisGUI::OnSliderRangeMinimum, this ) ) ) );
                m_vConnections.push_back ( ( SharedPtr<boostwrapper::Connection> ( m_pCommand->GetContext ()->ConnectValueChange ( tpanalysis::UI_sliderMax_2, &TPAnalysisGUI::OnSliderRangeMaximum, this ) ) ) );
                m_vConnections.push_back ( ( SharedPtr<boostwrapper::Connection> ( m_pCommand->GetContext ()->ConnectValueChange ( tpanalysis::UI_sliderValueMin, &TPAnalysisGUI::OnSliderUserMinimum, this ) ) ) );
                m_vConnections.push_back ( ( SharedPtr<boostwrapper::Connection> ( m_pCommand->GetContext ()->ConnectValueChange ( tpanalysis::UI_sliderValueMax, &TPAnalysisGUI::OnSliderUserMaximum, this ) ) ) );
                m_vConnections.push_back ( ( SharedPtr<boostwrapper::Connection> ( m_pCommand->GetContext ()->ConnectValueChange ( tpanalysis::updateColorsTableAllowed, &TPAnalysisGUI::OnColorsDefinitionChanged, this ) ) ) );
                m_vConnections.push_back ( ( SharedPtr<boostwrapper::Connection> ( m_pCommand->GetContext ()->ConnectValueChange ( tpanalysis::resetPickPointSelection, &TPAnalysisGUI::OnResetPickPointChanged, this ) ) ) );
                m_vConnections.push_back ( ( SharedPtr<boostwrapper::Connection> ( m_pCommand->GetContext ()->ConnectValueChange ( tpanalysis::UI_sliderEnable, &TPAnalysisGUI::OnSliderEnable, this ) ) ) );

                OnSliderRangeMinimum ( m_pCommand->GetContext ()->GetValue <int> ( tpanalysis::UI_sliderMin_2 ) );
                OnSliderRangeMaximum ( m_pCommand->GetContext ()->GetValue <int> ( tpanalysis::UI_sliderMax_2 ) );
                OnSliderUserMaximum  ( m_pCommand->GetContext ()->GetValue <int> ( tpanalysis::UI_sliderMax_2 ) );
                OnSliderEnable ( m_pCommand->GetContext ()->GetValue <int> ( tpanalysis::UI_sliderEnable ) );

                if ( m_pBind )
                {
                    
                    QTableWidget *pListData = FindTableWidget ( QT_LISTITEMCOLORVALUES_NAME );
                    int i = 1;
                    while ( pListData != nullptr )
                    {
                        m_vpListData.push_back ( pListData );
                        std::wstring sName = QT_LISTITEMCOLORVALUES_NAME;
                        sName += L"_" + boost::lexical_cast< std::wstring >( i );
                        ++i;
                        pListData = FindTableWidget ( sName.c_str () );
                    }
                }

                m_pCommand->GetContext ()->SetValue <int> ( tpanalysis::updateFeedrateDraw, -1 );

                m_pIntoolMgr = m_pBind->GetIntoolManager ();
                
                m_pIntoolMgr->SetActiveIntool ( tpanalysis::UI_FirstEntities );
                m_pIntoolMgr->SetAutoNextFlag ( False );

                WeakPtr<intool::Intool> pIntool ( m_pIntoolMgr->GetIntool ( tpanalysis::UI_FirstEntities ) );

                if ( pIntool )
                {

                    WeakPtr<intool::Selector> pSelector ( ( intool::Selector* ) pIntool->Cast ( L"Selector" ) );

                    if ( pSelector )
                    {
                        pSelector->SetAutomaticPreselection ( omsl::True );
                    }
                }

                Setup1stSelector ();
                Setup2ndSelector ();

                omsl::hmbroadcast::SetGuiToListener ( this );

                callStackString ( L"Load", -1 );
                return 0;
            }

            void TPAnalysisGUI::SetupSliderPickSelector ()
            {

                for ( int i = 0; i < 2; i++ )
                {
                    WeakPtr<intool::Intool> pIntool ( i == 0 ? m_pIntoolMgr->GetIntool ( tpanalysis::UI_LeftSliderPick ) : m_pIntoolMgr->GetIntool ( tpanalysis::UI_RightSliderPick ));
                    ToolpathUsrFilterPtr usrFilter ( i == 0 ? m_tpUsrLeft : m_tpUsrRight );
                    if ( !pIntool )
                    {
                        return;
                    }

                    WeakPtr<intool::Selector> pSelector ( ( intool::Selector* ) pIntool->Cast ( L"Selector" ) );

                    if ( !pSelector )
                    {
                        return;
                    }

                    WeakPtr<selectionservice::InteractiveSelector> pSelection ( pSelector->GetInteractiveSelector () );

                    if ( !pSelection )
                    {
                        return;
                    }

                    pSelection->SetHighlight ( omsl::selectionservice::kHighlightNone );

                    pSelection->SetObjectTypes ( selectionservice::kSnapPoints );

                    WeakPtr<selectionservice::EntityFilter> pCurrFilter ( selectionservice::GetEntityFilterUtilities ()->GetEntityFilter ( pSelection->GetFilters () ) );

                    if ( pCurrFilter )
                    {
                        pCurrFilter->Reset ();
                        pCurrFilter->SetMode ( L"ParGeomEntity:ParGeom:Point", selectionservice::EntityFilter::kEnable );
                        pCurrFilter->SetMode ( L"GeomEntity:TopoGeom:Geom3d:Curve3d", selectionservice::EntityFilter::kDisableAlways );
                        pCurrFilter->SetMode ( L"ParGeomEntity:ParGeom:Curve:BoundCurve", selectionservice::EntityFilter::kDisableAlways );
                        pCurrFilter->SetMode ( L"CompositeEntity.GeomEntity:TopoGeom:Geom3d:Curve3d", selectionservice::EntityFilter::kDisableAlways );
                        pCurrFilter->SetMode ( L"CompositeEntity.ParGeomEntity:ParGeom:Curve:BoundCurve", selectionservice::EntityFilter::kDisableAlways );
                        pCurrFilter->SetMode ( L"ToolPathEntity", selectionservice::EntityFilter::kEnable );
                        pCurrFilter->SetMode ( L"ToolPathEntity.TopoGeom:ToolpathSegment", selectionservice::EntityFilter::kEnable );

                        omsl::selectionservice::Get ()->GetSnap ()->GetTypes ()->SetValue ( L"PolylineIthPoint", omsl::True );

                        // clean up, because of different selection filter
                        if ( usrFilter && usrFilter.Get () )
                        {
                            pSelection->GetFilters ()->Remove ( usrFilter.Get () );
                            usrFilter = nullptr;
                        }
                        {
                            usrFilter = new ToolpathUsrFilter ( m_pCommand, 0 );
                            if ( usrFilter.Get () )
                            {
                                pSelection->AddFilter ( usrFilter.Get () );
                            }
                        }
                    }
                }
            }

            void TPAnalysisGUI::Setup1stSelector ( )
            {
                WeakPtr<intool::Intool> pIntool ( m_pIntoolMgr->GetIntool ( tpanalysis::UI_FirstEntities ) );

                if ( !pIntool )
                {
                    return;
                }

                WeakPtr<intool::Selector> pSelector ( ( intool::Selector* ) pIntool->Cast ( L"Selector" ) );

                if ( !pSelector )
                {
                    return;
                }

                WeakPtr<selectionservice::InteractiveSelector> pSelection ( pSelector->GetInteractiveSelector () );

                if ( !pSelection )
                {
                    return;
                }

                pSelection->SetHighlight ( omsl::selectionservice::kHighlightNone );

                selectionservice::ObjectTypes_t selTypes = selectionservice::kPoints;

                selTypes = selectionservice::kEntitiesSnapPoints;

                pSelection->SetObjectTypes ( selTypes );

                WeakPtr<selectionservice::EntityFilter> pCurrFilter ( selectionservice::GetEntityFilterUtilities ()->GetEntityFilter ( pSelection->GetFilters () ) );

                if ( pCurrFilter )
                {
                    pCurrFilter->Reset ();

                    pCurrFilter->SetMode ( L"ParGeomEntity:ParGeom:Point", selectionservice::EntityFilter::kEnable );
                    pCurrFilter->SetMode ( L"GeomEntity:TopoGeom:Geom3d:Curve3d", selectionservice::EntityFilter::kDisableAlways );
                    pCurrFilter->SetMode ( L"ParGeomEntity:ParGeom:Curve:BoundCurve", selectionservice::EntityFilter::kDisableAlways );
                    pCurrFilter->SetMode ( L"CompositeEntity.GeomEntity:TopoGeom:Geom3d:Curve3d", selectionservice::EntityFilter::kDisableAlways );
                    pCurrFilter->SetMode ( L"CompositeEntity.ParGeomEntity:ParGeom:Curve:BoundCurve", selectionservice::EntityFilter::kDisableAlways );
                    pCurrFilter->SetMode ( L"ToolPathEntity", selectionservice::EntityFilter::kEnable );
                    pCurrFilter->SetMode ( L"ToolPathEntity.ToolpathSegment", selectionservice::EntityFilter::kEnable );

                    pCurrFilter->SetMode ( L"ParGeomEntity:ParGeom:Curve:ToolpathMovement", selectionservice::EntityFilter::kEnable );
                    pCurrFilter->SetMode ( L"CompositeEntity.ParGeomEntity:ParGeom:Curve:ToolpathMovement", selectionservice::EntityFilter::kEnable );

                    omsl::selectionservice::Get ()->GetSnap ()->GetTypes ()->SetValue ( L"PolylineIthPoint", omsl::True );

                    // clean up, because of different selection filter
                    if ( m_tpUsrFilter1 && m_tpUsrFilter1.Get () )
                    {
                        pSelection->GetFilters ()->Remove ( m_tpUsrFilter1.Get () );
                        m_tpUsrFilter1 = nullptr;
                    }

                    {
                        m_tpUsrFilter1 = new ToolpathUsrFilter ( m_pCommand, 0 );
                        if ( m_tpUsrFilter1.Get () )
                        {
                            pSelection->AddFilter ( m_tpUsrFilter1.Get () );
                        }
                    }
                }
            }

            void TPAnalysisGUI::Setup2ndSelector ( )
            {
                WeakPtr<intool::Intool> pIntool ( m_pIntoolMgr->GetIntool ( tpanalysis::UI_SecondEntities ) );

                if ( !pIntool )
                {
                    return;
                }

                WeakPtr<intool::Selector> pSelector ( ( intool::Selector* ) pIntool->Cast ( L"Selector" ) );

                if ( !pSelector )
                {
                    return;
                }

                WeakPtr<selectionservice::InteractiveSelector> pSelection ( pSelector->GetInteractiveSelector () );

                if ( !pSelection )
                {
                    return;
                }

                pSelection->SetHighlight ( omsl::selectionservice::kHighlightNone );

                selectionservice::ObjectTypes_t selTypes = selectionservice::kPoints;
              
                selTypes = selectionservice::kEntitiesSnapPoints;

                pSelection->SetObjectTypes ( selTypes );

                WeakPtr<selectionservice::EntityFilter> pCurrFilter ( selectionservice::GetEntityFilterUtilities ()->GetEntityFilter ( pSelection->GetFilters () ) );

                if ( pCurrFilter )
                {
                    pCurrFilter->Reset ();

                    pCurrFilter->SetMode ( L"ParGeomEntity:ParGeom:Point", selectionservice::EntityFilter::kEnable );
                    pCurrFilter->SetMode ( L"GeomEntity:TopoGeom:Geom3d:Curve3d", selectionservice::EntityFilter::kEnable );
                    pCurrFilter->SetMode ( L"GeomEntity:TopoGeom", selectionservice::EntityFilter::kEnable );
                    pCurrFilter->SetMode ( L"ParGeomEntity:ParGeom:Curve:BoundCurve", selectionservice::EntityFilter::kEnable );
                    pCurrFilter->SetMode ( L"CompositeEntity.GeomEntity:TopoGeom:Geom3d:Curve3d", selectionservice::EntityFilter::kEnable );
                    pCurrFilter->SetMode ( L"CompositeEntity.ParGeomEntity:ParGeom:Curve:BoundCurve", selectionservice::EntityFilter::kEnable );
                    pCurrFilter->SetMode ( L"GeomEntity:TopoGeom:Cell:Face", selectionservice::EntityFilter::kEnable );
                    pCurrFilter->SetMode ( L"CompositeEntity.GeomEntity:TopoGeom:Cell:Face", selectionservice::EntityFilter::kEnable );

                    //pCurrFilter->SetMode ( L"GeomEntity:TopoGeom:Geom3d:Curve3d", selectionservice::EntityFilter::kDisableAlways );
                    //pCurrFilter->SetMode ( L"ParGeomEntity:ParGeom:Curve:BoundCurve", selectionservice::EntityFilter::kDisableAlways );
                    //pCurrFilter->SetMode ( L"CompositeEntity.GeomEntity:TopoGeom:Geom3d:Curve3d", selectionservice::EntityFilter::kDisableAlways );
                    //pCurrFilter->SetMode ( L"CompositeEntity.ParGeomEntity:ParGeom:Curve:BoundCurve", selectionservice::EntityFilter::kDisableAlways );
                    pCurrFilter->SetMode ( L"ToolPathEntity", selectionservice::EntityFilter::kEnable );
                    pCurrFilter->SetMode ( L"ToolPathEntity.ToolpathSegment", selectionservice::EntityFilter::kEnable );

                    pCurrFilter->SetMode ( L"ParGeomEntity:ParGeom:Curve:ToolpathMovement", selectionservice::EntityFilter::kEnable );
                    pCurrFilter->SetMode ( L"CompositeEntity.ParGeomEntity:ParGeom:Curve:ToolpathMovement", selectionservice::EntityFilter::kEnable );

                    omsl::selectionservice::Get ()->GetSnap ()->GetTypes ()->SetValue ( L"PolylineIthPoint", omsl::True );
                }
            }

            void TPAnalysisGUI::OnResetPickPointChanged ( const int& value )
            {
                if ( value == 0 )
                    return;

                if ( m_pIntoolMgr.Get () )
                {
                    WeakPtr<intool::Intool> pIntool;
                    if ( value == 2 )
                    {
                        pIntool = m_pIntoolMgr->GetIntool ( tpanalysis::UI_LeftSliderPick );
                        if ( pIntool ) pIntool->Reset ();
                    }
                    if ( value == 1 )
                    {
                        pIntool = m_pIntoolMgr->GetIntool ( tpanalysis::UI_RightSliderPick );
                        if ( pIntool ) pIntool->Reset ();
                    }
                }
            }

            void TPAnalysisGUI::OnApply ( signal::Phase phase, command::Command* /*pCommand*/ )
            {
                if ( phase == omsl::signal::kPost )
                {

                }
            }

            void TPAnalysisGUI::OnExit ( signal::Phase phase, command::Command* /*pCommand*/ )
            {
                //  redirectoutput::ClearStatus( this );
                if ( phase == omsl::signal::kPre )
                {
                    //OnCommandExitPre ();
                    //ResetTooltip ();
                    //ResetTransparency ();
                }
            }

            void TPAnalysisGUI::OnSelected ( const SharedPtr<iterator::hcntcore::CadEntityForward> & )
            {
                //if ( !m_transparencyChecked )
                //    InitTransparency ();
            }


            omsl::Long __stdcall TPAnalysisGUI::OnModalityStarted ( propertyservice::Modality* )
            {
                omsl::commands::tp_analyzer::Properties* tpp = GetProperties ();
                if ( !tpp )
                {
                    return -1;
                }
                SharedPtr<propertyservice::PropertyBag> settings ( tpp->GetBag () );
                if ( settings )
                {
                    auto iter = settings->GetIterator ();
                    if ( iter && iter->Size () )
                    {
                        for ( OMSL_EACH ( iter ) )
                        {
                            auto data = guitools::events::CreateBindData ( settings.get (), iter->GetFirst (), omsl::False );
                            guitools::events::BindEmitter ().Emit ( signal::kPost, data.get () );
                        }
                    }
                }

                return 0;
            }

            OMSL_IMPL (bool) TPAnalysisGUI::CheckSliderReset ( omsl::selectionservice::InteractiveSelector* pIntSel )
            {
                for ( int i = 0; i < 2; i++ )
                {
                    WeakPtr<intool::Intool> pIntool ( i == 0 ? m_pIntoolMgr->GetIntool ( tpanalysis::UI_LeftSliderPick ) : m_pIntoolMgr->GetIntool ( tpanalysis::UI_RightSliderPick ) );
                    if ( !pIntool )
                    {
                        return false;
                    }

                    WeakPtr<intool::Selector> pSelector ( ( intool::Selector* ) pIntool->Cast ( L"Selector" ) );

                    if ( !pSelector )
                    {
                        return false;
                    }

                    WeakPtr<selectionservice::InteractiveSelector> pSelection ( pSelector->GetInteractiveSelector () );

                    if ( !pSelection )
                    {
                        return false ;
                    }

                    if ( pSelection.Get () == pIntSel ) // match
                    {
                        if ( i == 0 )
                            m_pCommand->GetContext ()->SetValue <int> ( tpanalysis::UI_sliderValueMin, 0 );
                        else
                            m_pCommand->GetContext ()->SetValue <int> ( tpanalysis::UI_sliderValueMax, DEFAULTSEGMAX );
                        return true;
                    }
                }

                return false;
            }

            bool TPAnalysisGuiHelper::ResetHistogram ( double * dMin, double * dMax )
            {
                return ( bgeo::IsZero ( dMin [0] - dMin [1] ) && bgeo::IsZero ( dMax [0] - dMax [1] ) );
            }
            //const wchar_t* TPAnalysisGUI::UpdateMeasureUnitsKey () const
            //{
            //    return tpanalysis::UpdateMeasureUnits.Name;
            //}
        }  // namespace tp_analyzer
    }  // namespace commands
}  // namespace omsl
