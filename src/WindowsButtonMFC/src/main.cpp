#include <afxwin.h>
#define IDB_BUTTON 10001

class CButtonApp : public CWinApp
{
public:
    BOOL InitInstance() override;
};
CButtonApp theApp;

class CButtonWindow : public CFrameWnd
{
public:
    CButtonWindow();
    afx_msg void HandleButton();
    DECLARE_MESSAGE_MAP()
private:
    CButton* button = NULL;
};

void CButtonWindow::HandleButton()
{
    ::MessageBox(NULL, _T("我被单击了"), _T("Hello Button"), MB_OK);
}

BEGIN_MESSAGE_MAP(CButtonWindow, CFrameWnd)
ON_BN_CLICKED(IDB_BUTTON, HandleButton)
END_MESSAGE_MAP()

BOOL CButtonApp::InitInstance()
{
    m_pMainWnd = new CButtonWindow;
    m_pMainWnd->ShowWindow(m_nCmdShow);
    m_pMainWnd->UpdateWindow();
    return TRUE;
}

CButtonWindow::CButtonWindow()
{
    /// 建立窗口
    CRect r;
    Create(NULL, _T("CButton"), WS_OVERLAPPEDWINDOW, CRect(0, 0, 200, 200));
    CenterWindow();
    GetClientRect(&r);
    r.InflateRect(-20, -20);
    button = new CButton();
    button->Create(_T("标准按钮"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, r, this, IDB_BUTTON);
}
