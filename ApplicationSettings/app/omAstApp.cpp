#include "stdafx.h"

// std
#include <sstream>

// boost
#include "boost/cstdint.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"

// om
#include "../../../version.h"
#include "utils/omTStrings.h"
#include "utils/omTStringUtils.h"
#include "utils/omLocale.h"
#include "utils/omProducts.h"
#include "utils/omFileUtils.h"
#include "utils/omMessageLog.h"
#include "utils/omRegDefines.h"
#include "utils/omVersionInfo.h"

// om::ui
#include "ui/atl/omUiAtlResUtils.h"

#include "../appres/omAppSettingsRes.h"
#include "omAstMessageLogObserver.h"
#include "omAstApp.h"

using namespace om::ast;

namespace {

    const TCHAR g_locFileName[] = _T(/*MSG0*/"omAppSettings.loc");
    const TCHAR g_defaultAppTitle[] = _T(/*MSG0*/"OPEN MIND Application Settings");

} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////
// om::ast::App

App::App() :
    m_parentWnd(NULL)
{
}

App::~App()
{
}

App &App::GetSingleton()
{
    static App app;
    return app;
}

bool App::Init(LPCTSTR cmdLine)
{
    om::InitClassicLocale();
    om::InitNoGroupingSystemLocale();

    // set from "C" to operating system locale
    // -> Don't use setlocale function. This function
    //    doesn't set the global std::locale! But
    //    std::locale::global uses setlocale. Thus
    //    using std::locale::global we get both at
    //    once.
    std::locale::global(std::locale(om::g_systemLocaleName));

    m_pMessageLogObserver.reset(new MessageLogObserver);

    ParseCmdLine(cmdLine);

    if (false == m_productInfo.Init(
        om::g_productInfoCenter,
        OM_MAJOR_VERSION,
        OM_MINOR_VERSION))
    {
        OM_ASSERT(0);
        return false;
    }

    m_appSectionPath = m_productInfo.GetRegistrySubPath();
    m_appSectionPath += om::reg::g_delimiter;
    m_appSectionPath += om::reg::g_uiKey;

    if (m_version.empty())
        m_version = m_productInfo.GetVersionString();

    if (false == InitResources())
        return false;

    InitAppTitle();

    return true;
}

bool App::Exit()
{
    return true;
}

om::tstring App::GetAppSectionPath(LPCTSTR subPath /*= NULL*/) const
{
    om::tstring path = m_appSectionPath;
    if (false == om::empty(subPath))
    {
        path += om::reg::g_delimiter;
        path += subPath;
    }

    return path;
}

om::tstring App::GetFileDialogRegPath() const
{
    om::tstring path = GetAppSectionPath(om::reg::g_uiKey);
    if (false == path.empty())
    {
        path += om::reg::g_delimiter;
        path += _T(/*MSG0*/"FileDialogs");
    }

    return path;
}

void App::InitAppTitle()
{
    om::tstring title;
    om::LoadString(title, IDS_AST_APP_NAME);
    if (title.empty())
        title = g_defaultAppTitle;

    title += _T(/*MSG0*/" ");
    title += OM_VERSION_NAME;
    
    LPCTSTR sp = m_productInfo.GetServicePack();
    if (false == om::empty(sp))
    {
        title += _T(/*MSG0*/" | ");
        title += sp;
    }
    
    if (false == m_appTitleExt.empty())
        title += m_appTitleExt;

    m_appTitle.swap(title);
}

