#include <Windows.h>

/// 入口函数	字符集	命令行参数类型	适用场景
/// WinMain		ANSI	LPSTR			传统ANSI程序
/// wWinMain	Unicode	LPWSTR			现代Windows程序
/// main		ANSI	char*[]			控制台程序
/// wmain		Unicode	wchar_t*[]		Unicode控制台程序

int APIENTRY wWinMain(_In_ HINSTANCE     hInstance,     /// 当前实例句柄
                      _In_opt_ HINSTANCE hPrevInstance, /// 前一个实例 总是 NULL（Win16兼容性遗留）
                      _In_ LPWSTR        lpCmdLine,     /// 命令行参数
                      _In_ int           nCmdShow       /// 窗口显示模式
)
{
    /// 忽略未使用的参数
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    /// 您的应用程序代码
    ::MessageBox(NULL, L"Hello, Windows!", L"欢迎", MB_OK);

    return 0;
}
