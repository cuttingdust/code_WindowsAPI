#include <Windows.h>
#include <stdio.h>

// typedef struct _STARTUPINFO
// {
//     DWORD  cb;              /// 结构体大小，必须设置为 sizeof(STARTUPINFO)
//     LPTSTR lpReserved;      /// 保留，必须为 NULL
//     LPTSTR lpDesktop;       /// 桌面名称
//     LPTSTR lpTitle;         /// 控制台窗口标题
//     DWORD  dwX;             /// 窗口左上角 X 坐标
//     DWORD  dwY;             /// 窗口左上角 Y 坐标
//     DWORD  dwXSize;         /// 窗口宽度
//     DWORD  dwYSize;         /// 窗口高度
//     DWORD  dwXCountChars;   /// 控制台窗口列数
//     DWORD  dwYCountChars;   /// 控制台窗口行数
//     DWORD  dwFillAttribute; /// 控制台文本背景色
//     DWORD  dwFlags;         /// 标志位，决定哪些成员有效
//     WORD   wShowWindow;     /// 窗口显示状态
//     WORD   cbReserved2;     /// 保留，必须为 0
//     LPBYTE lpReserved2;     /// 保留，必须为 NULL
//     HANDLE hStdInput;       /// 标准输入句柄
//     HANDLE hStdOutput;      /// 标准输出句柄
//     HANDLE hStdError;       /// 标准错误句柄
// } STARTUPINFO, *LPSTARTUPINFO;

/// 内核对象是一块内核分配的内存，只能被内核模式下的代码访问，也叫系统资源。
/// Windows提供了API对内核对象的访问，没有其他访问方式
/// • 进程和进程内核对象是两块内存，进程销毁，不会自动把内核对象销毁
/// • 创建进程时除了给进程分配一定的内存空间用于存储进程的代码和数据以及资源外， 还将创建进程内核对象和线程内核对象
/// • 内核对象的内容：安全属性、使用计数（所有内核对象都有）
/// – 进程内核对象：进程ID、优先级、退出代码
/// – 文件内核对象：偏移量、共享模式、打开模式


/// 使用计数指明进程对特定内核对象的引用次数
/// • 创建内核对象时，内核对象的使用计数 = 1若又有另一个进程打开内核对象（使用内核 对象）时，
/// 使用计数 = 2；该进程使用完内核对象时，应该使用CloseHandle将使用计数 值减1，
/// 当使用计数 = 0时，系统将释放此内核对象资源
/// • 使用CreateProcess创建进程时，在创建完成后应该调用CloseHandle关闭创建好
/// 的子进程内核对象句柄和子进程的主线程内核对象句柄，
/// 因为CreateProcess的执行会将新创建的两个内核对象的使用计数 + 1


int main(int argc, char *argv[])
{
    {
        wchar_t     szCommandLine[] = L"notepad.exe";
        STARTUPINFO si; /// 包含新进程的主窗口如何显示的配置信息。  （引用计数=1）
        memset(&si, 0, sizeof(si));
        PROCESS_INFORMATION pi;

        si.cb      = sizeof(STARTUPINFO);
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USEFILLATTRIBUTE; /// 启用颜色设置
                                                                     /// 设置颜色：白色文本，蓝色背景
        si.dwFillAttribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE;
        si.wShowWindow     = TRUE;
        BOOL bRet          = ::CreateProcess(NULL,               /// 应用程序名（使用命令行）
                                             szCommandLine,      /// 命令行字符串
                                             NULL,               /// 进程安全属性
                                             NULL,               /// 线程安全属性
                                             FALSE,              /// 句柄继承选项
                                             CREATE_NEW_CONSOLE, /// 创建标志
                                             NULL,               /// 环境块
                                             NULL,               /// 当前目录
                                             &si,                /// STARTUPINFO  /// （引用计数=2）
                                             &pi                 /// PROCESS_INFORMATION
                 );
        if (!bRet)
        {
            DWORD error = GetLastError();
            printf("创建进程失败，错误代码: %d\n", error);
            return -1;
        }

        /// 启动外部工具，最多等待10秒
        DWORD result = WaitForSingleObject(pi.hProcess, 10000);
        switch (result)
        {
            case WAIT_OBJECT_0:
                printf("工具执行完成\n");
                break;
            case WAIT_TIMEOUT:
                printf("工具执行超时，强制终止\n");
                TerminateProcess(pi.hProcess, 0);
                break;
            case WAIT_FAILED:
                printf("等待失败，错误代码: %d\n", GetLastError());
                break;
        }


        /// pi si 内存没有变
        /// closehandle 这样只是避免资源泄露 /// 归还引用句柄
        ::CloseHandle(pi.hThread);  /// 关闭主线程  （引用计数-1） /// 相当于借书卡
        ::CloseHandle(pi.hProcess); /// 关闭主进程

        printf("新进程的进程ID%d\n", pi.dwProcessId);
        printf("新进程的主线程ID%d\n", pi.dwThreadId);
    }

    getchar();
    return 0;
}
