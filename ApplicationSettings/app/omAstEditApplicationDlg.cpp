#include "stdafx.h"

// std
#include <fstream>

// om
#include "utils/omTStrings.h"
#include "utils/omFileUtils.h"
#include "utils/omTStringUtils.h"
#include "utils/omMessageLog.h"
#include "utils/omProducts.h"

// om::ui
#include "ui/atl/omUiAtlInitCtrl.h"
#include "ui/atl/omUiAtlCtrlUtils.h"
#include "ui/atl/omUiAtlPushButton.h"
#include "ui/atl/omUiAtlStaticLine.h"
#include "ui/atl/omUiAtlFileDialog.h"

// om::ast
#include "omAstApp.h"
#include "omAstEditApplicationDlg.h"

using namespace om::ast;

namespace {

    const TCHAR g_appKeysFile[] = _T(/*MSG0*/"omRMkeys.txt");

    // {12A26B73-5CF2-4118-893E-11D5B6BBCC6C}
    const GUID g_openExeFileDataGUID =
    { 0x12a26b73, 0x5cf2, 0x4118, { 0x89, 0x3e, 0x11, 0xd5, 0xb6, 0xbb, 0xcc, 0x6c } };

    bool LoadKeys(om::tstrings &keys);
    void InitKeys(om::tstrings &keys);

} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////
// om::ast::EditApplicationDlg

OM_UI_ATL_BEGIN_LAYOUT_MAP(EditApplicationDlg, kVertical | kSizeX) // | kNoClipChildren)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_KEY, kSizeX)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_NAME, kSizeX)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_PROGRAM, kSizeX)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_ARGUMENTS, kSizeX)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_BROWSE_EXE, kMoveXRight)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_LINE1, kSizeX)
    OM_UI_ATL_LAYOUT_WINDOW(IDOK, kMoveXRight)
    OM_UI_ATL_LAYOUT_WINDOW(IDCANCEL, kMoveXRight)
    OM_UI_ATL_LAYOUT_GRIPPER(ATL_IDW_STATUS_BAR)
OM_UI_ATL_END_LAYOUT_MAP()

EditApplicationDlg::EditApplicationDlg(
    SettingsType type,
    RegisteredApplicationPtr pApp /*= RegisteredApplicationPtr()*/) :
    m_type(type),
    m_pApp(pApp),
    m_readOnly(false),
    m_prevEnableAllVersionsState(BST_CHECKED)
{
}

EditApplicationDlg::~EditApplicationDlg()
{
}

LRESULT EditApplicationDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    om::ui::atl::InitDlgControl(*this, IDC_AST_KEY, m_key);
    om::ui::atl::InitDlgControl(*this, IDC_AST_NAME, m_name);
    om::ui::atl::InitDlgControl(*this, IDC_AST_PROGRAM, m_program);
    om::ui::atl::InitDlgControl(*this, IDC_AST_ARGUMENTS, m_arguments);
    om::ui::atl::InitDlgControl(*this, IDC_AST_BROWSE_EXE, m_pBrowseExe);
    om::ui::atl::InitDlgControl(*this, IDC_AST_ALL_VERSIONS, m_allVersions);
    om::ui::atl::InitDlgControl(*this, IDC_AST_LINE1, m_pLine1);

    om::ui::atl::SetDlgButtonIcon(*this, IDC_AST_BROWSE_EXE, IDI_AST_BROWSE_EXE);

    EnableToolTips();
    AddToolTip(IDC_AST_BROWSE_EXE, IDS_AST_SELECT_PROGRAM);

    InitKey();

    if (NULL != m_pApp.get())
    {
        m_key.SetWindowText(m_pApp->GetKey().c_str());
        m_name.SetWindowText(m_pApp->GetName().c_str());
        m_program.SetWindowText(m_pApp->GetExecutable().c_str());
        m_arguments.SetWindowText(m_pApp->GetArguments().c_str());
        m_allVersions.SetCheck(kCommonConfiguration == m_pApp->GetConfigurationType() ? BST_CHECKED : BST_UNCHECKED);

        m_currentKeyValue = m_pApp->GetKey().c_str();
    }
    else
    {
        om::tstring caption;
        om::LoadString(caption, IDS_AST_CAPTION_NEW_APPLICATION);
        SetWindowText(caption.c_str());

        m_allVersions.SetCheck(BST_CHECKED);
    }

    UpdateAllVersionsUI();

    if (true == m_readOnly)
    {
        m_key.ModifyStyle(CBS_DROPDOWN, CBS_DROPDOWNLIST);
        m_key.EnableWindow(FALSE);
        m_name.SetReadOnly(TRUE);
        m_arguments.SetReadOnly(TRUE);
        if (m_pBrowseExe.get())
            m_pBrowseExe->EnableWindow(FALSE);
    }

    return TRUE;
}

