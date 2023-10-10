#include "stdafx.h"

// boost
#include "boost/algorithm/string/case_conv.hpp"
#include "boost/algorithm/string/trim.hpp"

// om::ast
#include "omAstRegisteredApplication.h"

using namespace om::ast;

namespace {

    void SplitCall(
        const om::tstring &call,
        om::tstring &executable,
        om::tstring &args);

} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////
// om::ast::RegisteredApplication

RegisteredApplication::RegisteredApplication(
    SettingsType settingsType,
    ConfigurationType configurationType,
    const om::tstring &key,
    const om::tstring &name,
    const om::tstring &executable,
    const om::tstring &arguments) :
    m_settingsType(settingsType),
    m_configurationType(configurationType),
    m_key(key),
    m_name(name),
    m_executable(executable),
    m_arguments(arguments)
{
}

RegisteredApplication::RegisteredApplication(
    SettingsType settingsType,
    ConfigurationType configurationType,
    const om::tstring &key,
    const om::tstring &name,
    const om::tstring &call) :
    m_settingsType(settingsType),
    m_configurationType(configurationType),
    m_key(key),
    m_name(name)
{
    SplitCall(call, m_executable, m_arguments);
}

RegisteredApplication::RegisteredApplication(const RegisteredApplication &other) :
    m_settingsType(other.m_settingsType),
    m_configurationType(other.m_configurationType),
    m_key(other.m_key),
    m_name(other.m_name),
    m_executable(other.m_executable),
    m_arguments(other.m_arguments)
{
}

RegisteredApplication::~RegisteredApplication()
{
}

RegisteredApplication &RegisteredApplication::operator=(const RegisteredApplication &other)
{
    RegisteredApplication tmp(other);
    Swap(tmp);
    return *this;
}

void RegisteredApplication::Swap(RegisteredApplication &other)
{
    std::swap(m_settingsType, other.m_settingsType);
    std::swap(m_configurationType, other.m_configurationType);
    m_key.swap(other.m_key);
    m_name.swap(other.m_name);
    m_executable.swap(other.m_executable);
    m_arguments.swap(other.m_arguments);
    m_icon.Swap(other.m_icon);
}

bool RegisteredApplication::operator<(const RegisteredApplication &other) const
{
    // 1. sort by name
    // 2. sort by configuration - current version first
    // 3. sort by setting - user setting first

    int res = CompareRegisteredApplicationKey(m_key.c_str(), other.m_key.c_str());
    if (res < 0)
        return true;
    else if (res > 0)
        return false;

    if (m_configurationType != other.m_configurationType)
        return kCurrentVersionConfiguration == m_configurationType;

    if (m_settingsType != other.m_settingsType)
        return kUserSettings == other.m_settingsType;

    return false; // equal form our point of view
}

bool RegisteredApplication::operator==(const RegisteredApplication &other) const
{
    return    0 == CompareRegisteredApplicationKey(m_key.c_str(), other.m_key.c_str())
           && m_configurationType != other.m_configurationType
           && m_settingsType != other.m_settingsType;
}

HICON RegisteredApplication::GetIcon() const
{
    if (m_icon.IsNull())
    {
        // We check size of string we copy to buffer.
        // Flawfinder: ignore
        TCHAR buffer[MAX_PATH];
        if (m_executable.size() < MAX_PATH)
        {
            // We check size of string we copy to buffer above.
            // Flawfinder: ignore
            _tcscpy(buffer, m_executable.c_str());
            WORD iconIdx = 0;
            m_icon.Attach(::ExtractAssociatedIcon(_Module.get_m_hInst(), buffer, &iconIdx));
        }
    }

    return m_icon.Get();
}

om::tstring RegisteredApplication::GetCall() const
{
    om::tstring call;
    if (false == m_arguments.empty())
    {
        call = _T(/*MSG0*/"\"");
        call += m_executable;
        call += _T(/*MSG0*/"\"");
        call += _T(/*MSG0*/" ");
        call += m_arguments;
    }
    else
        call = m_executable;    // no "" -> SCR 43999

    return call;
}

///////////////////////////////////////////////////////////////////////////////
// *unique*

namespace {

    void SplitCall(
        const om::tstring &call,
        om::tstring &executable,
        om::tstring &args)
    {
        LPCTSTR cur = call.c_str();
        while (_istspace(*cur))
            cur = _tcsinc(cur);

        if (*cur == _T(/*MSG0*/'\"'))
        {
            ++cur;
            LPCTSTR end = _tcschr(cur, _T(/*MSG0*/'\"'));
            if (NULL == end)
            {
                executable = cur;
                // Passed string is based on std::basic_string - EOS always present.
                // Flawfinder: ignore
                cur += _tcslen(cur);
            }
            else
            {
                executable.assign(cur, end - cur);
                cur = end;
                ++cur;
            }
        }
        else
        {
            om::tstring tmp(call);
            boost::algorithm::to_lower(tmp);

            LPCTSTR end = _tcsstr(tmp.c_str(), _T(/*MSG0*/".exe"));
            if (NULL == end)
            {
                executable = cur;
                // Passed string is based on std::basic_string - EOS always present.
                // Flawfinder: ignore
                cur += _tcslen(cur);
            }
            else
            {
                end += 4;
                size_t len = end - tmp.c_str();
                executable.assign(cur, len);
                cur += len;
            }
        }

        args = cur;
        boost::algorithm::trim(args);
    }

} // anonymous namespace
