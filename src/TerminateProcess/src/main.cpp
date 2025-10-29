#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <errno.h>

/// 根据进程ID获取进程名（正确处理Unicode到ANSI转换）
BOOL GetProcessName(DWORD dwId, char* pName, DWORD dwSize)
{
    BOOL   bFound    = FALSE;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32First(hSnapshot, &pe32))
        {
            do
            {
                if (pe32.th32ProcessID == dwId)
                {
                    int len = WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, NULL, 0, NULL, NULL);
                    if (len > 0 && (DWORD)len <= dwSize)
                    {
                        WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, pName, dwSize, NULL, NULL);
                        bFound = TRUE;
                    }
                    else
                    {
                        strncpy_s(pName, dwSize, "名称过长", _TRUNCATE);
                        bFound = TRUE;
                    }
                    break;
                }
            }
            while (Process32Next(hSnapshot, &pe32));
        }

        CloseHandle(hSnapshot);
    }

    return bFound;
}

BOOL TerminateProcessFromId(DWORD dwId)
{
    BOOL   bRet     = FALSE;
    HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, FALSE, dwId);

    if (hProcess != NULL)
    {
        // 获取退出代码
        DWORD exitCode = 0;
        if (GetExitCodeProcess(hProcess, &exitCode))
        {
            if (exitCode == STILL_ACTIVE)
            {
                bRet = ::TerminateProcess(hProcess, 0);
                if (bRet)
                {
                    printf("已发送终止信号\n");
                }
            }
            else
            {
                printf("进程已经退出 (退出代码: %lu)\n", exitCode);
                bRet = TRUE;
            }
        }

        CloseHandle(hProcess);
    }

    return bRet;
}

int main()
{
    {
        char  input[32] = { 0 };
        DWORD dwId      = 0;

        printf("=== 进程终止工具 ===\n");
        printf("请输入目标进程ID: ");

        if (fgets(input, sizeof(input), stdin) != NULL)
        {
            input[strcspn(input, "\n")] = 0;

            char* endptr       = NULL;
            errno              = 0;
            unsigned long temp = strtoul(input, &endptr, 10);

            if (endptr != input && *endptr == '\0' && errno == 0 && temp != 0)
            {
                dwId = (DWORD)temp;

                /// 显示进程信息
                char processName[MAX_PATH] = "未知";
                if (GetProcessName(dwId, processName, sizeof(processName)))
                {
                    printf("目标进程: %s (PID: %lu)\n", processName, dwId);
                }
                else
                {
                    printf("警告: 未找到进程 %lu\n", dwId);
                }

                /// 确认操作
                printf("确定要终止此进程吗? (Y/N): ");
                char confirm = getchar();

                /// 清除输入缓冲区
                int c;
                while ((c = getchar()) != '\n' && c != EOF)
                    ;

                if (confirm == 'y' || confirm == 'Y')
                {
                    if (TerminateProcessFromId(dwId))
                    {
                        printf("✅ 进程终止成功\n");
                    }
                    else
                    {
                        DWORD error = GetLastError();
                        printf("❌ 进程终止失败! 错误代码: %lu\n", error);

                        switch (error)
                        {
                            case ERROR_ACCESS_DENIED:
                                printf("💡 需要管理员权限或目标进程是系统进程\n");
                                break;
                            case ERROR_INVALID_PARAMETER:
                                printf("💡 进程不存在\n");
                                break;
                            case ERROR_INVALID_HANDLE:
                                printf("💡 无效的进程句柄\n");
                                break;
                            default:
                                printf("💡 未知错误\n");
                                break;
                        }
                    }
                }
                else
                {
                    printf("操作已取消\n");
                }
            }
            else
            {
                printf("❌ 错误: 请输入有效的进程ID数字\n");
            }
        }
    }

    printf("按回车键退出...");
    getchar();
    return 0;
}
