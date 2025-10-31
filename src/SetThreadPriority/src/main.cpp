#include <stdio.h>

#include <Windows.h>

/// 线程函数
// DWORD WINAPI ThreadIdle(LPVOID lpParam)
// {
//     int i = 0;
//     for (int i = 0; i < 10; i++)
//     {
//         printf("Idle线程正在运行\n");
//     }
//     return 0;
// }
//
// DWORD WINAPI ThreadNormal(LPVOID lpParam)
// {
//     for (int i = 0; i < 10; i++)
//     {
//         printf("Normal线程正在运行\n");
//     }
//     return 0;
// }

/// 上面计算太小 时间片没耗尽 就计算结束了

DWORD WINAPI ThreadIdle(LPVOID lpParam)
{
    for (int i = 0; i < 1000000; i++)
    { /// 增加工作量
        if (i % 100000 == 0)
        {
            printf("Idle线程: %d\n", i);
        }
        /// 模拟一些计算工作
        volatile long long temp = 0;
        for (int j = 0; j < 1000; j++)
        {
            temp += j * j;
        }
    }
    return 0;
}

DWORD WINAPI ThreadNormal(LPVOID lpParam)
{
    for (int i = 0; i < 1000000; i++)
    {
        if (i % 100000 == 0)
        {
            printf("Normal线程: %d\n", i);
        }
        /// 同样的计算工作
        volatile long long temp = 0;
        for (int j = 0; j < 1000; j++)
        {
            temp += j * j;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    DWORD  dwThreadId;
    HANDLE hThread[2];

    /// 创建一个Idle
    hThread[0] = ::CreateThread(NULL,       /// 默认的安全属性
                                NULL,       /// 默认的堆栈
                                ThreadIdle, /// 线程的入口地址
                                NULL,       /// 默认的线程参数
                                CREATE_SUSPENDED, &dwThreadId);

    ::SetThreadPriority(hThread[0], THREAD_PRIORITY_IDLE);
    ::ResumeThread(hThread[0]);

    /// 创建一个Normal
    hThread[1] = ::CreateThread(NULL, NULL, ThreadNormal, NULL, 0, &dwThreadId);

    /// 等待新线程完成
    // WaitForSingleObject(hThread, INFINITE); /// join 阻塞等待

    ::WaitForMultipleObjects(2,             /// 等待个数
                             hThread, TRUE, /// 只有都完成了，才能结束
                             INFINITE);

    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);

    /// 其实这个优先级的设置 还是不一定有用 Windows多线程 本身就是乱序的

    getchar();

    return 0;
}
