#include "stdafx.h"

// om::ui::atl
#include "ui/atl/omUiAtlLinkCommonCtrlsDll.h"

#include "omAstApp.h"
#include "omAstMainDlg.h"

///////////////////////////////////////////////////////////////////////////////
//

WTL::CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    WTL::CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    om::ast::MainDlg dlgMain;

    if (dlgMain.Create(om::ast::GetApp().GetParentWnd()) == NULL)
    {
        ATLTRACE(_T("Main dialog creation failed!\n"));
        return 0;
    }

    dlgMain.ShowWindow(nCmdShow);

    int nRet = theLoop.Run();

    _Module.RemoveMessageLoop();
    return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
    HRESULT hRes = ::CoInitialize(NULL);
    // If you are running on NT 4.0 or higher you can use the following call instead to 
    // make the EXE free threaded. This means that calls come in on a random RPC thread.
    //	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ATLASSERT(SUCCEEDED(hRes));

    // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
    ::DefWindowProc(NULL, 0, 0, 0L);

    WTL::AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

    hRes = _Module.Init(NULL, hInstance);
    ATLASSERT(SUCCEEDED(hRes));

    int nRet = -1;
    if (true == om::ast::GetApp().Init(lpstrCmdLine))
        nRet = Run(lpstrCmdLine, nCmdShow);
    om::ast::GetApp().Exit();

    _Module.Term();
    ::CoUninitialize();

    return nRet;
}
