#include <stdio.h>

#include <Windows.h>
#include <tchar.h>

/// 控制台程序 隐藏控制指令
/// /entry:\"mainCRTStartup\"
#pragma comment(linker, "/subsystem:\"windows\" ")


int main(int argc, char *argv[])
{
    MessageBox(0, TEXT("Console"), TEXT(""), 0);
    printf("In Console Hide!");
    getchar();
    return 0;
}

int WINAPI WinMain(                       /// 入口函数
        _In_ HINSTANCE     hInstance,     /// 当前实例句柄
        _In_opt_ HINSTANCE hPrevInstance, /// 前一个实例句柄（已废弃)
        _In_ LPSTR         lpCmdLine,     /// 命令行参数字符串
        _In_ int           nCmdShow       ///  窗口显示方式
)
{
    /// 使用示例：加载应用程序图标
    // HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));

    return main(__argc, __argv);
}
