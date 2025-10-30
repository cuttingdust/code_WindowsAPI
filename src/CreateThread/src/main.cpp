#include <stdio.h>

#include <Windows.h>

/// 线程函数
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
    int i = 0;
    while (i < 10)
    {
        printf("我是一个来自线程的输出%d\n", i++);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    HANDLE hThread;
    DWORD  dwThreadId;

    /// 创建一个线程
    hThread = ::CreateThread(NULL,       /// 默认的安全属性
                             NULL,       /// 默认的堆栈
                             ThreadProc, /// 线程的入口地址
                             NULL,       /// 默认的线程参数
                             0,          /// 线程立即执行
                             &dwThreadId);

    /// 等待新线程完成
    // WaitForSingleObject(hThread, INFINITE); /// join 阻塞等待

    printf("现在一个新线程即将启动,新线程的id是%d\n", dwThreadId);


    CloseHandle(hThread);

    getchar();

    return 0;
}
