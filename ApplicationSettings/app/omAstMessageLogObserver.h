#pragma once

// std
#include <utility>
#include <queue>

// om
#include "utils/omTStrings.h"
#include "utils/omThreads.h"
#include "utils/omMessageLog.h"
#include "utils/omSignal.h"

namespace om {
    namespace ast {

        class MessageLogObserver :
            public om::threads::ClassLevelLockable<MessageLogObserver>
        {
        public:
            MessageLogObserver(DWORD threadId = 0, HWND mainWnd = 0);
            ~MessageLogObserver();

            void SetMainWnd(HWND mainWnd) { m_mainWnd = mainWnd; }

        protected:

            bool OnMessage(om::MessageLog::MessageType type, LPCTSTR msg, LPCTSTR detail);

            void ShowMessage(om::MessageLog::MessageType type, LPCTSTR msg);

        private:
            MessageLogObserver(const MessageLogObserver &);             // disabled
            MessageLogObserver &operator=(const MessageLogObserver &);  // disabled

            DWORD m_threadId;
            ATL::CWindow m_mainWnd;

            OM_NAMESPACE_BOOST_SIGNALS::scoped_connection m_logMessageCon;
        };

    } // namespace ast
} // namespace om
