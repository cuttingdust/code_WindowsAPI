#include "resource.h"

#include <stdio.h>
#include <string>

#include <Windows.h>


LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);

int APIENTRY wWinMain(_In_ HINSTANCE     hInstance,     /// 当前实例句柄
                      _In_opt_ HINSTANCE hPrevInstance, /// 前一个实例 总是 NULL（Win16兼容性遗留）
                      _In_ LPWSTR        lpCmdLine,     /// 命令行参数
                      _In_ int           nCmdShow       /// 窗口显示模式
)
{
    /// 忽略未使用的参数
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    wchar_t    wszClassName[] = L"MainWClass";
    WNDCLASSEX wndclass;

    wndclass.cbSize        = sizeof(wndclass);
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = MainWndProc;
    wndclass.cbClsExtra    = NULL;
    wndclass.cbWndExtra    = NULL;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = ::LoadIcon(hInstance, LPCWSTR(IDI_APP));
    wndclass.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = (LPCWSTR)IDR_TYPER;
    wndclass.lpszClassName = wszClassName;
    wndclass.hIconSm       = NULL;

    ::RegisterClassEx(&wndclass);

    /// 创建主窗口
    HWND hWnd = ::CreateWindowEx(0,
                                 wszClassName, /// 类名
                                 TEXT("我的第一个GUI程序"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                 CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (NULL == hWnd)
    {
        ::MessageBox(NULL, TEXT("创建窗口失败"), TEXT("错误"), MB_OK);
        return -1;
    }

    /// 显示窗口
    ::ShowWindow(hWnd, nCmdShow);
    ::UpdateWindow(hWnd);

    /// 从操作系统的消息队列中不断的捡取消息
    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    static std::wstring str;
    switch (message)
    {
        case WM_DESTROY:
            {
                ::PostQuitMessage(0);
                return 0;
            }
        case WM_COMMAND:
            {
                switch (LOWORD(wparam))
                {
                    case ID_EXIT:
                        ::SendMessage(hWnd, WM_CLOSE, 0, 0);
                        break;
                }
            }
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC         hdc = ::BeginPaint(hWnd, &ps);

                ::SetTextColor(hdc, RGB(255, 0, 0));
                ::SetBkColor(hdc, ::GetSysColor(COLOR_3DFACE));
                ::TextOut(hdc, 0, 0, str.c_str(), str.length());
                ::EndPaint(hWnd, &ps);
                break;
            }
        case WM_LBUTTONDOWN:
            {
                wchar_t szPoint[56];
                wsprintf(szPoint, L"X=%d,Y=%d", LOWORD(lparam), HIWORD(lparam));
                str = szPoint;

                if (wparam & MK_SHIFT)
                {
                    str += L" Shift 按下了";
                }
                ::InvalidateRect(hWnd, NULL, 1);
                return 0;
            }
        case WM_CHAR:
            {
                str = str + wchar_t(wparam);

                ::InvalidateRect(hWnd, NULL, 0);
                return 0;
            }
    }
    return ::DefWindowProc(hWnd, message, wparam, lparam);
}
