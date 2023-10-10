#include "stdafx.h"

// om::ast
#include "omAstUtils.h"

using namespace om::ast;

///////////////////////////////////////////////////////////////////////////////
// om::ast

SettingsType om::ast::GetSettingsType()
{
    SettingsType type = kUserSettings;

    ATL::CRegKey key;
    if (ERROR_SUCCESS == key.Create(HKEY_LOCAL_MACHINE, _T(/*MSG0*/""), NULL, 0, KEY_READ | KEY_WRITE))
        type = kMachineSettings;

    return type;
}
