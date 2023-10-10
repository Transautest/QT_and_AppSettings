#include "stdafx.h"

// Windows
#define SECURITY_WIN32
#include <security.h>

// om
#include "utils/omTStringUtils.h"

// om::ui
#include "ui/atl/omUiAtlInitCtrl.h"
#include "ui/atl/omUiAtlCtrlUtils.h"
#include "ui/atl/omUiAtlStaticLine.h"
#include "ui/atl/omUiAtlGridControl.h"
#include "ui/atl/omUiAtlGridDataCell.h"
#include "ui/atl/omUiAtlGridStyle.h"
#include "ui/atl/omUiAtlHelp.h"
#include "ui/atl/omUiAtlHighDpiUtils.h"

#include "omAstApp.h"
#include "omAstUtils.h"
#include "omAstEditApplicationDlg.h"
#include "omAstMainDlg.h"

using namespace om::ast;

namespace {

    const DWORD_PTR g_machineLanguageId = static_cast<DWORD_PTR>(-1);

    enum ApplicationColumn {
        kKeyColumn = 0,
        kNameColumn = 1,
        kProgramColumn = 2,
        kVersionColumn = 3,
        kAppColumn = 4,

        kColumnCount = 5
    };

    typedef om::ui::atl::grid::TDataCell <
        om::tstring
    > StringCell;

    typedef om::ui::atl::grid::TDataCell <
        om::tstring,
        om::ui::atl::grid::TextPolicy<om::tstring>,
        om::ui::atl::grid::ImagePolicy < om::tstring >
    > KeyCell;

    typedef om::ui::atl::grid::TDataCell <
        om::ast::RegisteredApplicationPtr
    > RegisteredApplicationCell;

} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////
// om::ast::MainDlg

OM_UI_ATL_BEGIN_LAYOUT_MAP(MainDlg, kVertical | kSizeX | kSizeY) // | kNoClipChildren)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_SETTINGS_TYPE, kSizeX)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_LINE1, kSizeX)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_LANGUAGE, kSizeX)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_LINE2, kSizeX)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_NEW, kMoveXRight)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_EDIT, kMoveXRight)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_DELETE, kMoveXRight)
    OM_UI_ATL_LAYOUT_LISTCTRL(IDC_AST_APPLICATIONS, kSizeX | kSizeY)
        OM_UI_ATL_LAYOUT_FIXED_COLUMN(kAppColumn)
    OM_UI_ATL_LAYOUT_WINDOW(IDC_AST_LINE3, kSizeX | kMoveYPrev)
    OM_UI_ATL_LAYOUT_WINDOW(IDOK, kMoveXRight | kMoveYBottom)
    OM_UI_ATL_LAYOUT_WINDOW(IDCANCEL, kMoveXRight | kMoveYBottom)
    OM_UI_ATL_LAYOUT_WINDOW(IDHELP, kMoveXRight | kMoveYBottom)
    OM_UI_ATL_LAYOUT_GRIPPER(ATL_IDW_STATUS_BAR)
OM_UI_ATL_END_LAYOUT_MAP()

MainDlg::MainDlg() :
    resizable_base_t(CSize(300, 450)),
    m_activeSettingsType(GetSettingsType())
{
}

MainDlg::~MainDlg()
{
}

BOOL MainDlg::PreTranslateMessage(MSG* pMsg)
{
    return CWindow::IsDialogMessage(pMsg);
}

BOOL MainDlg::OnIdle()
{
    return FALSE;
}

