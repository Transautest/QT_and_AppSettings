#pragma warning( disable: 4996 )
#define OMSL_TYPEDEF_BOOST
#include "boost/bind/bind.hpp"
#include "boost/signals2.hpp"
#include "tp_localevent.h"
#include <connection\connectionhelper.h>
#include <memory>

#include <stdio.h>

using namespace boost::placeholders;

namespace omsl
{
namespace hmbroadcast
{

static int s_regTPAEmitterCounter = 0;
static int s_traceCount = 0;
static bool s_trace = false;

static wchar_t s_traceString [1000] [150];

class EmitterTPSignalClass
{
  public:
  EmitterTPSignalClass();
  ~EmitterTPSignalClass();

  void Register();

  omsl::boostwrapper::Connection* __stdcall Connect( TPA_Event::Type::Slot  subscriberMethod, void* subscriberObj );

  void Unregister();
  void Emit ( const wchar_t* name, int type, omsl::hcntcore::CadEntity* cadEntityPtr, int data, double f );
  bool IsRegistered();

  private:

  bool m_registered = false;

  TPA_Event::Type::Signal_t m_Signal;
};

//
//class EmitterMenueGVIconClass
//{
//  public:
//  EmitterMenueGVIconClass();
//  ~EmitterMenueGVIconClass();
//
//  void Register();
//
//  omsl::boostwrapper::Connection* __stdcall Connect( HCSMenueGVIcon::Type::Slot  subscriberMethod, void* subscriberObj );
//
//  void Unregister();
//
//  void Emit ( int type );
//  bool IsRegistered();
//
//  private:
//
//  bool m_registered = false;
//
//  HCSMenueGVIcon::Type::Signal_t m_Signal;
//};
//
//
//
//class EmitterMenuePTIconClass
//{
//  public:
//   EmitterMenuePTIconClass ();
//  ~EmitterMenuePTIconClass ();
//
//  void Register();
//
//  omsl::boostwrapper::Connection* __stdcall Connect( HCSMenueGVIcon::Type::Slot  subscriberMethod, void* subscriberObj );
//
//  void Unregister();
//
//  void Emit ( int type );
//  bool IsRegistered();
//
//  private:
//
//  bool m_registered = false;
//
//  HCSMenuePTIcon::Type::Signal_t m_Signal;
//};

std::shared_ptr< EmitterTPSignalClass > gEmitterTPClass;
//std::shared_ptr< EmitterMenueGVIconClass > gEmitterGVIClass;
//std::shared_ptr< EmitterMenuePTIconClass > gEmitterPTIClass;
//

void DeleteEmitterClass()
{
    if ( s_regTPAEmitterCounter == 0 )
    {
        s_traceCount = 0;
        gEmitterTPClass.reset ();
        //gEmitterGVIClass.reset ();
        //gEmitterPTIClass.reset ();
    }
}


EmitterTPSignalClass* GetEmitterTPClass()
{
  if( !gEmitterTPClass )
    gEmitterTPClass.reset( new EmitterTPSignalClass );
  return gEmitterTPClass.get();
}

//EmitterMenueGVIconClass* GetEmitterGVIClass()
//{
//  if( !gEmitterGVIClass )
//    gEmitterGVIClass.reset( new EmitterMenueGVIconClass );
//  return gEmitterGVIClass.get();
//}
//
//EmitterMenuePTIconClass* GetEmitterPTIClass()
//{
//  if( !gEmitterPTIClass )
//    gEmitterPTIClass.reset( new EmitterMenuePTIconClass );
//  return gEmitterPTIClass.get();
//}

void __stdcall RegisterSender()
{
    if ( s_regTPAEmitterCounter == 0 )
    {
        if ( !GetEmitterTPClass ()->IsRegistered () )
        {
            const TCHAR *env = _wgetenv ( L"OM_CADSHELL" );
            size_t ln = env != nullptr ? wcslen ( env ) : 0;
            if ( env && ln > 3 && env [3] == L'1' )
            {
                s_trace = true;
            }
            else
            {
                s_trace = false;
                s_traceCount = 0;
            }
            GetEmitterTPClass ()->Register ();
        }
        //if ( !GetEmitterGVIClass ()->IsRegistered () )
        //    GetEmitterGVIClass ()->Register ();

        //if ( !GetEmitterPTIClass ()->IsRegistered () )
        //    GetEmitterPTIClass ()->Register ();
    }

    s_regTPAEmitterCounter++;
}

void __stdcall RemoveSender()
{
    if ( s_regTPAEmitterCounter == 1 )
    {
        if ( gEmitterTPClass )
            if ( GetEmitterTPClass ()->IsRegistered () )
                GetEmitterTPClass ()->Unregister ();

        //if ( gEmitterGVIClass )
        //    if ( GetEmitterGVIClass ()->IsRegistered () )
        //        GetEmitterGVIClass ()->Unregister ();

        //if ( gEmitterPTIClass )
        //    if ( GetEmitterPTIClass ()->IsRegistered () )
        //        GetEmitterPTIClass ()->Unregister ();
    }

    if ( s_regTPAEmitterCounter > 0 )
        s_regTPAEmitterCounter--;
}

void __stdcall EmitTPSignal ( const wchar_t* name, int type, omsl::hcntcore::CadEntity* cadEntityPtr, int data, double f )
{
  if( gEmitterTPClass )
    GetEmitterTPClass()->Emit( name, type, cadEntityPtr, data, f );
}
//
//void __stdcall EmitGVIcon( int type )
//{
//  if( gEmitterGVIClass )
//    GetEmitterGVIClass()->Emit( type );
//}
//
//void __stdcall EmitPTIcon( int type )
//{
//  if( gEmitterPTIClass )
//    GetEmitterPTIClass()->Emit( type );
//}

EmitterTPSignalClass::EmitterTPSignalClass()
{
}

EmitterTPSignalClass::~EmitterTPSignalClass()
{
  if( IsRegistered() )
    Unregister();
}

omsl::boostwrapper::Connection* __stdcall EmitterTPSignalClass::Connect( TPA_Event::Type::Slot  subscriberMethod, void* subscriberObj )
{
  return omsl::boostwrapper::ConnectionWrapper::Create( m_Signal.connect( boost::bind( subscriberMethod, subscriberObj, _1, _2, _3, _4, _5 ) ) );
}

void EmitterTPSignalClass::Register()
{
  omsl::connection::AddSignalHelper< TPA_Event, EmitterTPSignalClass >( this, &EmitterTPSignalClass::Connect );

  m_registered = true;
}

void EmitterTPSignalClass::Unregister()
{
  m_Signal.disconnect_all_slots ();
  omsl::connection::RemoveSignalHelper< TPA_Event, EmitterTPSignalClass >( this, &EmitterTPSignalClass::Connect );
  m_registered = false;
}

void EmitterTPSignalClass::Emit( const wchar_t* name, int type, omsl::hcntcore::CadEntity* cadEntityPtr, int data, double f )
{
    if ( s_trace )
    {
        wchar_t t [150] = {L""};
        swprintf ( s_traceString[s_traceCount%1000], L"i=%4d : n = %40.40s : t = %3.3d : Ptr = %x : d = %10.10d, f = %12.6lf", s_traceCount, name, type, (long) (void*) cadEntityPtr, data, f );
        s_traceCount++;
    }

    m_Signal ( name, type, cadEntityPtr, data, f );
}

bool EmitterTPSignalClass::IsRegistered()
{
  return m_registered;
}

//
//EmitterMenueGVIconClass::EmitterMenueGVIconClass ()
//{}
//
//EmitterMenueGVIconClass::~EmitterMenueGVIconClass ()
//{
//    if ( IsRegistered () )
//        Unregister ();
//}
//
//omsl::boostwrapper::Connection* __stdcall EmitterMenueGVIconClass::Connect ( HCSMenueGVIcon::Type::Slot  subscriberMethod, void* subscriberObj )
//{
//    return omsl::boostwrapper::ConnectionWrapper::Create ( m_Signal.connect ( boost::bind ( subscriberMethod, subscriberObj, _1 ) ) );
//}
//
//void EmitterMenueGVIconClass::Register ()
//{
//    omsl::connection::AddSignalHelper< HCSMenueGVIcon, EmitterMenueGVIconClass > ( this, &EmitterMenueGVIconClass::Connect );
//
//    m_registered = true;
//}
//
//void EmitterMenueGVIconClass::Unregister ()
//{
//    m_Signal.disconnect_all_slots ();
//
//    omsl::connection::RemoveSignalHelper< HCSMenueGVIcon, EmitterMenueGVIconClass > ( this, &EmitterMenueGVIconClass::Connect );
//    m_registered = false;
//}
//
//void EmitterMenueGVIconClass::Emit ( int type )
//{
//    m_Signal ( type );
//}
//
//bool EmitterMenueGVIconClass::IsRegistered ()
//{
//    return m_registered;
//}
//
//
//EmitterMenuePTIconClass::EmitterMenuePTIconClass ()
//{}
//
//EmitterMenuePTIconClass::~EmitterMenuePTIconClass ()
//{
//    if ( IsRegistered () )
//        Unregister ();
//}
//
//omsl::boostwrapper::Connection* __stdcall EmitterMenuePTIconClass::Connect ( HCSMenuePTIcon::Type::Slot  subscriberMethod, void* subscriberObj )
//{
//    return omsl::boostwrapper::ConnectionWrapper::Create ( m_Signal.connect ( boost::bind ( subscriberMethod, subscriberObj, _1 ) ) );
//}
//
//void EmitterMenuePTIconClass::Register ()
//{
//    omsl::connection::AddSignalHelper< HCSMenuePTIcon, EmitterMenuePTIconClass > ( this, &EmitterMenuePTIconClass::Connect );
//
//    m_registered = true;
//}
//
//void EmitterMenuePTIconClass::Unregister ()
//{
//    m_Signal.disconnect_all_slots ();
//
//    omsl::connection::RemoveSignalHelper< HCSMenuePTIcon, EmitterMenuePTIconClass > ( this, &EmitterMenuePTIconClass::Connect );
//    m_registered = false;
//}
//
//void EmitterMenuePTIconClass::Emit ( int type )
//{
//    m_Signal ( type );
//}
//
//bool EmitterMenuePTIconClass::IsRegistered ()
//{
//    return m_registered;
//}

}
}