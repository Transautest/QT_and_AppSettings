#include "stdafx.h"

// om
#include "utils/omRegDefines.h"
#include "utils/omRegUtils.h"
#include "utils/omProducts.h"

// om::ast
#include "omAstApplicationSettings.h"

using namespace om::ast;

namespace {

    inline om::tstring GetCurrentVersionRegPath(const om::tstring &version)
    {
        om::tstring path = om::reg::GetProductRegPath(om::g_productInfoCenter, version.c_str());
        path += om::reg::g_delimiterCharacter;
        path += om::reg::g_applicationsKey;

        return path;
    }

    bool InitRegisteredApplications(
        const om::tstring &path,
        SettingsType settingsType,
        ConfigurationType configurationType,
        RegisteredApplications &apps);

    om::ast::RegisteredApplicationPtr CreateApp(
        HKEY parent,
        LPCTSTR subKey,
        SettingsType settingsType,
        ConfigurationType configurationType,
        REGSAM scope = 0);

    bool SaveRegisteredApplications(
        const om::tstring &path,
        SettingsType settingsType,
        ConfigurationType configurationType,
        const RegisteredApplications &apps);

    bool SaveRegisteredApplication(
        HKEY parent,
        const RegisteredApplication &app,
        REGSAM scope = 0);

} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////
// om::ast::ApplicationSettings

ApplicationSettings::ApplicationSettings() :
    m_currentVersionUserAppsChanged(false),
    m_currentVersionMachineAppsChanged(false),
    m_commonUserAppsChanged(false),
    m_commonMachineAppsChanged(false)
{
}

ApplicationSettings::ApplicationSettings(const ApplicationSettings &other) :
    m_apps(other.m_apps),
    m_currentVersionUserAppsChanged(other.m_currentVersionUserAppsChanged),
    m_currentVersionMachineAppsChanged(other.m_currentVersionMachineAppsChanged),
    m_commonUserAppsChanged(other.m_commonUserAppsChanged),
    m_commonMachineAppsChanged(other.m_commonMachineAppsChanged)
{
}

ApplicationSettings::~ApplicationSettings()
{
}

ApplicationSettings &ApplicationSettings::operator=(const ApplicationSettings &other)
{
    ApplicationSettings tmp(other);
    swap(tmp);
    return *this;
}

void ApplicationSettings::swap(ApplicationSettings &other)
{
    m_apps.swap(other.m_apps);
    std::swap(m_currentVersionUserAppsChanged, other.m_currentVersionUserAppsChanged);
    std::swap(m_currentVersionMachineAppsChanged, other.m_currentVersionMachineAppsChanged);
    std::swap(m_commonUserAppsChanged, other.m_commonUserAppsChanged);
    std::swap(m_commonMachineAppsChanged, other.m_commonMachineAppsChanged);
}

bool ApplicationSettings::Init(const om::tstring &version)
{
    om::tstring path = GetCurrentVersionRegPath(version);

    RegisteredApplications apps;
    if (false == InitRegisteredApplications(path, kUserSettings, kCurrentVersionConfiguration, apps))
        return false;

    RegisteredApplications tmp;
    if (false == InitRegisteredApplications(path, kMachineSettings, kCurrentVersionConfiguration, tmp))
        return false;
    apps.insert(apps.end(), tmp.begin(), tmp.end());

    path = om::reg::GetCommonRegPath(om::reg::g_applicationsKey);
    
    tmp.clear();
    if (false == InitRegisteredApplications(path, kUserSettings, kCommonConfiguration, tmp))
        return false;
    apps.insert(apps.end(), tmp.begin(), tmp.end());
    
    tmp.clear();
    if (false == InitRegisteredApplications(path, kMachineSettings, kCommonConfiguration, tmp))
        return false;
    apps.insert(apps.end(), tmp.begin(), tmp.end());

    std::sort(apps.begin(), apps.end(), LessRegisteredApplication());

    m_apps.swap(apps);
    m_currentVersionUserAppsChanged = false;
    m_currentVersionMachineAppsChanged = false;
    m_commonUserAppsChanged = false;
    m_commonMachineAppsChanged = false;

    return true;
}

