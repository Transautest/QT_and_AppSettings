#include "stdafx.h"

// boost
#include "boost/lexical_cast.hpp"

// om::ast
#include "omAstApp.h"
#include "omAstResizable.h"

namespace {

    const TCHAR g_baseLayoutRegKey[] = _T(/*MSG0*/"Dialog_");

} // anonymous namespace om

using namespace om::ast;

///////////////////////////////////////////////////////////////////////////////
// om::ast

bool om::ast::GetLayoutIORegistryKeyImpl(
    ATL::CRegKey &key,
    ATL::CWindow & /*wnd*/,
    LPCTSTR /*regPath*/,
    UINT templateId)
{
    if (0 == templateId)
        return false;

    ATL::CRegKey baseKey;
    LONG res = baseKey.Create(HKEY_CURRENT_USER,
        GetApp().GetAppSectionPath(_T(/*MSG0*/"Dialogs")).c_str());
    if (ERROR_SUCCESS != res)
        return false;

    om::tstring section = g_baseLayoutRegKey;
    section += boost::lexical_cast<om::tstring>(templateId);

    res = key.Create(baseKey, section.c_str());
    return ERROR_SUCCESS == res;
}