bool App::InitResources()
{
    if (false == m_productInfo.Initialized())
    {
        OM_ASSERT(0);
        return false;
    }

    om::tstring resFile = m_productInfo.GetProductPath();
    if (   resFile.empty()
        || false == om::fileutils::AppendToPath(resFile, m_productInfo.GetLanguage())
        || false == om::fileutils::AppendToPath(resFile, g_locFileName))
    {
        OM_ASSERT(0);
        return false;
    }

    HINSTANCE hInstRes = NULL;
    // Path is absolute. Therefore the right DLL should be loaded.
    // Flawfinder: ignore
    if (NULL == (hInstRes = ::LoadLibrary(resFile.c_str())))
    {
        DWORD lastError = GetLastError();

        om::tstring msg = _T(/*MSG0*/"Failed to load resource file.\r\n");
        msg += resFile.c_str();

        om::GetMessageLog().LogWinError(lastError, msg.c_str());

        return false;
    }

    // Prevent BETA testers from using the wrong resources
    //
    // But don't perform this check in end-user builds!
    // This check in end-user build would force us to
    // update all language files every time we change
    // the english resource files, even if these are only
    // spelling corrections or new features with special
    // guarding code inside the executable.
    //
#ifdef OM_BETA

    if (0 != _tcscmp(m_productInfo.GetLanguage(), om::g_defaultLanguage))
    {
        om::tstring defaultResFile = om::g_defaultLanguage;
        if (   false == om::fileutils::MakePathAbsolute(defaultResFile)
            || false == om::fileutils::AppendToPath(defaultResFile, g_locFileName))
        {
            OM_ASSERT(0);
            return false;
        }

        if (-1 == ::GetFileAttributes(defaultResFile.c_str()))
        {
            om::tstring msg;
            om::FormatString(
                msg,
                _T(/*MSG0*/"Missing default loc file:\r\n")
                _T(/*MSG0*/"   %1\r\n")
                _T(/*MSG0*/"\r\nCannot perform revision validation. Resources might be outdated!")
                _T(/*MSG0*/"\r\n\r\nDo you like to continue?"),
                defaultResFile.c_str());

            int res = ::MessageBox(NULL, msg.c_str(),
                g_defaultAppTitle, MB_YESNO | MB_ICONERROR);
            if (IDNO == res)
            {
                ::FreeLibrary(hInstRes);
                return false;
            }
        }

        om::VersionInfo defaultVersionInfo(defaultResFile.c_str());
        unsigned expectedVersion = om::GetRevision(defaultVersionInfo.GetFixedFileVersion());
        if (0 == expectedVersion)
        {
            om::GetMessageLog().Log(
                om::MessageLog::kError,
                _T(/*MSG0*/"Invalid default LOC-file")
                _T(/*MSG0*/"\r\nFailed to retrieve version info."));
            ::FreeLibrary(hInstRes);
            return false;
        }

        om::VersionInfo versionInfo(resFile.c_str());
        unsigned foundVersion = om::GetRevision(versionInfo.GetFixedFileVersion());
        if (0 == foundVersion)
        {
            om::GetMessageLog().Log(
                om::MessageLog::kError,
                _T(/*MSG0*/"Invalid LOC-file")
                _T(/*MSG0*/"\r\nFailed to retrieve version info."));
            ::FreeLibrary(hInstRes);
            return false;
        }

        if (foundVersion != expectedVersion)
        {
            m_appTitleExt = _T(/*MSG0*/" - wrong loc file (");
            m_appTitleExt += boost::lexical_cast<om::tstring>(expectedVersion);
            m_appTitleExt += _T(/*MSG0*/'/');
            m_appTitleExt += boost::lexical_cast<om::tstring>(foundVersion);
            m_appTitleExt += _T(/*MSG0*/')');

            if (false == m_appTitle.empty())
                m_appTitle += m_appTitleExt;
        }
    }

#endif // OM_BETA

    om::ui::atl::SetResourceInstance(hInstRes);

    return true;
}

void App::ParseCmdLine(LPCTSTR cmdLine)
{
    if (om::empty(cmdLine))
        return;

    om::tstring tmp(cmdLine);
    om::tstrings args;
    boost::algorithm::split(args, tmp, boost::algorithm::is_any_of(_T(/*MSG0*/" ")), boost::algorithm::token_compress_on);

    om::tstrings::iterator iter = args.begin();
    om::tstrings::iterator iterEnd = args.end();
    while (iter != iterEnd)
    {
        if (*iter == _T(/*MSG0*/"-v"))
        {
            ++iter;
            if (iter == iterEnd)
                break;
            m_version = *iter;
        }
        else if (*iter == _T(/*MSG0*/"-w"))
        {
            ++iter;
            if (iter == iterEnd)
                break;

            boost::uint64_t w = 0;
            if (0 == _tcsnicmp((*iter).c_str(), _T(/*MSG0*/"0x"), 2))
            {
                std::basic_istringstream<TCHAR> ostr((*iter).c_str() + 2);
                ostr >> std::hex >> w;
            }
            else
            {
                std::basic_istringstream<TCHAR> ostr(*iter);
                ostr >> w;
            }

            m_parentWnd = reinterpret_cast<HWND>(w);
            if (FALSE == ::IsWindow(m_parentWnd))
                m_parentWnd = NULL;
        }

        ++iter;
    }
}