LRESULT MainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // center the dialog on the screen
    CenterWindow();

    // set icons
    HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_AST_APP),
        IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_AST_APP),
        IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    SetIcon(hIconSmall, FALSE);

    // register object for message filtering and idle updates
    WTL::CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    UIAddChildWindowContainer(m_hWnd);

    om::ui::atl::InitDlgControl(*this, IDC_AST_SETTINGS_TYPE, m_settingsType);
    om::ui::atl::InitDlgControl(*this, IDC_AST_LANGUAGE, m_language);
    om::ui::atl::InitDlgControl(*this, IDC_AST_APPLICATIONS, m_pApplications);
    om::ui::atl::InitDlgControl(*this, IDC_AST_LINE1, m_pLine1);
    om::ui::atl::InitDlgControl(*this, IDC_AST_LINE2, m_pLine2);
    om::ui::atl::InitDlgControl(*this, IDC_AST_LINE3, m_pLine3);
    
    m_languageSettings.Init(GetApp().GetVersion());
    m_applicationSettings.Init(GetApp().GetVersion());
    
    m_pMachineSettingsStyle = om::ui::atl::grid::Style::Create();
    if (NULL != m_pMachineSettingsStyle.get())
        m_pMachineSettingsStyle->SetTextColor(ComColor(kCI_GrayText));

    m_pProgramStyle = om::ui::atl::grid::Style::Create();
    m_pProgramStyle->SetTextFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS);

    SetWindowText(GetApp().GetAppTitle().c_str());

    InitSettingsType();
    InitLanguage();
    InitApplicationsTable();
    InitApplicationsData();
    UpdateUI();

    return TRUE;
}

LRESULT MainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // unregister message filtering and idle updates
    WTL::CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->RemoveMessageFilter(this);
    pLoop->RemoveIdleHandler(this);

    return 0;
}

LRESULT MainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    Update();
    if (false == Save())
        return 0;

    CloseDialog(wID);
    return 0;
}

LRESULT MainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CloseDialog(wID);
    return 0;
}

LRESULT MainDlg::OnHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    om::ui::atl::ShowHelp(_T(/*MSG0*/"hmTD.AppSettings.main"), nullptr, *this);

    return 0;
}

LRESULT MainDlg::OnNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (NULL == m_pApplications.get())
        return 0;

    EditApplicationDlg dlg(m_activeSettingsType);
    if (IDOK == dlg.DoModal(*this))
    {
        int row = m_pApplications->GetFocusCell().GetRow();
        row = InsertApp(row, dlg.GetRegisteredApp());
        if (row >= 0)
            m_pApplications->SetFocusCell(om::ui::atl::grid::CellId(kKeyColumn, row));
    }

    return 0;
}

LRESULT MainDlg::OnEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (NULL == m_pApplications.get())
        return 0;

    int row = m_pApplications->GetFocusCell().GetRow();
    RegisteredApplicationPtr pApp = GetRegisteredApp(row);
    if (NULL == pApp.get())
        return 0;

    RegisteredApplicationPtr pTmp(new RegisteredApplication(*pApp));
    EditApplicationDlg dlg(m_activeSettingsType, pTmp);
    dlg.SetReadOnly(m_activeSettingsType != pTmp->GetSettingsType());
    if (IDOK == dlg.DoModal(*this))
    {
        if (0 == CompareRegisteredApplicationKey(pTmp->GetKey().c_str(), pApp->GetKey().c_str()))
        {
            *pApp = *pTmp;
            if (true == m_applicationSettings.SetApp(pApp))
            {
                pApp = m_applicationSettings.FindApp(pApp->GetKey().c_str(),
                    kUserSettings == m_activeSettingsType ? kUnknownSettingsType : m_activeSettingsType);
                ATLASSERT(NULL != pApp.get());
                if (NULL != pApp.get())
                {
                    UpdateRow(row, pApp);
                    m_pApplications->InvalidateRow(row);
                }
            }
        }
        else
        {
            DeleteRow(row);
            row = InsertApp(row, pTmp);
            if (row >= 0)
                m_pApplications->SetFocusCell(om::ui::atl::grid::CellId(kKeyColumn, row));
        }
    }

    return 0;
}

