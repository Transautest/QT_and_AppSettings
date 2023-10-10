#pragma once
#include "tp_listeningGUIclass.h"
#include "tp_localevent.h"
#include "tp_GUI_listener.h"
#include <sstream>
#include <memory>

#include "../GUI/tpanalysisgui.h"

namespace omsl
{
namespace hmbroadcast
{

static int s_regListenerGUICounter = 0;

// to listen toggle events from menue
std::shared_ptr< ListenSelectResetClass > gListenShowClass;

ListenSelectResetClass* GetListenSelectResetClass ()
{
    if ( !gListenShowClass )
        gListenShowClass.reset ( new ListenSelectResetClass );
    return gListenShowClass.get ();
}

void __stdcall RegisterGuiListener ()
{
    if ( s_regListenerGUICounter == 0 )
    {
        if ( !GetListenSelectResetClass ()->IsRegistered () )
            GetListenSelectResetClass ()->AddConnection ();
    }

    s_regListenerGUICounter++;
}

void __stdcall SetGuiToListener ( void *gui )
{
    if ( GetListenSelectResetClass ()->IsRegistered () )
        GetListenSelectResetClass ()->SetGUI ( gui );
}

void  __stdcall RemoveGuiListener ()
{
    if ( s_regListenerGUICounter == 1 )
    {
        if ( gListenShowClass )
            if ( GetListenSelectResetClass ()->IsRegistered () )
                GetListenSelectResetClass ()->RemoveConnection ();
    }

    s_regListenerGUICounter--;
}


ListenSelectResetClass::ListenSelectResetClass ()
:  m_gui ( nullptr )
{}

ListenSelectResetClass::~ListenSelectResetClass ()
{
    RemoveConnection ();
}

omsl::Long __stdcall ListenSelectResetClass::Slot_1 ( omsl::selectionservice::InteractiveSelector* pIntSel )
{
    std::wostringstream os;
    bool res = false;
    {
        if ( m_gui != nullptr ) // && cadEntityPtr != nullptr )
        {
            {
                omsl::commands::tp_analyzer::TPAnalysisGUI* pCommandGui = ( omsl::commands::tp_analyzer::TPAnalysisGUI* ) m_gui;
                res = pCommandGui->CheckSliderReset
                (
                    pIntSel 
                );
            }
        }
    }

    return 0;
}

void ListenSelectResetClass::AddConnection ()
{
    if ( !m_Connector )
    {
        m_Connector.reset ( new omsl::connection::ConnectorT< omsl::selectionservice::events::UserInvokedReset, ListenSelectResetClass > ( this, &ListenSelectResetClass::Slot_1 ) );
    }

    m_Connector->AddPermanent ();
    m_registered = true;
}

void ListenSelectResetClass::SetGUI ( void *gui )
{
    m_gui = gui;
}

void ListenSelectResetClass::RemoveConnection ()
{
    if ( m_Connector )
    {
        m_Connector->Disconnect ();
        m_Connector->Remove ();
        m_gui   = nullptr;
        m_registered = false;
    }
}

bool ListenSelectResetClass::IsConnected ()
{
    auto retVal = m_Connector ? m_Connector->IsConnected () : 0;
    return retVal != 0;
}

void ListenSelectResetClass::DeleteConnector ()
{
    RemoveConnection ();
    m_Connector.reset ();
}

bool ListenSelectResetClass::IsRegistered ()
{
    return m_registered;
}

}}
