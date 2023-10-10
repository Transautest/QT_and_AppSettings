#include "stdafx.h"

// boost
#include "boost/bind/bind.hpp"

// om
#include "utils/omMessageLog.h"

// om::ast
#include "omAstApp.h"
#include "omAstMessageLogObserver.h"

using namespace boost::placeholders;
using namespace om::ast;

///////////////////////////////////////////////////////////////////////////////
// om::ast::MessageLogObserver

MessageLogObserver::MessageLogObserver(DWORD threadId /*= 0*/, HWND mainWnd /*= 0*/) :
    m_threadId(0 != threadId ? threadId : ::GetCurrentThreadId()),
    m_mainWnd(mainWnd)
{
    m_logMessageCon = om::GetMessageLog().Connect(
        boost::bind(&MessageLogObserver::OnMessage, this, _1, _2, _3));
}

MessageLogObserver::~MessageLogObserver()
{
}

bool MessageLogObserver::OnMessage(om::MessageLog::MessageType type, LPCTSTR msg, LPCTSTR detail)
{
    if (NULL == msg)
        return true;   // continue message processing

    om::tstring msgString = msg;
    if (NULL != detail)
    {
        msgString += _T(/*MSG0*/"\r\n\r\n");
        msgString += detail;
    }

    ShowMessage(type, msgString.c_str());

    return true;
}

void MessageLogObserver::ShowMessage(om::MessageLog::MessageType type, LPCTSTR msg)
{
    if (om::empty(msg))
        return;

    UINT mbType = MB_OK;
    switch (type)
    {
        case om::MessageLog::kWarning:
            mbType |= MB_ICONWARNING;
            break;
        case om::MessageLog::kError:
            mbType |= MB_ICONERROR;
            break;
        default:
            mbType |= MB_ICONINFORMATION;
            break;
    }

    HWND parent = 0;
    if (TRUE == m_mainWnd.IsWindow())
        parent = m_mainWnd;

    ::MessageBox(parent, msg, GetApp().GetAppTitle().c_str(), mbType);
}