LRESULT MainDlg::OnDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (NULL == m_pApplications.get())
        return 0;

    om::ui::atl::grid::CellId cellId = m_pApplications->GetFocusCell();
    if (false == DeleteRow(cellId.GetRow()))
        return 0;

    if (cellId.GetRow() >= m_pApplications->GetRowCount())
        cellId.DecRow();
    m_pApplications->SetFocusCell(cellId);

    return 0;
}

LRESULT MainDlg::OnSettingsTypeChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    int item = m_settingsType.GetCurSel();
    if (item < 0)
        return 0;

    SettingsType newSettingsType = static_cast<SettingsType>(m_settingsType.GetItemData(item));
    if (newSettingsType == m_activeSettingsType)
        return 0;

    UpdateLanguageSettings();
    
    m_activeSettingsType = newSettingsType;

    InitLanguage();
    InitApplicationsData();
    UpdateUI();

    return 0;
}

LRESULT MainDlg::OnSelChangedApplications(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
    UpdateUI();
    return 0;
}

LRESULT MainDlg::OnKeyDownApplications(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
    LPNMKEY pData = reinterpret_cast<LPNMKEY>(pnmh);
    switch (pData->nVKey)
    {
        case VK_F2:
            PostMessage(WM_COMMAND, MAKEWPARAM(IDC_AST_EDIT, BN_CLICKED));
            break;
        case VK_DELETE:
            PostMessage(WM_COMMAND, MAKEWPARAM(IDC_AST_DELETE, BN_CLICKED));
            break;
        case VK_INSERT:
            PostMessage(WM_COMMAND, MAKEWPARAM(IDC_AST_NEW, BN_CLICKED));
            break;
    }

    return FALSE;
}

void MainDlg::CloseDialog(int nVal)
{
    DestroyWindow();
    ::PostQuitMessage(nVal);
}

void MainDlg::InitSettingsType()
{
    if (kMachineSettings == m_activeSettingsType)
    {
        om::tstring label;
        om::LoadString(label, IDS_AST_LABEL_SYSTEM);
        int item = m_settingsType.AddString(label.c_str());
        if (item >= 0)
        {
            m_settingsType.SetItemData(item, kMachineSettings);
            m_settingsType.SetCurSel(item);
        }
    }

    // GetUserNameEx also receives the size of the buffer inc. EOS.
    // Flawfinder: ignore
    TCHAR user[2048];
    ULONG nameLen = 2048;
    if (0 == GetUserNameEx(NameDisplay, user, &nameLen))
        *user = _T(/*MSG0*/'\0');

    if (om::empty(user))
    {
        om::tstring label;
        om::LoadString(label, IDS_AST_LABEL_CURRENT_USER);

        size_t len = label.size();
        if (len >= 2048)
            len = 2047;

        // Number of characters copied to user is limited to buffer size above.
        // Flawfinder: ignore
        memcpy(user, label.c_str(), len * sizeof(TCHAR));
        user[len] = _T(/*MSG0*/'\0');
    }

    int item = m_settingsType.AddString(user);
    if (item >= 0)
        m_settingsType.SetItemData(item, kUserSettings);

    if (m_settingsType.GetCurSel() < 0)
    {
        m_settingsType.SetCurSel(0);
        m_activeSettingsType = static_cast<SettingsType>(m_settingsType.GetItemData(0));
    }

    if (kUserSettings == m_activeSettingsType)
        m_settingsType.EnableWindow(FALSE);
}

