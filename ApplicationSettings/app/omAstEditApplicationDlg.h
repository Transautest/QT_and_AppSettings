#pragma once

// boost
#include "boost/scoped_ptr.hpp"

// om::ui
#include "ui/atl/omUiAtlFwd.h"
#include "ui/atl/omUiAtlDlgToolTipExt.h"

#include "../appres/omAppSettingsRes.h"
#include "omAstResizable.h"
#include "omAstDefines.h"
#include "omAstApplicationSettings.h"

namespace om {
    namespace ast {

        class EditApplicationDlg :
            public CDialogImpl<EditApplicationDlg>,
            public om::ast::Resizable<EditApplicationDlg>::Type,
            public om::ui::atl::DlgToolTipExt<EditApplicationDlg>
        {
            OM_UI_ATL_DECLARE_LAYOUT_MAP()
        public:
            typedef om::ast::Resizable<EditApplicationDlg>::Type resizable_base_t;

            enum { IDD = IDD_AST_EDIT_APPLICATION };

            EditApplicationDlg(
                SettingsType type,
                RegisteredApplicationPtr pApp = RegisteredApplicationPtr());
            ~EditApplicationDlg();

            void SetReadOnly(bool readOnly = true) { m_readOnly = readOnly; }
            RegisteredApplicationPtr GetRegisteredApp() const { return m_pApp; }

            BEGIN_MSG_MAP(EditApplicationDlg)
                CHAIN_MSG_MAP(resizable_base_t)
                //MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)  // called by Resizable
                COMMAND_ID_HANDLER(IDOK, OnOK)
                COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
                COMMAND_ID_HANDLER(IDC_AST_BROWSE_EXE, OnBrowseExe)
                COMMAND_HANDLER(IDC_AST_KEY, CBN_SELCHANGE, OnSelChangeKey)
                COMMAND_HANDLER(IDC_AST_KEY, CBN_EDITUPDATE, OnEditUpdate)
            END_MSG_MAP()

            LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
            LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
            LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
            LRESULT OnBrowseExe(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
            LRESULT OnSelChangeKey(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
            LRESULT OnEditUpdate(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

        private:
            EditApplicationDlg(const EditApplicationDlg &);             // disabled
            EditApplicationDlg &operator=(const EditApplicationDlg &);  // disabled

            void InitKey();
            void UpdateAllVersionsUI();

            SettingsType m_type;
            RegisteredApplicationPtr m_pApp;
            ATL::CAtlString m_currentKeyValue;
            bool m_readOnly;
            int m_prevEnableAllVersionsState;

            WTL::CComboBox m_key;
            WTL::CEdit m_name;
            WTL::CEdit m_program;
            WTL::CEdit m_arguments;
            WTL::CButton m_allVersions;
            boost::scoped_ptr<om::ui::atl::PushButton> m_pBrowseExe;
            boost::scoped_ptr<om::ui::atl::StaticLine> m_pLine1;
        };

    } // namespace ast
} // namespace om
