#pragma once
#include "tp_listeningclass.h"
#include "tp_localevent.h"
#include "tp_listener.h"
#include <sstream>
#include <memory>
#include "../model/tpanalysismodel.h"
//#include "../model/tp_speedanalysismodel.h"

namespace omsl
{
namespace hmbroadcast
{

    static int s_regTPAListenerCounter = 0;

// to listen common signals between CadShell and TPA
std::shared_ptr< ListeningTPSignalClass > gListeningTPClass;

// to get the tooltip settings and localisation
std::shared_ptr< ListeningTTSignalClass > gListeningTTClass;

ListeningTPSignalClass* GetListeningTPClass ()
{
    if ( !gListeningTPClass )
        gListeningTPClass.reset ( new ListeningTPSignalClass () );
    return gListeningTPClass.get ();
}

ListeningTTSignalClass* GetListeningTTClass ()
{
    if ( !gListeningTTClass )
        gListeningTTClass.reset ( new ListeningTTSignalClass () );
    return gListeningTTClass.get ();
}

void __stdcall RegisterListener ()
{
    if ( s_regTPAListenerCounter == 0 )
    {
        if ( !GetListeningTPClass ()->IsRegistered () )
            GetListeningTPClass ()->AddConnection ();

        if ( !GetListeningTTClass ()->IsRegistered () )
            GetListeningTTClass ()->AddConnection ();
    }

    s_regTPAListenerCounter++;
}

void __stdcall SetModelToListener ( void *model, bool isFRA )
{

    if ( GetListeningTPClass ()->IsRegistered () )
        GetListeningTPClass ()->SetModel ( model, isFRA );

    if ( GetListeningTTClass ()->IsRegistered () )
        GetListeningTTClass ()->SetModel ( model, isFRA );
}

void  __stdcall RemoveListener ()
{
    if ( s_regTPAListenerCounter == 1 )
    {
        if ( gListeningTPClass )
            if ( GetListeningTPClass ()->IsRegistered () )
                GetListeningTPClass ()->RemoveConnection ();

        if ( gListeningTTClass )
            if ( GetListeningTTClass ()->IsRegistered () )
                GetListeningTTClass ()->RemoveConnection ();
    }
    if ( s_regTPAListenerCounter > 0 )
        s_regTPAListenerCounter--;
}


ListeningTPSignalClass::ListeningTPSignalClass()
: m_isFRA (false)
, m_registered (false)
{
    m_model = nullptr;
}

ListeningTPSignalClass::~ListeningTPSignalClass()
{
    RemoveConnection();
    m_model = nullptr;
    m_isFRA = false;
    m_registered = false;
}

omsl::Long __stdcall ListeningTPSignalClass::Slot_4( const wchar_t* /*name*/, int type, omsl::hcntcore::CadEntity* cadEntityPtr, int data ) // run cadshell function
{
    std::wostringstream os;
    bool res = false;
    if ( m_model != nullptr )
    {
        {
            omsl::commands::tp_analyzer::TPAnalysisModel* pCommandModel = ( omsl::commands::tp_analyzer::TPAnalysisModel* ) m_model;
        
            res = pCommandModel->CheckSignalTypeForCadEntityTPA
            (
                cadEntityPtr
                , type
                , data
            );
        }
    }

    return 0;
}

void ListeningTPSignalClass::AddConnection()
{
  if ( !m_Connector )
    m_Connector.reset( new omsl::connection::ConnectorT< HMCS_Event, ListeningTPSignalClass >( this, &ListeningTPSignalClass::Slot_4 ) );
  m_Connector->AddPermanent();
  m_registered = true;
}

void ListeningTPSignalClass::SetModel ( void *model, bool /*isFRA*/ )
{
    if ( m_model != nullptr && s_regTPAListenerCounter > 1 )
    {
        omsl::commands::tp_analyzer::TPAnalysisModel* pCommandModel = ( omsl::commands::tp_analyzer::TPAnalysisModel* ) m_model;
        if ( pCommandModel )
        {
//            pCommandModel->ResetGraphicsSize ();
            omsl::commands::tp_analyzer::TPAnalysisModel* pNewCommandModel = ( omsl::commands::tp_analyzer::TPAnalysisModel* ) model;
            if ( pNewCommandModel )
                pNewCommandModel->copyInternalSettings ( pCommandModel );
        }
    }

    m_model = model;
}

void ListeningTPSignalClass::RemoveConnection()
{
  if( m_Connector )
  {
    m_Connector->Disconnect();
    m_Connector->Remove();
    m_registered = false;
  }
}

bool ListeningTPSignalClass::IsConnected()
{
  auto retVal = m_Connector ? m_Connector->IsConnected() : 0;
  return retVal != 0;
}

void ListeningTPSignalClass::DeleteConnector()
{
  RemoveConnection();
  m_Connector.reset();
}

bool ListeningTPSignalClass::IsRegistered()
{
  return m_registered;
}

ListeningTTSignalClass::ListeningTTSignalClass ()
    : m_isFRA ( false )
    , m_registered ( false )
{
    m_model = nullptr;
}

ListeningTTSignalClass::~ListeningTTSignalClass ()
{
    RemoveConnection ();
    m_model = nullptr;
    m_isFRA = false;
    m_registered = false;
}

omsl::Long __stdcall ListeningTTSignalClass::Slot_4 ( const wchar_t* name, int type, const wchar_t* locstring, int data )
{
    std::wostringstream os;
    bool res = false;
    if ( m_model != nullptr )
    {
        {
            omsl::commands::tp_analyzer::TPAnalysisModel* pCommandModel = ( omsl::commands::tp_analyzer::TPAnalysisModel* ) m_model;

            res = pCommandModel->AddTTipLocString
            (
                  name
                , type
                , locstring
                , data
            );
        }
    }

    return 0;
}

void ListeningTTSignalClass::AddConnection ()
{
    if ( !m_Connector )
        m_Connector.reset ( new omsl::connection::ConnectorT< TTIP_Event, ListeningTTSignalClass > ( this, &ListeningTTSignalClass::Slot_4 ) );
    m_Connector->AddPermanent ();
    m_registered = true;
}

void ListeningTTSignalClass::SetModel ( void *model, bool isFRA )
{
    m_model = model;
    m_isFRA = isFRA;
}

void ListeningTTSignalClass::RemoveConnection ()
{
    if ( m_Connector )
    {
        m_Connector->Disconnect ();
        m_Connector->Remove ();
        m_registered = false;
    }
}

bool ListeningTTSignalClass::IsConnected ()
{
    auto retVal = m_Connector ? m_Connector->IsConnected () : 0;
    return retVal != 0;
}

void ListeningTTSignalClass::DeleteConnector ()
{
    RemoveConnection ();
    m_Connector.reset ();
}

bool ListeningTTSignalClass::IsRegistered ()
{
    return m_registered;
}

}}
