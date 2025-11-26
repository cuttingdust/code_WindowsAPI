#pragma once
#include <afxwin.h>


class CMyApp : public CWinApp
{
public:
    BOOL InitInstance() override;
};

class CMainWindow : public CWnd
{
    DECLARE_DYNCREATE(CMainWindow)
public:
    CMainWindow();

protected:
    void         PostNcDestroy() override;
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnDestroy();
    afx_msg void OnFileOpen();

protected:
    HDC m_hMemDC; /// 与客户区兼容的内存DC
    int m_nWidth;
    int m_nHeight;

protected:
    DECLARE_MESSAGE_MAP()
};
