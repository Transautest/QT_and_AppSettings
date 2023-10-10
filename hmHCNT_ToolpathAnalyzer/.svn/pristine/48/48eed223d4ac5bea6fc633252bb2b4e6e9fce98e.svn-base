#pragma once
#if !defined( __listenerlsampl__H__)
#define __listenerlsampl__H__
#define OMSL_TYPEDEF_BOOST
#include <portability.h>
#include <composer/compositioncontaineraccessor.h>
#include <connection/connectort.h>

#include <connection\connectionhelper.h>
#include <connection\connectioncontainer.h>
#include <boostwrapper/connectionwrapper.h>
#include <boostwrapper/sharedconnection.h>
#include <progress\progressevent.h>
#include "../broadcast/hcntsignaltypes.h"

namespace omsl { 
extern "C" void *__stdcall OmslGetObject( omsl::composer::CompositionType, const wchar_t* name );
namespace hmbroadcast
{
    void __stdcall RemoveGuiListener ();
    void __stdcall RegisterGuiListener ();
    void __stdcall SetGuiToListener ( void *gui );
}}

#endif
