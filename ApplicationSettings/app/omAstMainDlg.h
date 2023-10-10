#pragma once

// boost
#include "boost/scoped_ptr.hpp"

// om::ui
#include "ui/atl/omUiAtlFwd.h"
#include "ui/atl/omUiAtlGridDefines.h"

#include "../appres/omAppSettingsRes.h"
#include "omAstDefines.h"
#include "omAstLanguageSettings.h"
#include "omAstApplicationSettings.h"
#include "omAstResizable.h"

namespace om {
    namespace ast {

        class MainDlg :
            public CDialogImpl<MainDlg>,
            public WTL::CUpdateUI<MainDlg>,
            public WTL::CMessageFilter,
            public WTL::CIdleHandler,
            public om::ast::Resizable<MainDlg>::Type
        {
            OM_UI_ATL_DECLARE_LAYOUT_MAP()
        public:
            typedef om::ast::Resizable<MainDlg>::Type resizable_base_t;

            enum { IDD = IDD_AST_MAINDLG };

            MainDlg();
            MainDlg(const MainDlg &) = delete;
            ~MainDlg();

            MainDlg &operator=(const MainDlg &) = delete;

            virtual BOOL PreTranslateMessage(MSG* pMsg);
            virtual BOOL OnIdle();

            BEGIN_UPDATE_UI_MAP(MainDlg)
            END_UPDATE_UI_MAP()

            BEGIN_MSG_MAP(MainDlg)
                CHAIN_MSG_MAP(resizable_base_t)
                //MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)  // called by Resizable
                MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
                COMMAND_ID_HANDLER(IDOK, OnOK)
                COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
                COMMAND_ID_HANDLER(IDHELP, OnHelp)
                COMMAND_ID_HANDLER(IDC_AST_NEW, OnNew)
                COMMAND_ID_HANDLER(IDC_AST_EDIT, OnEdit)
                COMMAND_ID_HANDLER(IDC_AST_DELETE, OnDelete)
                COMMAND_HANDLER(IDC_AST_SETTINGS_TYPE, CBN_SELCHANGE, OnSettingsTypeChange)
                NOTIFY_HANDLER(IDC_AST_APPLICATIONS, OM_UI_ATL_GRID_SELCHANGED, OnSelChangedApplications)
                NOTIFY_HANDLER(IDC_AST_APPLICATIONS, NM_KEYDOWN, OnKeyDownApplications)
            END_MSG_MAP()

            LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
            LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
            LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
            LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
            LRESULT OnHelp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
            LRESULT OnNew(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
            LRESULT OnEdit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
            LRESULT OnDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
            LRESULT OnSettingsTypeChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
            LRESULT OnSelChangedApplications(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
            LRESULT OnKeyDownApplications(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

            void CloseDialog(int nVal);

        private:
            void InitSettingsType();
            void InitApplicationsTable();
            void InitApplicationsData();
            void InitLanguage();
            void Update();
            void UpdateLanguageSettings();
            bool Save();
            RegisteredApplicationPtr GetRegisteredApp(int row) const;
            int FindApp(LPCTSTR key) const;
            void SetMachineApp(int row, RegisteredApplicationPtr pApp);
            int InsertApp(int row, RegisteredApplicationPtr pApp);
            bool UpdateRow(int row, RegisteredApplicationPtr pApp);
            bool DeleteRow(int row);
            void UpdateUI();

            SettingsType m_activeSettingsType;
            LanguageSettings m_languageSettings;

            ApplicationSettings m_applicationSettings;
            
            om::ui::atl::grid::StylePtr m_pMachineSettingsStyle;
            om::ui::atl::grid::StylePtr m_pProgramStyle;

            WTL::CComboBox m_settingsType;
            WTL::CComboBox m_language;
            boost::scoped_ptr<om::ui::atl::grid::Control> m_pApplications;
            boost::scoped_ptr<om::ui::atl::StaticLine> m_pLine1;
            boost::scoped_ptr<om::ui::atl::StaticLine> m_pLine2;
            boost::scoped_ptr<om::ui::atl::StaticLine> m_pLine3;
            boost::scoped_ptr<om::ui::atl::StaticLine> m_pLine4;
        };

    } // namespace ast
} // namespace om
