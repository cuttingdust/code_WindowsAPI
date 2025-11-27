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

//////////////////////////////////////////////////////////////////


#include <dbghelp.h>
#include <vector>
#include <string>

#pragma comment(lib, "dbghelp.lib")

void CheckDLLDependencies(const char* dllName);


void CheckPEImports()
{
    printf("=== PE导入表分析 ===\n");

    HMODULE           hModule   = GetModuleHandle(NULL);
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + dosHeader->e_lfanew);

    PIMAGE_IMPORT_DESCRIPTOR importDesc =
            (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)hModule +
                                       ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
                                               .VirtualAddress);

    while (importDesc->Name != 0)
    {
        const char* dllName = (const char*)((BYTE*)hModule + importDesc->Name);

        /// 检查DLL是否存在
        HMODULE hTest = LoadLibraryExA(dllName, NULL, LOAD_LIBRARY_AS_DATAFILE);
        if (hTest == NULL)
        {
            printf("❌ 缺失导入DLL: %s\n", dllName);

            /// 进一步检查该DLL的依赖
            CheckDLLDependencies(dllName);
        }
        else
        {
            printf("✅ 导入DLL正常: %s\n", dllName);
            FreeLibrary(hTest);
        }

        importDesc++;
    }
}

void CheckDLLDependencies(const char* dllName)
{
    /// 检查这个缺失DLL的依赖链
    char searchPath[MAX_PATH];
    GetSystemDirectoryA(searchPath, MAX_PATH);

    char fullPath[MAX_PATH];
    sprintf_s(fullPath, "%s\\%s", searchPath, dllName);

    /// 使用LoadLibraryEx来获取更详细的错误信息
    HMODULE hLib = LoadLibraryExA(fullPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (hLib == NULL)
    {
        DWORD error = GetLastError();
        switch (error)
        {
            case ERROR_MOD_NOT_FOUND:
                printf("   ↳ 系统找不到指定的文件\n");
                break;
            case ERROR_DLL_NOT_FOUND:
                printf("   ↳ 依赖的DLL缺失\n");
                break;
            case ERROR_BAD_EXE_FORMAT:
                printf("   ↳ 文件格式错误或架构不匹配\n");
                break;
            default:
                printf("   ↳ 错误代码: 0x%X\n", error);
                break;
        }
    }
    else
    {
        FreeLibrary(hLib);
    }
}
