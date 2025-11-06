#include <stdio.h>
#include <windows.h>
#include <process.h>
HANDLE g_hEventEW, g_hEventNS;

UINT WINAPI ProcessA(LPVOID)
{
    while (true)
    {
        WaitForSingleObject(g_hEventEW, INFINITE); /// P操作
        printf("A\n");                             /// V操作
        SetEvent(g_hEventNS);
        Sleep(200);
    }
    return 0;
}

UINT WINAPI ProcessB(LPVOID)
{
    while (true)
    {
        WaitForSingleObject(g_hEventNS, INFINITE);
        printf("B\n");
        SetEvent(g_hEventEW);
        Sleep(200);
    }
    return 0;
}
int main(void)
{
    HANDLE h[2];
    h[0] = (HANDLE)_beginthreadex(NULL, 0, ProcessA, NULL, 0, NULL);
    h[1] = (HANDLE)_beginthreadex(NULL, 0, ProcessB, NULL, 0, NULL);

    /// 问题1：主线程如何调度A，B两个线程？
    g_hEventEW = CreateEvent(NULL, FALSE, TRUE, NULL);
    g_hEventNS = CreateEvent(NULL, FALSE, FALSE, NULL);
    WaitForMultipleObjects(2, h, TRUE, INFINITE);
    /// 问题2：A、B如何交互打印
    getchar();
    return 0;
}
