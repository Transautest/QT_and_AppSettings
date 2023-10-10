// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0501
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0600
#define _RICHEDIT_VER	0x0300

// ATL
#include <atlbase.h>
#include <atlstr.h>
#include <atltypes.h>
#include <atlwin.h>

// WTL

#define _WTL_NO_WTYPES

#include "atlapp.h"
#include "atlframe.h"
#include "atlctrls.h"
#include "atldlgs.h"
#include "atlctrlw.h"
#include "atlcrack.h"
#include "atlgdi.h"
#include "atlmisc.h"
#include "atlctrlx.h"

extern WTL::CAppModule _Module;
