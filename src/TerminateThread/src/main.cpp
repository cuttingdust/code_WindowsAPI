#include <Windows.h>
#include <iostream>

DWORD WINAPI WorkerThread(LPVOID lpParam)
{
    for (int i = 0; i < 10; i++)
    {
        std::cout << "线程工作中: " << i << std::endl;
        Sleep(1000); /// 每秒输出一次
    }
    return 0;
}

int main()
{
    /// 创建线程
    HANDLE hThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);

    /// 让线程运行3秒
    Sleep(3000);

    std::cout << "准备强制终止线程..." << std::endl;

    /// 使用 TerminateThread 强制终止线程
    if (::TerminateThread(hThread, 0)) /// 这个还是很危险的 能够在线程里面 终止另一个线程
    {
        std::cout << "线程已被强制终止" << std::endl;
    }
    else
    {
        std::cout << "终止失败" << std::endl;
    }

    /// 关闭句柄
    CloseHandle(hThread);

    std::cout << "程序结束" << std::endl;
    getchar();
    return 0;
}
