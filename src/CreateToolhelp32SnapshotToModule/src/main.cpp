#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>

void ShowModuleList(DWORD processId = 0, int maxCount = 50)
{
    MODULEENTRY32W me32;
    me32.dwSize = sizeof(me32);

    /// 创建模块快照，如果processId为0则获取所有模块
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        printf("创建模块快照失败! 错误代码: %d\n", GetLastError());
        return;
    }

    if (!Module32FirstW(hModuleSnap, &me32))
    {
        printf("Module32FirstW 失败! 错误代码: %d\n", GetLastError());
        CloseHandle(hModuleSnap);
        return;
    }

    printf("%-6s %-8s %-20s %-12s %s\n", "序号", "PID", "模块名称", "模块大小", "完整路径");
    printf("----------------------------------------------------------------------------------------\n");

    int count = 0;
    do
    {
        if (count >= maxCount)
        {
            printf("\n已达到显示限制 (%d 个模块)\n", maxCount);
            break;
        }

        /// 将宽字符串转换为多字节字符串输出
        char moduleName[256];
        char modulePath[MAX_PATH];

        WideCharToMultiByte(CP_UTF8, 0, me32.szModule, -1, moduleName, sizeof(moduleName), NULL, NULL);
        WideCharToMultiByte(CP_UTF8, 0, me32.szExePath, -1, modulePath, sizeof(modulePath), NULL, NULL);

        /// 计算模块大小 (KB)
        DWORD moduleSizeKB = me32.modBaseSize / 1024;

        printf("%-6d %-8u %-20s %-8u KB %s\n", count + 1, me32.th32ProcessID, moduleName, moduleSizeKB, modulePath);
        count++;
    }
    while (Module32NextW(hModuleSnap, &me32));

    printf("\n总共显示 %d 个模块\n", count);
    CloseHandle(hModuleSnap);
}

void ShowCurrentProcessModules(int maxCount = 50)
{
    DWORD currentProcessId = ::GetCurrentProcessId();
    printf("=== 当前进程 (%u) 的模块列表 ===\n", currentProcessId);
    ShowModuleList(currentProcessId, maxCount);
}

void ShowSpecificProcessModules(DWORD processId, int maxCount = 50)
{
    printf("=== 进程 %u 的模块列表 ===\n", processId);
    ShowModuleList(processId, maxCount);
}

/// 显示系统所有模块（所有进程）
void ShowAllSystemModules(int maxCount = 100)
{
    printf("=== 系统所有模块列表 ===\n");
    ShowModuleList(0, maxCount); /// processId = 0 表示所有进程
}

int main()
{
    // setlocale(LC_ALL, "chs"); // 中文支持
    // setlocale(LC_ALL, "zh_CN.UTF-8");

    printf("=== 模块列表显示程序 ===\n\n");

    int choice;
    do
    {
        printf("\n请选择显示选项:\n");
        printf("1. 显示当前进程的模块\n");
        printf("2. 显示指定进程的模块\n");
        printf("3. 显示系统所有模块\n");
        printf("4. 退出\n");
        printf("请选择 (1-4): ");

        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                ShowCurrentProcessModules(50);
                break;

            case 2:
                {
                    DWORD pid;
                    printf("请输入进程ID: ");
                    scanf("%u", &pid);
                    ShowSpecificProcessModules(pid, 50);
                }
                break;

            case 3:
                ShowAllSystemModules(100);
                break;

            case 4:
                printf("程序退出\n");
                break;

            default:
                printf("无效选择，请重新输入\n");
                break;
        }
    }
    while (choice != 4);

    printf("\n按回车键退出...");
    getchar(); // 清除之前的换行符
    getchar(); // 等待按键
    return 0;
}