LRESULT EditApplicationDlg::OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (true == m_readOnly)
    {
        EndDialog(IDCANCEL);
        return 0;
    }

    ATL::CAtlString tmp;
    m_key.GetWindowText(tmp);
    om::tstring key = static_cast<LPCTSTR>(tmp);
    om::Trim(key);
    if (key.empty())
    {
        om::tstring msg;
        om::LoadString(msg, IDS_AST_ERR_KEY_EMPTY);
        om::GetMessageLog().Log(om::MessageLog::kError, msg.c_str());
        m_key.SetFocus();
        return 0;
    }

    m_name.GetWindowText(tmp);
    om::tstring name = static_cast<LPCTSTR>(tmp);
    om::Trim(name);

    m_program.GetWindowText(tmp);
    om::tstring program = static_cast<LPCTSTR>(tmp);
    om::Trim(program);
    if (false == om::fileutils::FileExists(program.c_str()))
    {
        om::tstring msg;
        om::LoadString(msg, IDS_AST_ERR_MISSING_EXE);
        om::GetMessageLog().Log(om::MessageLog::kError, msg.c_str());
        if (NULL != m_pBrowseExe.get())
            m_pBrowseExe->SetFocus();
        return 0;
    }

    m_arguments.GetWindowText(tmp);
    om::tstring arguments = static_cast<LPCTSTR>(tmp);
    om::Trim(arguments);

    ConfigurationType configurationType =
        BST_CHECKED == m_allVersions.GetCheck() ? kCommonConfiguration : kCurrentVersionConfiguration;
    if (   kCommonConfiguration == configurationType
        && true == om::IsVersionDependedApp(key.c_str()))
    {
        ATLASSERT(0);   // UpdateAllVersionsUI should prevent this situation
        configurationType = kCurrentVersionConfiguration;
    }

    if (NULL == m_pApp.get())
        m_pApp.reset(new RegisteredApplication(m_type, configurationType, key, name, program, arguments));
    else
    {
        m_pApp->SetKey(key);
        m_pApp->SetName(name);
        m_pApp->SetExecutable(program);
        m_pApp->SetArguments(arguments);
        m_pApp->SetConfigurationType(configurationType);
    }

    EndDialog(IDOK);

    return 0;
}

LRESULT EditApplicationDlg::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    EndDialog(IDCANCEL);
    return 0;
}

LRESULT EditApplicationDlg::OnBrowseExe(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    om::tstring filter;
    om::LoadString(filter, IDS_AST_EXE_FILE_FILTER);

    om::tstring title;
    om::LoadString(title, IDS_AST_SELECT_PROGRAM);

    om::ui::atl::FileOpenDialog dlg;
    dlg.SetTitle(title.c_str());
    dlg.SetClientGuid(g_openExeFileDataGUID, GetApp().GetFileDialogRegPath().c_str());
    dlg.SetFileTypes(filter.c_str());

    om::tstring initialFolder = om::fileutils::GetShellFolder(om::fileutils::kProgramFiles);
    dlg.SetDefaultFolder(initialFolder.c_str());

    if (IDOK == dlg.Show(*this))
        m_program.SetWindowText(dlg.GetResult().c_str());

    return 0;
}

LRESULT EditApplicationDlg::OnSelChangeKey(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    int item = m_key.GetCurSel();
    if (item < 0)
        return 0;

    ATL::CAtlString key;
    m_key.GetLBText(item, key);

    ATL::CAtlString name;
    m_name.GetWindowText(name);

    if (m_currentKeyValue == name)
        m_name.SetWindowText(key);

    m_currentKeyValue = key;

    UpdateAllVersionsUI();

    return 0;
}

LRESULT EditApplicationDlg::OnEditUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    ATL::CAtlString key;
    m_key.GetWindowText(key);

    ATL::CAtlString name;
    m_name.GetWindowText(name);

    if (m_currentKeyValue == name)
        m_name.SetWindowText(key);

    m_currentKeyValue = key;

    UpdateAllVersionsUI();

    return 0;
}

void EditApplicationDlg::InitKey()
{
    om::tstrings keys;
    if (false == LoadKeys(keys))
        InitKeys(keys);

    om::tstrings::iterator iter = keys.begin();
    om::tstrings::iterator iterEnd = keys.end();
    for (; iter != iterEnd; ++iter)
        m_key.AddString((*iter).c_str());

    om::ui::atl::ExpandComboBoxToFit(m_key);
}

void EditApplicationDlg::UpdateAllVersionsUI()
{
    BOOL enableAllVersions = om::IsVersionDependedApp(m_currentKeyValue) ? FALSE : TRUE;

    if (enableAllVersions == m_allVersions.IsWindowEnabled())
        return;

    if (FALSE == enableAllVersions)
    {
        m_prevEnableAllVersionsState = m_allVersions.GetCheck();
        m_allVersions.SetCheck(BST_UNCHECKED);
    }
    else
    {
        m_allVersions.SetCheck(m_prevEnableAllVersionsState);
    }

    m_allVersions.EnableWindow(enableAllVersions);
}

///////////////////////////////////////////////////////////////////////////////
// *unique*

namespace {

    bool LoadKeys(om::tstrings &keys)
    {
        om::tstring pathName = GetApp().GetProductInfo().GetProductPath();
        if (false == om::fileutils::AppendToPath(pathName, g_appKeysFile))
            return false;

        std::basic_ifstream<TCHAR> istr(pathName.c_str());
        if (false == istr.is_open())
            return false;

        while (false == istr.eof())
        {
            om::tstring line;
            std::getline(istr, line);

            om::Trim(line);
            if (   line.empty()
                || line[0] == _T(/*MSG0*/';'))
                continue;

            keys.push_back(line);
        }

        return false == keys.empty();
    }

    void InitKeys(om::tstrings &keys)
    {
        static const LPCTSTR g_defKeys[] = {
            _T(/*MSG0*/"About OPEN MIND"),
            _T(/*MSG0*/"Application Manager"),
            _T(/*MSG0*/"Generalized Postprocessor"),
            _T(/*MSG0*/"Postprocessor"),
            _T(/*MSG0*/"Postprocessor Light"),
            _T(/*MSG0*/"Toolpath Editor"),
            _T(/*MSG0*/"http"),
            _T(/*MSG0*/"mailto"),
            _T(/*MSG0*/".txt")
        };

        const LPCTSTR *iter = g_defKeys;
        const LPCTSTR *iterEnd = iter + _countof(g_defKeys);
        for (; iter != iterEnd; ++iter)
            keys.push_back(om::tstring(*iter));
    }

} // anonymous namespace
