#include <stdio.h>
#include <windows.h>
#include <iostream>

//////////////////////////////////////////////////////////////////
/// 定义常量
#define SLEEP_TIME 1000          /// 内存监控间隔时间（毫秒）
#define FILE_PATH  "D:\\log.txt" /// 日志文件路径
//////////////////////////////////////////////////////////////////

/// 全局变量定义
BOOL                  bFlag = FALSE; /// 服务运行标志位
SERVICE_STATUS        servicestatus; /// 服务状态结构体
SERVICE_STATUS_HANDLE hstatus;       /// 服务状态句柄

/// 函数声明
int         WriteToLog(const char *strLine);
void WINAPI ServiceMain(int argc, char **argv);
void WINAPI CtrlHandler(DWORD request);

//////////////////////////////////////////////////////////////////


/**
 * @brief 将日志信息写入文件
 * @param strLine 要写入的日志内容
 * @return 成功返回0，失败返回-1
 */
int WriteToLog(const char *strLine)
{
    /// 以追加模式打开日志文件
    FILE *out = fopen(FILE_PATH, "a+");
    if (out == NULL)
    {
        return -1; /// 文件打开失败
    }

    /// 写入日志内容并换行
    fprintf(out, "%s\n", strLine);
    fclose(out); /// 关闭文件
    return 0;    /// 成功返回
}


/**
 * @brief 服务主函数 - 服务的入口点
 * @param argc 参数个数
 * @param argv 参数数组
 * 
 * 这个函数由SCM（服务控制管理器）调用，负责服务的初始化和主要逻辑
 */
void WINAPI ServiceMain(int argc, char **argv)
{
    /// 初始化服务状态结构体
    servicestatus.dwServiceType  = SERVICE_WIN32;         /// 服务类型：Win32服务
    servicestatus.dwCurrentState = SERVICE_START_PENDING; /// 当前状态：启动中

    /// service有两个状态组合，关机和停止服务
    /// 设置服务接受的控制命令：关机和停止
    servicestatus.dwControlsAccepted        = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
    servicestatus.dwWin32ExitCode           = 0; /// Win32退出代码
    servicestatus.dwServiceSpecificExitCode = 0; /// 服务特定退出代码
    servicestatus.dwCheckPoint              = 0; /// 检查点（用于进度指示）
    servicestatus.dwWaitHint                = 0; /// 等待提示时间

    /// 注册服务控制处理器
    /// 将服务名称"testService"与控制处理器函数CtrlHandler关联
    hstatus = ::RegisterServiceCtrlHandler(L"testService", CtrlHandler);
    if (hstatus == 0)
    {
        WriteToLog("注册服务失败"); /// 注册失败记录日志
        return;                     /// 注册失败直接返回
    }
    WriteToLog("注册服务成功"); /// 注册成功记录日志

    /// 通知SCM切换testService的状态为运行中
    servicestatus.dwCurrentState = SERVICE_RUNNING; /// 更新状态：运行中
    SetServiceStatus(hstatus, &servicestatus);      /// 向SCM报告状态更新

    /// 开始服务主循环 - 监控内存状态
    char str[100];          /// 日志内容缓冲区
    memset(str, '\0', 100); /// 清空缓冲区
    bFlag = TRUE;           /// 设置服务运行标志

    /// 服务主循环
    while (bFlag)
    {
        /// 获取系统内存状态信息
        MEMORYSTATUSEX ms;
        ms.dwLength = sizeof(ms);    /// 必须设置结构体大小
        ::GlobalMemoryStatusEx(&ms); /// 调用API获取内存状态

        /// 计算可用物理内存（转换为MB）
        int avail = ms.ullAvailPhys / (1024 * 1024);

        /// 格式化日志信息
        sprintf(str, "可用内存是%d MB", avail);
        WriteToLog(str); /// 写入日志文件

        Sleep(SLEEP_TIME); /// 等待指定时间后继续监控
    }

    WriteToLog("服务停止"); /// 服务退出记录日志
}


/**
 * @brief 服务控制处理器 - 处理SCM发送的控制命令
 * @param request 控制请求代码
 * 
 * 这个函数处理服务的停止、暂停、继续等控制命令
 */
void WINAPI CtrlHandler(DWORD request)
{
    /// 根据控制请求类型进行处理
    switch (request)
    {
        case SERVICE_CONTROL_STOP:                       /// 停止服务请求
            bFlag                        = false;        /// 设置停止标志
            servicestatus.dwCurrentState = SERVICE_STOP; /// 更新状态：停止中
            break;

        case SERVICE_CONTROL_SHUTDOWN:                      /// 系统关机请求
            bFlag                        = false;           /// 设置停止标志
            servicestatus.dwCurrentState = SERVICE_STOPPED; /// 更新状态：已停止
            break;

        default:
            break; /// 其他控制请求不做处理
    }

    /// 向SCM报告最新的服务状态
    SetServiceStatus(hstatus, &servicestatus);
}

//////////////////////////////////////////////////////////////////


/**
 * @brief 程序主函数 - 应用程序入口点
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 程序退出代码
 * 
 * 这个函数设置服务表并启动服务控制分发器
 */
int main(int argc, char *argv[])
{
    /// 定义服务分派表（必须以两个NULL条目结束）
    SERVICE_TABLE_ENTRY entrytable[2];

    /// 设置第一个服务条目
    entrytable[0].lpServiceName = (LPWSTR) "testservice";               /// 服务名称
    entrytable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain; /// 服务主函数

    /// 设置结束标记（两个NULL条目）
    entrytable[1].lpServiceName = NULL;
    entrytable[1].lpServiceProc = NULL;

    /// 启动服务控制分发器
    /// 这个调用会将程序连接到SCM，并开始处理服务控制请求
    /// 如果启动失败，函数会返回，否则会阻塞直到所有服务停止
    StartServiceCtrlDispatcher(entrytable);

    return 0; /// 程序退出
}
