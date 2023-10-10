#pragma once

// boost
#include "boost/scoped_ptr.hpp"

// om
#include "utils/omTString.h"
#include "utils/omProductInfo.h"

namespace om {
    namespace ast {

        class MessageLogObserver;

        class App {
        public:
            ~App();

            static App &GetSingleton();

            bool Init(LPCTSTR cmdLine);
            bool Exit();

            HWND GetParentWnd() const { return m_parentWnd; }
            const om::tstring &GetVersion() const { return m_version; }
            const om::tstring &GetAppTitle() const { return m_appTitle; }

            om::tstring GetAppSectionPath(LPCTSTR subPath = NULL) const;
            om::tstring GetFileDialogRegPath() const;

            const om::ProductInfo &GetProductInfo() const { return m_productInfo; }

        private:
            App();

            App(const App &);              // disabled
            App &operator=(const App &);   // disabled

            void InitAppTitle();
            bool InitResources();
            void ParseCmdLine(LPCTSTR cmdLine);

            HWND m_parentWnd;
            om::tstring m_version;
            om::tstring m_appTitle;
            om::tstring m_appTitleExt;

            om::tstring m_appSectionPath;

            om::ProductInfo m_productInfo;

            boost::scoped_ptr<MessageLogObserver> m_pMessageLogObserver;
        };

        inline App &GetApp()
        {
            return App::GetSingleton();
        }

    } // namespace ast
} // namespace om
