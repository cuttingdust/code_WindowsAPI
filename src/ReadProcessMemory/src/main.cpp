#include <iostream>

#include <windows.h>

int main(int argc, char *argv[])
{
    // HWND h = FindWindowA("EVERYTHING", "Everything");
    // HWND h = FindWindowA(NULL, "计算器");
    HWND h = FindWindowA("MainWindow", "植物大战僵尸中文版");
    printf("窗口句柄 h=%p\r\n", h);

    DWORD pid, tid;
    printf("&pid=%p\r\n", &pid);
    /// int 和long是等价
    /// DWORD  等价于 unsigned long
    /// DWORD* 等价于 LPDWORD
    tid = GetWindowThreadProcessId(h, &pid); /// a&b
    printf("tid=%d pid=%d  16进制tid=%X 16进制pid=%X\r\n", tid, pid, tid, pid);

    /// 获取 进程 权限 句柄
    HANDLE 进程句柄 = OpenProcess(PROCESS_ALL_ACCESS, 0, pid); /// HWND
    printf("进程句柄=%p \r\n", 进程句柄);
    unsigned int 返回值 = 0;
    ReadProcessMemory(进程句柄, (LPCVOID)0x14B9D230 /*要读取的地址*/, &返回值 /*存放数据的地址*/, 4, 0);
    printf("返回值=%X  返回值=%d 返回值=%u\r\n", 返回值, 返回值, 返回值); ///  GetLastError();

    /// 要写入的新值
    unsigned int 新值 = 1000; /// 替换为您要写入的值
    if (!WriteProcessMemory(进程句柄, (LPVOID)0x14B9D230, &新值, sizeof(新值), nullptr))
    {
        printf("写入内存失败，错误代码=%lu\r\n", GetLastError());
    }
    else
    {
        printf("成功写入新值=%X  新值=%d  新值=%u\r\n", 新值, 新值, 新值);
    }

    return 0;
}
