#include "stdafx.h"

// om
#include "utils/omRegDefines.h"
#include "utils/omRegUtils.h"

// om::ast
#include "omAstLanguageSettings.h"

using namespace om::ast;

///////////////////////////////////////////////////////////////////////////////
// om::ast::LanguageSettings

LanguageSettings::LanguageSettings()
{
}

bool LanguageSettings::Init(const om::tstring &version)
{
    om::tstring path = om::reg::GetCommonRegPath(om::reg::g_languagesKey, version.c_str());

    om::tstrings languages;

    REGSAM scope64 = 0;
    REGSAM scope32 = 0;
#ifdef _WIN64
    scope32 = KEY_WOW64_32KEY;
#else
    BOOL wow64Process = FALSE;
    if (   TRUE == ::IsWow64Process(GetCurrentProcess(), &wow64Process)
        && TRUE == wow64Process)
        scope64 = KEY_WOW64_64KEY;
#endif

    ATL::CRegKey key;
    LONG res = key.Open(HKEY_LOCAL_MACHINE, path.c_str(), KEY_QUERY_VALUE | scope64);
    if (ERROR_SUCCESS != res)
        res = key.Open(HKEY_LOCAL_MACHINE, path.c_str(), KEY_QUERY_VALUE | scope32);
    if (ERROR_SUCCESS == res)
    {
        // RegEnumValue also receives the size of the buffer inc. EOS.
        // Flawfinder: ignore
        TCHAR buffer[1024];
        DWORD idx = 0;
        DWORD len = 1024;
        DWORD type = 0;
        while (ERROR_SUCCESS == ::RegEnumValue(key, idx, buffer, &len, NULL, &type, NULL, NULL))
        {
            if (   len > 0
                && len < 1024
                && type == REG_SZ)
            {
                om::tstring lang;
                lang.assign(buffer, len);
                languages.push_back(lang);
            }

            len = 1024;
            ++idx;
        }

        key.Close();
    }

    path = om::reg::GetCommonRegPath(om::reg::g_configurationKey, version.c_str());

    om::tstring machineLanguage;
    res = key.Open(HKEY_LOCAL_MACHINE, path.c_str(), KEY_QUERY_VALUE | scope64);
    if (ERROR_SUCCESS != res)
        res = key.Open(HKEY_LOCAL_MACHINE, path.c_str(), KEY_QUERY_VALUE | scope32);
    if (ERROR_SUCCESS == res)
        om::reg::utils::QueryString(key, om::reg::g_languageValue, machineLanguage);
    key.Close();

    om::tstring userLanguage;
    res = key.Open(HKEY_CURRENT_USER, path.c_str(), KEY_QUERY_VALUE);
    if (ERROR_SUCCESS == res)
        om::reg::utils::QueryString(key, om::reg::g_languageValue, userLanguage);
    key.Close();

    m_userLanguage.swap(userLanguage);
    m_machineLanguage.swap(machineLanguage);
    m_availableLanguages.swap(languages);

    return true;
}

bool LanguageSettings::Save(const om::tstring &version)
{
    om::tstring path = om::reg::GetCommonRegPath(om::reg::g_configurationKey, version.c_str());

    ATL::CRegKey key;
    LONG res = key.Create(HKEY_CURRENT_USER, path.c_str(), NULL, 0, KEY_WRITE);
    if (ERROR_SUCCESS != res)
        return false;
    if (m_userLanguage.empty())
    {
        key.DeleteValue(om::reg::g_languageValue);
    }
    else
    {
        res = key.SetStringValue(om::reg::g_languageValue, m_userLanguage.c_str());
        if (ERROR_SUCCESS != res)
            return false;
    }

    key.Close();

    if (false == m_machineLanguage.empty())
    {
        REGSAM scope64 = 0;
        REGSAM scope32 = 0;
#ifdef _WIN64
        scope32 = KEY_WOW64_32KEY;
#else
        BOOL wow64Process = FALSE;
        if (   TRUE == ::IsWow64Process(GetCurrentProcess(), &wow64Process)
            && TRUE == wow64Process)
            scope64 = KEY_WOW64_64KEY;
#endif

        om::tstring machineLanguage;
        res = key.Open(HKEY_LOCAL_MACHINE, path.c_str(), KEY_QUERY_VALUE | scope64);
        if (ERROR_SUCCESS != res)
            res = key.Open(HKEY_LOCAL_MACHINE, path.c_str(), KEY_QUERY_VALUE | scope32);
        if (ERROR_SUCCESS == res)
            om::reg::utils::QueryString(key, om::reg::g_languageValue, machineLanguage);
        key.Close();

        if (machineLanguage != m_machineLanguage)
        {
            res = key.Create(HKEY_LOCAL_MACHINE, path.c_str(), NULL, 0, KEY_WRITE | scope64);
            if (ERROR_SUCCESS != res)
                return false;

            res = key.SetStringValue(om::reg::g_languageValue, m_machineLanguage.c_str());
            if (ERROR_SUCCESS != res)
                return false;

            key.Close();

            res = key.Create(HKEY_LOCAL_MACHINE, path.c_str(), NULL, 0, KEY_WRITE | scope32);
            if (ERROR_SUCCESS != res)
                return false;

            res = key.SetStringValue(om::reg::g_languageValue, m_machineLanguage.c_str());
            if (ERROR_SUCCESS != res)
                return false;
        }
    }

    return true;
}
