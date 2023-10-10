#pragma once
#include <boostwrapper\connection.h>
#include <boostwrapper\connectionwrapper.h>
#include <connection\connectionhelper.h>

#include <hcntcore\cadentity.h>

namespace omsl
{
namespace hmbroadcast
{
//    events for menue button toggle
DECLARE_EVENT_1 ( hmbroadcast, HCSMenue, int );
DECLARE_EVENT_1 ( hmbroadcast, HCSMenueGVIcon, int );
DECLARE_EVENT_1 ( hmbroadcast, HCSMenuePTIcon, int );

// events for toolpath handling
DECLARE_EVENT_5 ( hmbroadcast, TPA_Event, const wchar_t*, int, omsl::hcntcore::CadEntity*, int, double );
DECLARE_EVENT_4 ( hmbroadcast, HMCS_Event, const wchar_t*, int, omsl::hcntcore::CadEntity*, int );
DECLARE_EVENT_4 ( hmbroadcast, TTIP_Event, const wchar_t*, int, const wchar_t*, int );
}}
