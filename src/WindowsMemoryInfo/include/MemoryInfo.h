#pragma once

#include <afxwin.h>

class CMemoryApp : public CWinApp
{
public:
    virtual BOOL InitInstance();
};

class CMemoryWindow : public CWnd
{
public:
    CMemoryWindow();
    ~CMemoryWindow() override;

protected:
    void         PostNcDestroy() override;
    afx_msg BOOL OnCreate(LPCREATESTRUCT);
    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    DECLARE_MESSAGE_MAP()

protected:
    wchar_t m_szText[1024]; ///< 文本缓冲区
    RECT    m_rcInfo;       ///< 文本内容的边框
};