void MainDlg::InitApplicationsTable()
{
    if (NULL == m_pApplications.get())
        return;

    m_pApplications->SetColumnsResizable();
    m_pApplications->EnableListMode();
    m_pApplications->EnableSelection();
    m_pApplications->EnableSingleRowSelection();
    m_pApplications->EnableSingleSelectionArea();

    m_pApplications->SetColumnCount(kColumnCount);
    m_pApplications->SetRowCount(1);
    m_pApplications->SetFixedRowCount(1);

    m_pApplications->SetColumnStyle(kProgramColumn, m_pProgramStyle.get());

    om::tstring label;
    om::LoadString(label, IDS_AST_LABEL_KEY);
    om::ui::atl::grid::CellPtr pCell = StringCell::Create(label);
    om::ui::atl::grid::CellId cellId(0, kKeyColumn);
    m_pApplications->SetCell(cellId, pCell.get());

    om::LoadString(label, IDS_AST_LABEL_NAME);
    pCell = StringCell::Create(label);
    cellId.SetColumn(kNameColumn);
    m_pApplications->SetCell(cellId, pCell.get());

    om::LoadString(label, IDS_AST_LABEL_PROGRAM);
    pCell = StringCell::Create(label);
    cellId.SetColumn(kProgramColumn);
    m_pApplications->SetCell(cellId, pCell.get());

    om::LoadString(label, IDS_AST_LABEL_VERSION);
    pCell = StringCell::Create(label);
    cellId.SetColumn(kVersionColumn);
    m_pApplications->SetCell(cellId, pCell.get());

    m_pApplications->SetColumnInternal(kAppColumn);

    CRect rc;
    m_pApplications->GetClientRect(&rc);
    int w = rc.Width() / 6;
    m_pApplications->SetColumnWidth(kKeyColumn, w);
    m_pApplications->SetColumnWidth(kNameColumn, w);
    m_pApplications->SetColumnWidth(kProgramColumn, rc.Width() - w * 3);
    m_pApplications->SetColumnWidth(kVersionColumn, w);

    cellId.SetRow(1);
    cellId.SetColumn(kNameColumn);
    m_pApplications->SetFocusCell(cellId);
}

void MainDlg::InitApplicationsData()
{
    if (NULL == m_pApplications.get())
        return;

    bool columnsFit = m_pApplications->DoColumnsFit(1);
    m_pApplications->DeleteNonFixedRows();

    RegisteredApplications apps;
    if (kUserSettings == m_activeSettingsType)
        m_applicationSettings.GetUserRelevantApps(apps);
    else
        m_applicationSettings.GetMachineRelevantApps(apps);

    m_pApplications->SetRowCount(static_cast<int>(apps.size() + m_pApplications->GetFixedRowCount()));

    int row = m_pApplications->GetFixedRowCount();
    for (RegisteredApplicationPtr pApp : apps)
    {
        UpdateRow(row, pApp);
        ++row;
    }

    if (columnsFit)
        m_pApplications->ExpandColumnsToFit();
}

void MainDlg::InitLanguage()
{
    m_language.ResetContent();

    om::tstring activeLanguage;
    if (kUserSettings == m_activeSettingsType)
        activeLanguage = m_languageSettings.GetUserLanguage();
    else
        activeLanguage = m_languageSettings.GetMachineLanguage();

    const om::tstrings &languages = m_languageSettings.GetAvailableLanguages();

    om::tstrings::const_iterator iter = languages.begin();
    om::tstrings::const_iterator iterEnd = languages.end();
    DWORD_PTR idx = 0;
    for (; iter != iterEnd; ++iter, ++idx)
    {
        int item = m_language.AddString((*iter).c_str());
        if (item >= 0)
        {
            m_language.SetItemData(item, idx);
            if (0 == _tcsicmp((*iter).c_str(), activeLanguage.c_str()))
                m_language.SetCurSel(item);
        }
    }

    if (kUserSettings == m_activeSettingsType)
    {
        om::tstring machineLanguage;
        om::FormatString(machineLanguage, IDS_AST_MACHINE_LANGUAGE, m_languageSettings.GetMachineLanguage().c_str());
        int item = m_language.AddString(machineLanguage.c_str());
        if (item >= 0)
        {
            m_language.SetItemData(item, g_machineLanguageId);
            if (m_language.GetCurSel() < 0)
                m_language.SetCurSel(item);
        }
    }

    if (m_language.GetCurSel() < 0)
        m_language.SetCurSel(0);
}

void MainDlg::Update()
{
    UpdateLanguageSettings();
}