bool ApplicationSettings::Save(const om::tstring &version)
{
    om::tstring currentVersionPath = GetCurrentVersionRegPath(version);
    om::tstring commonPath = om::reg::GetCommonRegPath(om::reg::g_applicationsKey);

    if (true == m_currentVersionUserAppsChanged)
    {
        if (false == SaveRegisteredApplications(currentVersionPath, kUserSettings, kCurrentVersionConfiguration, m_apps))
            return false;

        m_currentVersionUserAppsChanged = false;
    }

    if (true == m_commonUserAppsChanged)
    {
        if (false == SaveRegisteredApplications(commonPath, kUserSettings, kCommonConfiguration, m_apps))
            return false;

        m_commonUserAppsChanged = false;
    }

    if (true == m_currentVersionMachineAppsChanged)
    {
        if (false == SaveRegisteredApplications(currentVersionPath, kMachineSettings, kCurrentVersionConfiguration, m_apps))
            return false;

        m_currentVersionMachineAppsChanged = false;
    }

    if (true == m_commonMachineAppsChanged)
    {
        if (false == SaveRegisteredApplications(commonPath, kMachineSettings, kCommonConfiguration, m_apps))
            return false;

        m_commonMachineAppsChanged = false;
    }

    return true;
}

bool ApplicationSettings::SetApp(RegisteredApplicationPtr pApp)
{
    if (NULL == pApp.get())
        return false;

    RegisteredApplications::iterator iter = std::lower_bound(
        m_apps.begin(), m_apps.end(), pApp, LessRegisteredApplication());
    if (   iter != m_apps.end()
        && **iter == *pApp)
    {
        *iter = pApp;
    }
    else
    {
        iter = m_apps.insert(iter, pApp);
        if (iter == m_apps.end())
            return false;
    }

    UpdateChangedFlags(*pApp);

    return true;
}

bool ApplicationSettings::EraseApp(RegisteredApplicationPtr pApp)
{
    RegisteredApplications::iterator iter = std::find(m_apps.begin(), m_apps.end(), pApp);
    if (iter == m_apps.end())
        return false;

    m_apps.erase(iter);

    UpdateChangedFlags(*pApp);

    return true;
}

RegisteredApplicationPtr ApplicationSettings::FindApp(
    LPCTSTR key,
    SettingsType type /*= kUnknownSettingsType*/) const
{
    RegisteredApplicationPtr pApp;

    if (false == om::empty(key))
    {
        RegisteredApplications::const_iterator iter = std::lower_bound(
            m_apps.begin(), m_apps.end(), key, LessRegisteredApplicationByKey());
        for (; iter != m_apps.end(); ++iter)
        {
            if (0 != CompareRegisteredApplicationKey(key, (*iter)->GetKey().c_str()))
                break;

            if (   kUnknownSettingsType == type
                || (*iter)->GetSettingsType() == type)
            {
                pApp = *iter;
                break;
            }
        }
    }

    return pApp;
}

void ApplicationSettings::GetUserRelevantApps(RegisteredApplications &apps) const
{
    apps.reserve(m_apps.size());

    om::tstring key;
    for (RegisteredApplicationPtr pApp : m_apps)
    {
        if (0 != CompareRegisteredApplicationKey(pApp->GetKey().c_str(), key.c_str()))
        {
            apps.push_back(pApp);
            key = pApp->GetKey();
        }
    }
}

void ApplicationSettings::GetMachineRelevantApps(RegisteredApplications &apps) const
{
    apps.reserve(m_apps.size());

    om::tstring key;
    for (RegisteredApplicationPtr pApp : m_apps)
    {
        if (   kMachineSettings == pApp->GetSettingsType()
            && 0 != CompareRegisteredApplicationKey(pApp->GetKey().c_str(), key.c_str()))
        {
            apps.push_back(pApp);
            key = pApp->GetKey();
        }
    }
}

