#include "resource.h"

#include <stdio.h>

#include <Windows.h>

/// 第一个图片，第二个图片句柄
HBITMAP g_hBitmap1;
HBITMAP g_hBitmap2;
HICON   g_hIcon;
HBRUSH  g_hBgBrush;

void CenterWindow(HWND hWnd);
//////////////////////////////////////////////////////////////////

/// void* /// int 指针大小
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

    g_hBitmap1 = ::LoadBitmap(hInstance, (LPCTSTR)IDB_BITMAP1);
    g_hBitmap2 = ::LoadBitmap(hInstance, (LPCTSTR)IDB_BITMAP2);
    g_hIcon    = ::LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);

    /// 创建窗体背景刷子
    g_hBgBrush = ::CreateSolidBrush(RGB(0xa6, 0xca, 0xf0));
    int nRes   = ::DialogBoxParam(hInstance, (LPCTSTR)IDD_DIALOG1, NULL, DlgProc, NULL);

    return 0;
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_INITDIALOG:
            {
                /// 居中
                CenterWindow(hDlg);
                /// 加载图标
                ::SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)g_hIcon);
                /// 设置IDC_BMP风格，以便于位图加载
                HWND hWndBMP = ::GetDlgItem(hDlg, IDC_BMP);
                /// SS_BTIMAP
                LONG nStyle = ::GetWindowLong(hWndBMP, GWL_STYLE);
                ::SetWindowLong(hWndBMP, GWL_STYLE, nStyle | SS_BITMAP);
                /// 设置完毕
                ::SendDlgItemMessage(hDlg, IDC_BMP, STM_SETIMAGE, IMAGE_BITMAP, (long)g_hBitmap1);
                /// 设置复选框状态
                ::CheckDlgButton(hDlg, IDC_BMP1, BST_CHECKED);
                break;
            }
        case WM_COMMAND:
            {
                switch (LOWORD(wparam))
                {
                    case IDOK:
                        {
                            HWND hWndEdit = ::GetDlgItem(hDlg, IDC_INPUT);
                            /// 取得文本
                            wchar_t szText[56];
                            int     nLen = ::GetWindowText(hWndEdit, szText, 56);
                            if (nLen > 0)
                            {
                                /// 添加文本
                                ::SendDlgItemMessage(hDlg, IDC_LIST, LB_ADDSTRING, NULL, (LPARAM)szText);
                                ::SetWindowText(hWndEdit, TEXT(""));
                            }
                            break;
                        }
                    case IDCANCEL:
                        {
                            ::EndDialog(hDlg, IDCANCEL);
                            break;
                        }
                    case IDC_TOPMOST:
                        {
                            HWND hWndCheck = ::GetDlgItem(hDlg, IDC_TOPMOST);
                            int  nRet      = ::SendMessage(hWndCheck, BM_GETCHECK, 0, 0);
                            if (nRet == BST_CHECKED)
                            {
                                /// 置顶
                                ::SetWindowPos(hDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
                            }
                            else
                            {
                                ::SetWindowPos(hDlg, HWND_NOTOPMOST, 0, 0, 0, 0,
                                               SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
                            }
                            break;
                        }
                    case IDC_BMP1:
                        {
                            int nRet = ::IsDlgButtonChecked(hDlg, IDC_BMP1);
                            if (nRet == BST_CHECKED)
                            {
                                ::SendDlgItemMessage(hDlg, IDC_BMP, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)g_hBitmap1);
                            }
                            break;
                        }
                    case IDC_BMP2:
                        {
                            int nRet = ::IsDlgButtonChecked(hDlg, IDC_BMP2);
                            if (nRet == BST_CHECKED)
                            {
                                ::SendDlgItemMessage(hDlg, IDC_BMP, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)g_hBitmap2);
                            }
                            break;
                        }
                    case IDC_HIDE:
                        {
                            HWND hWndBmp = ::GetDlgItem(hDlg, IDC_BMP);
                            HWND hWndCHK = ::GetDlgItem(hDlg, IDC_HIDE);
                            int  nRet    = ::SendMessage(hWndCHK, BM_GETCHECK, 0, 0);
                            if (nRet == BST_CHECKED)
                            {
                                ::ShowWindow(hWndBmp, SW_HIDE);
                            }
                            else
                            {
                                ::ShowWindow(hWndBmp, SW_SHOW);
                            }
                            break;
                        }

                    default:
                        break;
                }
                break;
            }
        case WM_CTLCOLORDLG:
            {
                HDC hdc = (HDC)wparam;
                ::SetBkColor(hdc, RGB(0xa6, 0xca, 0xf0));
                /// 设置整个对话框背景为浅蓝色
                return (INT_PTR)g_hBgBrush;
            }

            /// 如果需要静态控件和编辑框也使用相同背景，取消注释：
            // case WM_CTLCOLORSTATIC:
            // {
            // /// 设置静态控件（文本、图片等）背景
            // HDC hdc = (HDC)wparam;
            // SetBkMode(hdc, TRANSPARENT);     /// 文本背景透明
            // SetTextColor(hdc, RGB(0, 0, 0)); /// 设置文本颜色为黑色
            // return (INT_PTR)g_hBgBrush;
            // }

            // case WM_CTLCOLOREDIT:
            // {
            // /// 设置编辑框背景（可选）
            // HDC hdc = (HDC)wparam;
            // SetBkColor(hdc, RGB(0xa6, 0xca, 0xf0)); /// 与画刷相同的颜色
            // return (INT_PTR)g_hBgBrush;
            // }
        case WM_DESTROY:
            {
                if (g_hBgBrush)
                {
                    ::DeleteObject(g_hBgBrush);
                    g_hBgBrush = nullptr;
                }
                if (g_hBitmap1)
                {
                    ::DeleteObject(g_hBitmap1);
                    g_hBitmap1 = nullptr;
                }
                if (g_hBitmap2)
                {
                    ::DeleteObject(g_hBitmap2);
                    g_hBitmap2 = nullptr;
                }
                ::PostQuitMessage(0);
                return 0;
            }
    }
    return 0;
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
