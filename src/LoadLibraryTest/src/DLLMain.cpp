#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

void PrintParentProcessInfo();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            {
                /// 禁止线程通知，提高性能
                // DisableThreadLibraryCalls(hModule);

                /// 分配控制台用于输出
                AllocConsole();
                freopen("CONOUT$", "w", stdout);

                /// 获取当前进程ID和名称
                DWORD currentPID = GetCurrentProcessId();

                char processName[MAX_PATH] = { 0 };
                GetModuleFileNameExA(GetCurrentProcess(), NULL, processName, MAX_PATH);

                /// 获取调用线程信息
                DWORD threadID = GetCurrentThreadId();

                /// 获取加载时间
                SYSTEMTIME st;
                GetLocalTime(&st);

                std::cout << "=== DLL_PROCESS_ATTACH ===" << std::endl;
                std::cout << "进程ID: " << currentPID << std::endl;
                std::cout << "进程路径: " << processName << std::endl;
                std::cout << "线程ID: " << threadID << std::endl;
                std::cout << "加载时间: " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << std::endl;
                std::cout << "模块句柄: 0x" << std::hex << hModule << std::dec << std::endl;

                /// 尝试获取父进程信息
                PrintParentProcessInfo();

                break;
            }

        case DLL_THREAD_ATTACH:
            {
                DWORD threadID = GetCurrentThreadId();
                std::cout << "DLL_THREAD_ATTACH - 线程ID: " << threadID << std::endl;
                break;
            }

        case DLL_THREAD_DETACH:
            {
                DWORD threadID = GetCurrentThreadId();
                std::cout << "DLL_THREAD_DETACH - 线程ID: " << threadID << std::endl;
                break;
            }

        case DLL_PROCESS_DETACH:
            std::cout << "DLL_PROCESS_DETACH - 进程ID: " << GetCurrentProcessId() << std::endl;
            FreeConsole();
            break;
    }
    return TRUE;
}

/// 获取父进程信息
void PrintParentProcessInfo()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    DWORD currentPID = GetCurrentProcessId();
    DWORD parentPID  = 0;

    /// 查找父进程（这种方法在Windows上有局限）
    if (Process32First(hSnapshot, &pe32))
    {
        do
        {
            /// 这里可以尝试其他方法获取准确的父进程信息
            if (pe32.th32ProcessID == currentPID)
            {
                parentPID = pe32.th32ParentProcessID;
                break;
            }
        }
        while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);

    if (parentPID != 0)
    {
        std::cout << "父进程ID: " << parentPID << std::endl;

        /// 获取父进程名称
        HANDLE hParent = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, parentPID);
        if (hParent)
        {
            char parentName[MAX_PATH] = { 0 };
            if (GetModuleFileNameExA(hParent, NULL, parentName, MAX_PATH))
            {
                std::cout << "父进程路径: " << parentName << std::endl;
            }
            CloseHandle(hParent);
        }
    }
}