void ApplicationSettings::UpdateChangedFlags(RegisteredApplication &app)
{
    if (kUserSettings == app.GetSettingsType())
    {
        if (kCurrentVersionConfiguration == app.GetConfigurationType())
            m_currentVersionUserAppsChanged = true;
        else
            m_commonUserAppsChanged = true;
    }
    else
    {
        if (kCurrentVersionConfiguration == app.GetConfigurationType())
            m_currentVersionMachineAppsChanged = true;
        else
            m_commonMachineAppsChanged = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
// *unique*

namespace {

    bool InitRegisteredApplications(
        const om::tstring &path,
        SettingsType settingsType,
        ConfigurationType configurationType,
        RegisteredApplications &apps)
    {
        size_t loops = 1;
        HKEY parent = HKEY_CURRENT_USER;
        REGSAM scope[2] = { 0, 0 };

        if (kMachineSettings == settingsType)
        {
            parent = HKEY_LOCAL_MACHINE;
            // enumerate noth hives: 64bit and 32bit
#ifdef _WIN64
            loops = 2;
            scope[1] |= KEY_WOW64_32KEY;
#else
            BOOL wow64Process = FALSE;
            if (   TRUE == ::IsWow64Process(GetCurrentProcess(), &wow64Process)
                && TRUE == wow64Process)
            {
                loops = 2;
                scope[0] |= KEY_WOW64_64KEY;
            }
            else
                loops = 1;
#endif
        }
        else if (kUserSettings != settingsType)
            return false;

        apps.clear();

        for (size_t loop = 0; loop < loops; ++loop)
        {
            ATL::CRegKey key;
            LONG res = key.Open(parent, path.c_str(), KEY_ENUMERATE_SUB_KEYS | scope[loop]);
            if (ERROR_SUCCESS != res)
                return true;   // key not available - nothing to do

            DWORD idx = 0;
            // EnumKey also receives the size of the buffer inc. EOS.
            // Other parts of the code use name as zero terminated string.
            // Flawfinder: ignore
            TCHAR name[1024];
            DWORD len = 1024;
            while (ERROR_SUCCESS == key.EnumKey(idx, name, &len))
            {
                if (   len > 0
                    && len < 1024
                    && (   loop == 0
                        || apps.end() == std::find_if(apps.begin(), apps.end(), FindRegisteredApplicationByKey(name))))
                {
                    RegisteredApplicationPtr pApp = CreateApp(key, name, settingsType, configurationType, scope[loop]);
                    if (NULL != pApp.get())
                        apps.push_back(pApp);
                }

                ++idx;
                len = 1024;
            }
        }

        return true;
    }

    om::ast::RegisteredApplicationPtr CreateApp(
        HKEY parent,
        LPCTSTR subKey,
        SettingsType settingsType,
        ConfigurationType configurationType,
        REGSAM scope /*= 0*/)
    {
        om::ast::RegisteredApplicationPtr pApp;

        if (   NULL != parent
            || NULL != subKey)
        {
            ATL::CRegKey appKey;
            LONG res = appKey.Open(parent, subKey, KEY_QUERY_VALUE | scope);
            if (ERROR_SUCCESS == res)
            {
                om::tstring pathName;
                if (true == om::reg::utils::QueryString(appKey, om::reg::g_pathValue, pathName))
                {
                    om::tstring name;
                    om::reg::utils::QueryString(appKey, _T(/*MSG0*/""), name);

                    pApp.reset(new RegisteredApplication(settingsType, configurationType, subKey, name, pathName));
                }
            }
        }

        return pApp;
    }

    bool SaveRegisteredApplications(
        const om::tstring &path,
        om::ast::SettingsType settingsType,
        ConfigurationType configurationType,
        const om::ast::RegisteredApplications &apps)
    {
        size_t loops = 1;
        REGSAM scope[2] = { 0, 0 };
        HKEY parent = HKEY_CURRENT_USER;
        if (kMachineSettings == settingsType)
        {
            parent = HKEY_LOCAL_MACHINE;
#ifdef _WIN64
            loops = 2;
            scope[1] |= KEY_WOW64_32KEY;
#else
            BOOL wow64Process = FALSE;
            if (   TRUE == ::IsWow64Process(GetCurrentProcess(), &wow64Process)
                && TRUE == wow64Process)
            {
                loops = 2;
                scope[0] |= KEY_WOW64_64KEY;
            }
            else
                loops = 1;
#endif
        }
        else if (kUserSettings != settingsType)
            return false;

        for (size_t loop = 0; loop < loops; ++loop)
        {
            ATL::CRegKey key;
            LONG res = key.Create(parent, path.c_str(), NULL, 0, KEY_READ | KEY_WRITE | scope[loop]);
            if (ERROR_SUCCESS != res)
                return false;

            om::reg::utils::ClearKeyContents(key, scope[loop]);

            for (RegisteredApplicationPtr pApp : apps)
            {
                if (   pApp->GetSettingsType() == settingsType
                    && pApp->GetConfigurationType() == configurationType)
                    SaveRegisteredApplication(key, *pApp, scope[loop]);
            }
        }

        return true;
    }

    bool SaveRegisteredApplication(
        HKEY parent,
        const om::ast::RegisteredApplication &app,
        REGSAM scope /*= 0*/)
    {
        if (NULL == parent)
            return false;

        ATL::CRegKey appKey;
        LONG res = appKey.Create(parent, app.GetKey().c_str(), NULL, 0, KEY_SET_VALUE | scope);
        if (ERROR_SUCCESS != res)
            return false;

        if (false == app.GetName().empty())
            appKey.SetStringValue(_T(/*MSG0*/""), app.GetName().c_str());

        om::tstring call = app.GetCall();
        res = appKey.SetStringValue(om::reg::g_pathValue, call.c_str());
        return ERROR_SUCCESS == res;
    }

} // anonymous namespace