void MainDlg::UpdateLanguageSettings()
{
    int item = m_language.GetCurSel();
    if (item >= 0)
    {
        DWORD_PTR data = m_language.GetItemData(item);
        if (data == g_machineLanguageId)
        {
            m_languageSettings.SetUserLanguage(om::tstring());
        }
        else
        {
            const om::tstrings &languages = m_languageSettings.GetAvailableLanguages();
            if (data < languages.size())
            {
                if (kUserSettings == m_activeSettingsType)
                    m_languageSettings.SetUserLanguage(languages[data]);
                else
                    m_languageSettings.SetMachineLanguage(languages[data]);
            }
        }
    }
}

bool MainDlg::Save()
{
    return    m_languageSettings.Save(GetApp().GetVersion())
           && m_applicationSettings.Save(GetApp().GetVersion());
}

RegisteredApplicationPtr MainDlg::GetRegisteredApp(int row) const
{
    RegisteredApplicationPtr pApp;

    if (NULL != m_pApplications.get())
    {
        RegisteredApplicationCell *pCell =
            dynamic_cast<RegisteredApplicationCell *>(
            m_pApplications->GetCell(om::ui::atl::grid::CellId(row, kAppColumn)));
        if (NULL != pCell)
            pApp = pCell->Get();
    }

    return pApp;
}

int MainDlg::FindApp(LPCTSTR key) const
{
    if (   NULL == m_pApplications.get()
        || true == om::empty(key))
        return -1;

    int row = m_pApplications->GetFixedRowCount();
    int count = m_pApplications->GetRowCount();
    for (; row < count; ++row)
    {
        RegisteredApplicationPtr pApp = GetRegisteredApp(row);
        if (   NULL != pApp.get()
            && 0 == CompareRegisteredApplicationKey(pApp->GetKey().c_str(), key))
            return row;
    }

    return -1;
}

void MainDlg::SetMachineApp(int row, RegisteredApplicationPtr pApp)
{
    if (   NULL == pApp.get()
        || kMachineSettings != pApp->GetSettingsType()
        || NULL == m_pApplications.get())
        return;

    om::ui::atl::grid::CellId cellId(row, kAppColumn);
    RegisteredApplicationCell *pCell =
        dynamic_cast<RegisteredApplicationCell *>(m_pApplications->GetCell(cellId));
    if (NULL == pCell)
        return;

    pCell->Set(pApp);
    m_pApplications->SetRowStyle(cellId.GetRow(), m_pMachineSettingsStyle.get());
    m_pApplications->InvalidateRow(cellId.GetRow());
}

int MainDlg::InsertApp(int row, RegisteredApplicationPtr pApp)
{
    if (   NULL == m_pApplications.get()
        || NULL == pApp.get())
        return -1;

    if (false == m_applicationSettings.SetApp(pApp))
        return -1;

    pApp = m_applicationSettings.FindApp(
        pApp->GetKey().c_str(),
        kUserSettings == m_activeSettingsType ? kUnknownSettingsType : m_activeSettingsType);
    if (NULL == pApp.get())
    {
        ATLASSERT(0);
        return -1;
    }

    int existingAppRow = FindApp(pApp->GetKey().c_str());
    if (existingAppRow >= 0)
    {
        UpdateRow(existingAppRow, pApp);
        m_pApplications->InvalidateRow(existingAppRow);
        m_pApplications->SetFocusCell(om::ui::atl::grid::CellId(existingAppRow, kNameColumn));
        return existingAppRow;
    }

    row = m_pApplications->InsertRow(row);
    if (row < 0)
        return -1;

    UpdateRow(row, pApp);
    m_pApplications->InvalidateRow(row);
    m_pApplications->SetFocusCell(om::ui::atl::grid::CellId(row, kNameColumn));
    return row;
}

