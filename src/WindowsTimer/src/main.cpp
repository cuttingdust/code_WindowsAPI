#include <stdio.h>

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
    wndclass.hIcon         = ::LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName  = NULL;
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

#define IDT_TIMER 100001

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    static int nNum;      /// 计数
    static int bSetTimer; /// 计时器的开关变量
    wchar_t    szText[56];

    PAINTSTRUCT ps;
    HDC         hdc;

    switch (message)
    {
        case WM_CREATE:
            {
                bSetTimer = FALSE; /// 定时器创建
                break;
            }
        case WM_PAINT:
            {
                hdc = ::BeginPaint(hWnd, &ps);
                wsprintf(szText, L"计数: %d", nNum);
                ::TextOut(hdc, 10, 10, szText, wcslen(szText));
                ::EndPaint(hWnd, &ps);
                break;
            }
        case WM_LBUTTONDOWN:
            {
                if (bSetTimer)
                {
                    /// 关闭定时器
                    ::KillTimer(hWnd, IDT_TIMER);
                    bSetTimer = FALSE;
                }
                else
                {
                    if (::SetTimer(hWnd, IDT_TIMER, 1000, NULL) == 0)
                    {
                        ::MessageBox(hWnd, TEXT("安装定时器失败"), TEXT("TimerDemo"), MB_OK);
                    }
                    else
                    {
                        bSetTimer = TRUE;
                    }
                }
                break;
            }
        case WM_CLOSE: /// 用户要求关窗体
            {
                if (bSetTimer)
                {
                    ::KillTimer(hWnd, IDT_TIMER);
                }
                break;
            }
        case WM_DESTROY:
            {
                ::PostQuitMessage(0);
                return 0;
            }
        case WM_TIMER:
            {
                if (wparam == IDT_TIMER)
                {
                    hdc = GetDC(hWnd);
                    wsprintf(szText, L"计数：%d", nNum++);
                    ::TextOut(hdc, 10, 10, szText, wcslen(szText));
                }
                break;
            }
    }
    return ::DefWindowProc(hWnd, message, wparam, lparam);
}
