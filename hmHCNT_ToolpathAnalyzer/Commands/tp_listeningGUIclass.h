#pragma once
//#define OMSL_TYPEDEF_BOOST
#include <portability.h>
#include <connection/connectort.h>

#include <connection\connectionhelper.h>
#include <connection\connectioncontainer.h>
#include <boostwrapper/connectionwrapper.h>
#include <boostwrapper/sharedconnection.h>

#include <selectionservice/events.h>

#include <hcntcore/model.h>

namespace omsl
{
namespace hmbroadcast
{

class ListenSelectResetClass
{
    //.....
    // this is the method actually called by signal - the signature depends on signal
public:
    ListenSelectResetClass ();
    ~ListenSelectResetClass ();
    omsl::Long __stdcall Slot_1 ( omsl::selectionservice::InteractiveSelector* );

    void AddConnection ();
    void SetGUI ( void *gui );
    void RemoveConnection ();
    bool IsConnected ();
    void DeleteConnector ();
    bool IsRegistered ();
    // the Connect method to allow the client to connect to

protected:
    std::shared_ptr< omsl::connection::ConnectorBase > m_Connector;
    bool m_registered = false;
    void* m_gui;
};


}
}