bool MainDlg::UpdateRow(int row, RegisteredApplicationPtr pApp)
{
    if (   NULL == m_pApplications.get()
        || NULL == pApp.get())
        return false;

    if (   kUserSettings == m_activeSettingsType
        && kMachineSettings == pApp->GetSettingsType())
    {
        m_pApplications->SetRowStyle(row, m_pMachineSettingsStyle.get());
    }
    else
    {
        m_pApplications->SetRowStyle(row, NULL);
    }

    om::ui::atl::grid::CellPtr pCell = KeyCell::Create(pApp->GetKey());
    if (NULL != pCell.get())
    {
        if (HICON icon = pApp->GetIcon())
        {
            om::ui::atl::grid::StylePtr pCellStyle =
                om::ui::atl::grid::Style::Create();
            if (NULL != pCellStyle.get())
            {
                CSize iconSize = om::ui::atl::GetIconSizeForWindow(*this);

                om::ui::atl::ImageListHandle imageList(::ImageList_Create(iconSize.cx, iconSize.cy, ILC_COLOR8 | ILC_MASK, 1, 1));
                if (NULL != imageList.Get())
                {
                    if (0 == ::ImageList_AddIcon(imageList, icon))
                    {
                        pCellStyle->SetImageList(imageList.Detach());
                        pCell->SetStyle(pCellStyle.get());
                        pCell->SetImage(0);
                    }
                }
            }
        }
    }
    om::ui::atl::grid::CellId cellId(row, kKeyColumn);
    m_pApplications->SetCell(cellId, pCell.get());

    pCell = StringCell::Create(pApp->GetName());
    cellId.SetColumn(kNameColumn);
    m_pApplications->SetCell(cellId, pCell.get());

    pCell = StringCell::Create(pApp->GetExecutable());
    cellId.SetColumn(kProgramColumn);
    m_pApplications->SetCell(cellId, pCell.get());

    om::tstring label;
    om::LoadString(label, kCurrentVersionConfiguration == pApp->GetConfigurationType() ? IDS_AST_VAL_CURRENT : IDS_AST_VAL_ALL);
    pCell = StringCell::Create(label);
    cellId.SetColumn(kVersionColumn);
    m_pApplications->SetCell(cellId, pCell.get());

    pCell = RegisteredApplicationCell::Create(pApp);
    cellId.SetColumn(kAppColumn);
    m_pApplications->SetCell(cellId, pCell.get());

    return true;
}

bool MainDlg::DeleteRow(int row)
{
    if (   NULL == m_pApplications.get()
        || row < m_pApplications->GetFixedRowCount()
        || row >= m_pApplications->GetRowCount())
        return false;

    RegisteredApplicationPtr pApp = GetRegisteredApp(row);
    if (NULL == pApp.get())
    {
        ATLASSERT(0);
        return false;
    }

    if (false == m_applicationSettings.EraseApp(pApp))
    {
        ATLASSERT(0);
        return false;
    }

    pApp = m_applicationSettings.FindApp(pApp->GetKey().c_str(),
        kUserSettings == m_activeSettingsType ? kUnknownSettingsType : m_activeSettingsType);
    if (NULL != pApp.get())
    {
        UpdateRow(row, pApp);
        m_pApplications->InvalidateRow(row);
        return true;
    }

    return m_pApplications->DeleteRow(row);
}

void MainDlg::UpdateUI()
{
    bool enableDelete = false;

    if (NULL != m_pApplications.get())
    {
        RegisteredApplicationPtr pApp = GetRegisteredApp(m_pApplications->GetFocusCell().GetRow());
        if (   NULL != pApp.get()
            && (   kMachineSettings == m_activeSettingsType
                || kUserSettings == pApp->GetSettingsType()))
            enableDelete = true;
    }

    om::ui::atl::EnableDlgItem(*this, IDC_AST_DELETE, enableDelete);
    if (IDC_AST_DELETE == SendMessage(DM_GETDEFID))
        SendMessage(DM_SETDEFID, IDOK);
}
