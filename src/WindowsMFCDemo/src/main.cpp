#include <afxwin.h>


class CHelloApp : public CWinApp
{
public:
    BOOL InitInstance() override;
};
CHelloApp theApp;
/// CHelloApp theApp;你的应用程序作为一个C++对象，被MFC
/// 当成应用程序HINSTANCE 来感知

class CHelloWindow : public CFrameWnd
{
public:
    CHelloWindow();

private:
    CStatic* cs;
};

BOOL CHelloApp::InitInstance()
{
    m_pMainWnd = new CHelloWindow;
    m_pMainWnd->ShowWindow(m_nCmdShow);
    m_pMainWnd->UpdateWindow();
    return TRUE;
}

CHelloWindow::CHelloWindow()
{
    /// 建立窗口
    CFrameWnd::Create(NULL, _T("Hello MFC"), WS_OVERLAPPEDWINDOW, CRect(0, 0, 400, 300));
    CenterWindow();

    cs = new CStatic();
    cs->Create(_T("Hello My MFC"), WS_CHILD | WS_VISIBLE | SS_CENTER, CRect(50, 80, 350, 150), this);
}
