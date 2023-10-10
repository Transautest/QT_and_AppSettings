#pragma once
#ifndef __TP_ANALYZER_H__
#define __TP_ANALYZER_H__

#include <command/handshaking.h>
#include <smartpointers.h>

#include <composer/compositioncontainerinstantiation.h>
#include <composer/compositionexportdefine.h>

#if defined(TP_ANALYZER_EXPORTS)
#define TP_ANALYZER_API_DIRECT                   OMSL_EXPORT_DIRECT
#define TP_ANALYZER_API_DIRECT_wDEFAULTS         OMSL_EXPORT_DIRECT_wDEFAULTS
#define TP_ANALYZER_API_DIRECT_SHARED            OMSL_EXPORT_DIRECT_SHARED
#define TP_ANALYZER_API_DIRECT_SHARED_wDEFAULTS  OMSL_EXPORT_DIRECT_SHARED_wDEFAULTS
#define TP_ANALYZER_API_STATIC                   OMSL_EXPORT_STATIC_MEMBER
#define TP_ANALYZER_API_STATIC_SHARED            OMSL_EXPORT_STATIC_MEMBER_SHARED
#define TP_ANALYZER_API_STATIC_SHARED_wDEFAULTS  OMSL_EXPORT_STATIC_MEMBER_SHARED_wDEFAULTS
#else
#define TP_ANALYZER_API_DIRECT                   OMSL_IMPORT_DIRECT
#define TP_ANALYZER_API_DIRECT_wDEFAULTS         OMSL_IMPORT_DIRECT_wDEFAULTS
#define TP_ANALYZER_API_DIRECT_SHARED            OMSL_IMPORT_DIRECT_SHARED
#define TP_ANALYZER_API_DIRECT_SHARED_wDEFAULTS  OMSL_IMPORT_DIRECT_SHARED_wDEFAULTS
#define TP_ANALYZER_API_STATIC                   OMSL_IMPORT_STATIC_MEMBER
#define TP_ANALYZER_API_STATIC_SHARED            OMSL_IMPORT_STATIC_MEMBER_SHARED
#define TP_ANALYZER_API_STATIC_SHARED_wDEFAULTS  OMSL_IMPORT_STATIC_MEMBER_SHARED_wDEFAULTS
#endif

#ifndef DOXYGEN_IGNORE_THIS
namespace omsl { namespace commands {namespace tp_analyzer{ inline const wchar_t* OmslGetDllName() { return L"tp_analyzer"; } } }}
#endif

#endif //__TP_ANALYZER_H__
