#include "resource.h"

#include <stdio.h>
#include <string>

#include <Windows.h>

/// 通过VC手写代码，完成所有控件的原生生成
#define IDC_BUTTON   10001
#define IDC_RADIO    10002
#define IDC_CHECKBOX 10003
#define IDC_STATIC   10004
#define IDC_EDITTEXT 10005

HINSTANCE        appInstance;
LRESULT CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam);

int APIENTRY wWinMain(_In_ HINSTANCE     hInstance,     /// 当前实例句柄
                      _In_opt_ HINSTANCE hPrevInstance, /// 前一个实例 总是 NULL（Win16兼容性遗留）
                      _In_ LPWSTR        lpCmdLine,     /// 命令行参数
                      _In_ int           nCmdShow       /// 窗口显示模式
)
{
    /// 忽略未使用的参数
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    int nRes = ::DialogBoxParam(hInstance,
                                (LPCTSTR)IDD_MAIN, /// 由resoure.h引入的对话框资源号
                                NULL,              /// 父窗口句柄为空
                                DlgProc, NULL);
    if (nRes == IDOK)
    {
        ::MessageBox(NULL, TEXT("用户单击了IDOK"), TEXT("第一个对话框程序"), MB_OK);
    }
    else
    {
        ::MessageBox(NULL, TEXT("用户单击了IDCANCEL"), TEXT("第一个对话框程序"), MB_OK);
    }

    /// 从操作系统的消息队列中不断的捡取消息
    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_INITDIALOG:
            ::CreateWindowEx(0, TEXT("button"), TEXT("标准按钮"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 50, 30, 110,
                             30, hDlg, (HMENU)IDC_BUTTON, appInstance, NULL);

            ::CreateWindowEx(0, TEXT("button"), TEXT("单选框"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 50, 70, 110, 30,
                             hDlg, (HMENU)IDC_RADIO, appInstance, NULL);

            ::CreateWindowEx(0, TEXT("button"), TEXT("复选框"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 50, 110, 110,
                             30, hDlg, (HMENU)IDC_CHECKBOX, appInstance, NULL);
            /// 静态文本
            ::CreateWindowEx(0, TEXT("static"), TEXT("静态文本"), WS_CHILD | WS_VISIBLE | SS_SUNKEN, 50, 150, 150, 60,
                             hDlg, (HMENU)IDC_STATIC, appInstance, NULL);
            /// 可编辑文本
            ::CreateWindowEx(0, TEXT("edit"), TEXT("编辑文本"), WS_CHILD | WS_VISIBLE | ES_MULTILINE, 50, 220, 150, 60,
                             hDlg, (HMENU)IDC_EDITTEXT, appInstance, NULL);
            break;
        case WM_COMMAND:
            {
                switch (LOWORD(wparam))
                {
                    case IDC_BUTTON:
                        ::MessageBox(hDlg, TEXT("Hello"), TEXT("标准按钮"), MB_OK);
                        break;
                    case IDC_RADIO:
                        {
                            BOOL bChecked = ::IsDlgButtonChecked(hDlg, IDC_RADIO);
                            ::EnableWindow(::GetDlgItem(hDlg, IDC_BUTTON), bChecked);
                            /// 设置状态
                            ::CheckDlgButton(hDlg, IDC_RADIO, !bChecked);
                        }
                        break;
                    case IDC_EDITTEXT:
                        {
                            /// 如果我改变了文本框值，那么静态框也有相应动作
                            if (HIWORD(wparam) == EN_CHANGE)
                            {
                                if (IsDlgButtonChecked(hDlg, IDC_CHECKBOX))
                                {
                                    wchar_t sz[256];
                                    ::GetWindowText(::GetDlgItem(hDlg, IDC_EDITTEXT), sz, 256);
                                    ::SetWindowText(::GetDlgItem(hDlg, IDC_STATIC), sz);
                                }
                            }
                        }
                        break;
                    case IDOK:
                        ::EndDialog(hDlg, IDOK);
                        break;
                    case IDCANCEL:
                        ::EndDialog(hDlg, IDCANCEL);
                        break;
                }
                break;
            }
    }

    return 0;
}
