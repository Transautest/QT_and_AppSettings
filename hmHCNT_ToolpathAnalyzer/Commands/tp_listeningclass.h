#pragma once
//#define OMSL_TYPEDEF_BOOST
#include <portability.h>
#include <connection/connectort.h>

#include <connection\connectionhelper.h>
#include <connection\connectioncontainer.h>
#include <boostwrapper/connectionwrapper.h>
#include <boostwrapper/sharedconnection.h>

#include <hcntcore/model.h>

namespace omsl
{
namespace hmbroadcast
{

class ListeningTPSignalClass
{
  //.....
  // this is the method actually called by signal - the signature depends on signal
public:
  ListeningTPSignalClass();
  ~ListeningTPSignalClass();
  omsl::Long __stdcall Slot_4( const wchar_t* name, int type, omsl::hcntcore::CadEntity* cadEntityPtr, int data );

  void AddConnection();
  void SetModel ( void *model, bool isFRA );
  void RemoveConnection();
  bool IsConnected();
  void DeleteConnector();
  bool IsRegistered();
  // the Connect method to allow the client to connect to

protected:
  std::shared_ptr< omsl::connection::ConnectorBase > m_Connector;
  bool m_registered = false;
  bool m_isFRA;
  void* m_model;
};


class ListeningTTSignalClass
{
  //.....
  // this is the method actually called by signal - the signature depends on signal
public:
  ListeningTTSignalClass();
  ~ListeningTTSignalClass();
  omsl::Long __stdcall Slot_4( const wchar_t* name, int type, const wchar_t* locstring, int data );

  void AddConnection();
  void SetModel ( void *model, bool isFRA );
  void RemoveConnection();
  bool IsConnected();
  void DeleteConnector();
  bool IsRegistered();
  // the Connect method to allow the client to connect to

protected:
  std::shared_ptr< omsl::connection::ConnectorBase > m_Connector;
  bool m_registered = false;
  bool m_isFRA;
  void* m_model;
};

}
}
