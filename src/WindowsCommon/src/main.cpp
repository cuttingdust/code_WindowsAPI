#include "resource.h"

#include <stdio.h>
#include <Windows.h>

void CenterWindow(HWND hWnd);
void CenterWindowToParent(HWND hWnd);

wchar_t sz[256]; /// 缓冲区
wchar_t szBuff[256];

HINSTANCE        appInstance;
INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE     hInstance,     /// 当前实例句柄
                      _In_opt_ HINSTANCE hPrevInstance, /// 前一个实例 总是 NULL（Win16兼容性遗留）
                      _In_ LPWSTR        lpCmdLine,     /// 命令行参数
                      _In_ int           nCmdShow       /// 窗口显示模式
)
{
    /// 忽略未使用的参数
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    appInstance = hInstance;

    int nRes = ::DialogBoxParam(hInstance, (LPCTSTR)IDD_MAIN, NULL, DlgProc, NULL);

    return 0;
}

INT_PTR CALLBACK DlgProcWin(HWND hDlgWin, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND hParent = NULL; ///  保存父窗口句柄

    switch (msg)
    {
        case WM_INITDIALOG:
            {
                /// 简单的方法：使用系统函数居中
                CenterWindow(hDlgWin);
                /// 获得编辑控件
                ::SetWindowText(::GetDlgItem(hDlgWin, IDC_EDIT), szBuff);
                break;
            }
        case WM_CLOSE:
            {
                ::EndDialog(hDlgWin, IDCANCEL);
                break;
            }
        case WM_COMMAND:
            {
                switch (LOWORD(wParam))
                {
                    case IDOK:
                    case IDCANCEL:
                        {
                            ::EndDialog(hDlgWin, IDCANCEL);
                            break;
                        }
                    default:
                        break;
                }
            }
    }
    return 0;
}


INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int nRes = 0;
    switch (msg)
    {
        case WM_INITDIALOG:
            {
                /// 简单的方法：使用系统函数居中
                CenterWindow(hDlg);
                return TRUE;
            }
        case WM_CLOSE:
            {
                /// 处理点击 X 按钮
                ::EndDialog(hDlg, IDCANCEL);
                return TRUE;
            }
        case WM_SYSCOMMAND:
            {
                /// 处理系统命令（包括关闭）
                if (wParam == SC_CLOSE)
                {
                    ::EndDialog(hDlg, IDCANCEL);
                    return TRUE;
                }
                break;
            }
        case WM_COMMAND: /// 侦测我们的子窗口中的控件ID的
            {
                switch (LOWORD(wParam))
                {
                    case IDC_BTNConfirm:
                        {
                            szBuff[0] = '\0';
                            ::GetWindowText(::GetDlgItem(hDlg, IDC_NAME), sz, 256);
                            wsprintf(szBuff, L"您的姓名是 %s; ", sz);
                            memset(sz, 0, 256);
                            ::GetWindowText(::GetDlgItem(hDlg, IDC_AGE), sz, 256);
                            wsprintf(szBuff + wcslen(szBuff), L"您的年龄是 %d", _wtoi(sz));
                            ::MessageBox(NULL, szBuff, TEXT("Hello"), MB_OK);
                            ::ShowWindow(hDlg, SW_HIDE);
                            nRes = ::DialogBoxParam(appInstance, (LPCTSTR)IDD_WIN, NULL,
                                                    DlgProcWin, /// =>IDD_WIN,所以我们需要一个新的回调函数
                                                    NULL);
                            /// 清空一下当前 IDC_NAME 和 IDC_AGE

                            ::SetWindowText(::GetDlgItem(hDlg, IDC_NAME), L"");
                            ::SetWindowText(::GetDlgItem(hDlg, IDC_AGE), L"");
                            ::ShowWindow(hDlg, SW_SHOW);
                            ::SetForegroundWindow(hDlg);
                            break;
                        }
                    case IDOK:
                        {
                            ::EndDialog(hDlg, IDOK);
                            break;
                        }
                    case IDC_BTNRest:
                        {
                            wchar_t szTEMP[] = L"";
                            ::SetWindowText(::GetDlgItem(hDlg, IDC_NAME), szTEMP);
                            ::SetWindowText(::GetDlgItem(hDlg, IDC_AGE), szTEMP);
                            break;
                        }
                    default:
                        break;
                }
            }
        default:
            break;
    }
    return 0;
}


/// \brief 相对于父窗口居中
/// \param hWnd
void CenterWindowToParent(HWND hWnd)
{
    HWND hParent = GetParent(hWnd);
    RECT rcWnd, rcParent, rcScreen;

    GetWindowRect(hWnd, &rcWnd);
    int width  = rcWnd.right - rcWnd.left;
    int height = rcWnd.bottom - rcWnd.top;

    if (hParent)
    {
        GetWindowRect(hParent, &rcParent);
    }
    else
    {
        GetWindowRect(GetDesktopWindow(), &rcScreen);
        rcParent = rcScreen;
    }

    int parentWidth  = rcParent.right - rcParent.left;
    int parentHeight = rcParent.bottom - rcParent.top;

    int x = rcParent.left + (parentWidth - width) / 2;
    int y = rcParent.top + (parentHeight - height) / 2;

    SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


/// \brief 辅助函数：将窗口居中
/// \param hWnd
void CenterWindow(HWND hWnd)
{
    RECT rc;
    GetWindowRect(hWnd, &rc);

    int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int width  = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;

    SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}
