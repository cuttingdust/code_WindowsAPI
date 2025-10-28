#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>

// #include <Locale.h>

void ShowProcessList(int maxCount = 50)
{
    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(pe32);

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        printf("创建进程快照失败! 错误代码: %d\n", GetLastError());
        return;
    }

    if (!Process32FirstW(hProcessSnap, &pe32))
    {
        printf("Process32First 失败! 错误代码: %d\n", GetLastError());
        CloseHandle(hProcessSnap);
        return;
    }

    printf("%-6s %-8s %-8s %-8s %s\n", "序号", "PID", "PPID", "线程数", "进程名称");
    printf("------------------------------------------------------------\n");

    int count = 0;
    do
    {
        if (count >= maxCount)
        {
            printf("\n已达到显示限制 (%d 个进程)\n", maxCount);
            break;
        }

        /// 将宽字符串转换为多字节字符串输出
        char processName[256];
        WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, processName, sizeof(processName), NULL, NULL);

        printf("%-6d %-8u %-8u %-8u %s\n", count + 1, pe32.th32ProcessID, pe32.th32ParentProcessID, pe32.cntThreads,
               processName);
        count++;
    }
    while (Process32NextW(hProcessSnap, &pe32));

    printf("\n总共显示 %d 个进程 (系统可能有更多进程)\n", count);
    CloseHandle(hProcessSnap);
}

int main()
{
    // setlocale(LC_ALL, "chs"); ///chs 表示中文

    // setlocale(LC_ALL, "zh_CN.UTF-8");

    printf("=== 进程列表 (最多显示50个) ===\n");
    ShowProcessList(500);

    printf("\n按回车键退出...");
    getchar();
    return 0;
}
