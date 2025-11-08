#include <stdio.h>
#include <time.h>
#include <Windows.h>

//////////业务逻辑相关
/// 1	石头 2	剪刀 3	布
/// 0	平手 - 1 电脑获胜 - 2 电脑失败 1电脑失败 2	电脑获胜
const wchar_t *pszStratege[] = { L"", L"石头", L"剪刀", L"布" };
/// 建立缓冲区
wchar_t szText[100];
///////////

void judge(int play1, int play2)
{
    memset(szText, 0, 100);
    if (play1 - play2 == 0)
    {
        wchar_t szBuff[100];
        szBuff[0] = '\0';
        wsprintf(szBuff, L"电脑出的是%s,玩家出的是%s，你们打成了平手\n", pszStratege[play1], pszStratege[play2]);
        wcscat(szText, szBuff);
        return;
    }
    if (play1 - play2 == -1)
    {
        wchar_t szBuff[100];
        szBuff[0] = '\0';
        wsprintf(szBuff, L"电脑出的是%s,玩家出的是%s，电脑获胜\n", pszStratege[play1], pszStratege[play2]);
        wcscat(szText, szBuff);
        return;
    }
    if (play1 - play2 == -2)
    {
        wchar_t szBuff[100];
        szBuff[0] = '\0';
        wsprintf(szBuff, L"电脑出的是%s,玩家出的是%s，电脑失败\n", pszStratege[play1], pszStratege[play2]);
        wcscat(szText, szBuff);
        return;
    }
    if (play1 - play2 == 1)
    {
        wchar_t szBuff[100];
        szBuff[0] = '\0';
        wsprintf(szBuff, L"电脑出的是%s,玩家出的是%s,电脑失败\n", pszStratege[play1], pszStratege[play2]);
        wcscat(szText, szBuff);
        return;
    }
    if (play1 - play2 == 2)
    {
        wchar_t szBuff[100];
        szBuff[0] = '\0';
        wsprintf(szBuff, L"电脑出的是%s,玩家出的是%s，电脑获胜\n", pszStratege[play1], pszStratege[play2]);
        wcscat(szText, szBuff);
        return;
    }
}

int computerPlay()
{
    return rand() % 3 + 1;
}


HINSTANCE        appInstance;
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);

int APIENTRY wWinMain(_In_ HINSTANCE     hInstance,     /// 当前实例句柄
                      _In_opt_ HINSTANCE hPrevInstance, /// 前一个实例 总是 NULL（Win16兼容性遗留）
                      _In_ LPWSTR        lpCmdLine,     /// 命令行参数
                      _In_ int           nCmdShow       /// 窗口显示模式
)
{
    /// 设置控制台为UTF-8编码
    SetConsoleOutputCP(CP_UTF8);

    /// 忽略未使用的参数
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    /// 初始化随机数种子
    srand((unsigned)time(NULL));

    appInstance               = hInstance;
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

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    HWND        hBtnStone;
    HWND        hBtnSisscor;
    HWND        hBtnPaper;
    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rect;

    switch (message)
    {
        case WM_CREATE:
            {
                hBtnStone = (HWND)CreateWindow(TEXT("Button"), TEXT("石头"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 150,
                                               45, 160, 65, hWnd, (HMENU)10001, appInstance, NULL);

                hBtnSisscor = (HWND)CreateWindow(TEXT("Button"), TEXT("剪刀"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                                 350, 45, 160, 65, hWnd, (HMENU)10002, appInstance, NULL);

                hBtnPaper = (HWND)CreateWindow(TEXT("Button"), TEXT("布"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 550,
                                               45, 160, 65, hWnd, (HMENU)10003, appInstance, NULL);
                break;
            }
        case WM_COMMAND:
            {
                if (LOWORD(wparam) == 10001 && HIWORD(wparam) == BN_CLICKED)
                {
                    // MessageBox(hWnd, TEXT("石头被点击"), TEXT("石头剪刀布"), MB_OK);
                    judge(computerPlay(), 1); /// szText缓冲区已经有结果，需要显示给用户
                    MessageBox(hWnd, szText, TEXT("结果"), MB_OK);
                }
                if (LOWORD(wparam) == 10002 && HIWORD(wparam) == BN_CLICKED)
                {
                    // MessageBox(hWnd, TEXT("剪刀被点击"), TEXT("石头剪刀布"), MB_OK);
                    judge(computerPlay(), 2); /// szText缓冲区已经有结果，需要显示给用户
                    MessageBox(hWnd, szText, TEXT("结果"), MB_OK);
                }
                if (LOWORD(wparam) == 10003 && HIWORD(wparam) == BN_CLICKED)
                {
                    // MessageBox(hWnd, TEXT("布被点击"), TEXT("石头剪刀布"), MB_OK);
                    judge(computerPlay(), 3); /// szText缓冲区已经有结果，需要显示给用户
                    MessageBox(hWnd, szText, TEXT("结果"), MB_OK);
                }
                break;
            }
        case WM_DESTROY:
            {
                ::PostQuitMessage(0);
                return 0;
            }
    }
    return ::DefWindowProc(hWnd, message, wparam, lparam);
}
