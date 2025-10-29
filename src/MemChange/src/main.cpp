#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <Psapi.h>

#pragma comment(lib, "Psapi.lib")

/// 使用适合x64的数据类型
#ifdef _WIN64
typedef DWORD64 MEM_ADDR;
#else
typedef DWORD MEM_ADDR;
#endif

/// 查找数据的地址列表
MEM_ADDR g_arList[1024];
int      g_nListCnt; /// 有效的地址个数
HANDLE   g_hProcess; /// 目标进程的句柄


BOOL FindFirst(DWORD dwValue);
BOOL FindNext(DWORD dwValue);
void ShowList();
BOOL WriteMemory(MEM_ADDR dwAddr, DWORD dwValue);
BOOL CompareAPage(MEM_ADDR dwBaseAddr, DWORD dwValue);

int main(void)
{
    /// 检查当前编译平台
#ifdef _WIN64
    printf("运行在 x64 模式\n");
#else
    printf("运行在 x86 模式\n");
#endif

    /// 启动目标进程
    wchar_t             szFileName[] = L"MemChange_Testor.exe";
    STARTUPINFO         si           = { 0 };
    PROCESS_INFORMATION pi           = { 0 };

    si.cb          = sizeof(si);
    si.dwFlags     = STARTF_USESHOWWINDOW;
    si.wShowWindow = TRUE;

    BOOL bRet = ::CreateProcess(NULL, szFileName, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    if (!bRet)
    {
        printf("创建进程失败，错误代码: %d\n", GetLastError());
        return -1;
    }

    g_hProcess = pi.hProcess;

    /// 等待目标进程初始化
    Sleep(1000);

    printf("目标进程已启动 (PID: %d)\n", pi.dwProcessId);

    /// 首先，我们需要将当前的memchange程序接收一个目标值，
    /// 并将这个目标值进行搜索比对
    int iVal;
    printf("请输入您要查找的变量数值: ");
    scanf("%d", &iVal);

    /// 进行第一次查找
    FindFirst(iVal);
    /// 进行交互显示
    ShowList();

    while (g_nListCnt > 1)
    {
        printf("当前有 %d 个候选地址，请再次输入变量数值进行筛选: ", g_nListCnt);
        scanf("%d", &iVal);
        /// 进行下一次搜索
        FindNext(iVal);
        ShowList();

        if (g_nListCnt == 0)
        {
            printf("所有地址都不匹配，搜索失败\n");
            break;
        }
    }

    if (g_nListCnt == 1)
    {
        /// 希望用户将修改的新数值输入，我们进行修改替换
        printf("目标地址是: ");
#ifdef _WIN64
        printf("0x%016llX\n", g_arList[0]);
#else
        printf("0x%08X\n", g_arList[0]);
#endif
        printf("请输入新数据: ");
        scanf("%d", &iVal);

        /// 写入新数值，修改完毕
        if (WriteMemory(g_arList[0], iVal))
        {
            printf("修改成功!\n");
        }
        else
        {
            printf("修改失败! 错误代码: %d\n", GetLastError());
        }
    }
    else if (g_nListCnt > 1)
    {
        printf("仍有 %d 个候选地址，无法确定唯一目标\n", g_nListCnt);
    }

    /// 清理资源
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    getchar();
    return 0;
}

BOOL CompareAPage(MEM_ADDR dwBaseAddr, DWORD dwValue)
{
    BYTE arBytes[4096];
    if (!::ReadProcessMemory(g_hProcess, (LPVOID)dwBaseAddr, arBytes, 4096, NULL))
        return FALSE;

    /// 当前页面可以读
    DWORD* pdw;
    for (int i = 0; i < (int)4 * 1024 - 3; i++)
    {
        pdw = (DWORD*)&arBytes[i];
        if (pdw[0] == dwValue)
        {
            if (g_nListCnt >= 1024)
                return FALSE;
            g_arList[g_nListCnt++] = dwBaseAddr + i;
        }
    }
    return TRUE;
}

BOOL FindFirst(DWORD dwValue)
{
    const DWORD dwOnePage = 4 * 1024;
    if (g_hProcess == NULL)
        return FALSE;

    printf("开始搜索内存...\n");

    /// 使用 VirtualQueryEx 正确遍历内存区域（兼容x86/x64）
    MEMORY_BASIC_INFORMATION mbi;
    MEM_ADDR                 dwBase          = 0;
    DWORD                    dwSearchedPages = 0;

#ifdef _WIN64
    /// x64 (64位) 内存布局
    /// 地址空间：256TB（Windows 实现）
    /// 	0x0000000000000000 - 0x00007FFFFFFFFFFF: 用户空间 (128TB)
    /// 	0xFFFF080000000000 - 0xFFFFFFFFFFFFFFFF: 内核空间 (128TB)
    ///
    /// 用户空间详细划分：
    /// 	0x0000000000000000 - 0x000000000000FFFF: 保留（空指针区域）
    /// 	0x0000000000010000 - 0x000007FFFFFFFFFF: 用户代码、数据等
    /// 	0x000007FFFFFFFFFF - 0x00007FFFFFFFFFFF: 高地址区域
    /// Windows 实际限制：
    /// 实际上用户空间约为 8TB
    /// 常用范围：0x00000000'00000000 到 0x00007FFF'FFFF0000
    /// 所以 x64 设置：
    ///
    /// MEM_ADDR maxAddress = (MEM_ADDR)0x7FFFFFFF0000;  // 用户空间上限
    MEM_ADDR maxAddress = (MEM_ADDR)0x7FFFFFFF0000; ///
#else
    /// x86 (32位) 内存布局
    /// 地址空间：4GB
    /// 	0x00000000 - 0x7FFFFFFF: 用户空间 (2GB)
    /// 	0x80000000 - 0xFFFFFFFF: 内核空间 (2GB)
    ///
    /// 用户空间详细划分：
    /// 	0x00000000 - 0x0000FFFF: 保留（空指针区域）
    /// 	0x00010000 - 0x7FFEFFFF: 用户代码、数据、堆、栈等
    /// 	0x7FFF0000 - 0x7FFFFFFF: 用户空间保护页（禁止访问）
    /// 所以 x86 设置：
    ///
    /// MEM_ADDR maxAddress = (MEM_ADDR)0x7FFF0000;  // 用户空间上限
    ///
    MEM_ADDR maxAddress = (MEM_ADDR)0x7FFF0000; /// 用户空间上限
#endif

    while (VirtualQueryEx(g_hProcess, reinterpret_cast<LPCVOID>(dwBase), &mbi, sizeof(mbi)) && dwBase < maxAddress)
    {
        /// 只搜索已提交的、可读的内存区域
        if (mbi.State == MEM_COMMIT &&
            (mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_EXECUTE_READ ||
             mbi.Protect == PAGE_EXECUTE_READWRITE))
        {
            /// 搜索这个内存区域的所有页面
            for (MEM_ADDR addr = (MEM_ADDR)mbi.BaseAddress; addr < (MEM_ADDR)mbi.BaseAddress + mbi.RegionSize;
                 addr += dwOnePage)
            {
                CompareAPage(addr, dwValue);
                dwSearchedPages++;

                /// 进度显示
                if (dwSearchedPages % 1000 == 0)
                {
                    printf("已搜索 %lu 个页面，找到 %d 个匹配地址...\n", dwSearchedPages, g_nListCnt);
                }
            }
        }

        /// 移动到下一个内存区域
        dwBase = (MEM_ADDR)mbi.BaseAddress + mbi.RegionSize;

        /// 安全检查，避免无限循环
        if (dwBase == 0)
            break;
    }

    printf("首次搜索完成，共搜索 %lu 个页面，找到 %d 个匹配地址\n", dwSearchedPages, g_nListCnt);
    return TRUE;
}

BOOL FindNext(DWORD dwValue)
{
    /// 保存一下有效地址个数
    int nOrgCnt = g_nListCnt;
    g_nListCnt  = 0;

    BOOL  bRet = FALSE; /// 假定查找失败
    DWORD dwReadValue;
    for (int i = 0; i < nOrgCnt; i++)
    {
        if (::ReadProcessMemory(g_hProcess, (LPVOID)g_arList[i], &dwReadValue, sizeof(DWORD), NULL))
        {
            if (dwReadValue == dwValue)
            {
                g_arList[g_nListCnt++] = g_arList[i];
                bRet                   = TRUE;
            }
        }
    }
    printf("筛选后剩余 %d 个匹配地址\n", g_nListCnt);
    return bRet;
}

void ShowList()
{
    printf("当前匹配的地址列表 (%d 个):\n", g_nListCnt);
    for (int i = 0; i < g_nListCnt; i++)
    {
#ifdef _WIN64
        printf("[%d] 0x%016llX\n", i + 1, g_arList[i]); /// 输出16位十六进制数，不足16位前面补零
#else
        printf("[%d] 0x%08X\n", i + 1, g_arList[i]); /// 输出8位十六进制数，不足8位前面补零
#endif
    }
    if (g_nListCnt == 0)
    {
        printf("没有找到匹配的地址\n");
    }
}

BOOL WriteMemory(MEM_ADDR dwAddr, DWORD dwValue)
{
    return ::WriteProcessMemory(g_hProcess, (LPVOID)dwAddr, &dwValue, sizeof(DWORD), NULL);
}
