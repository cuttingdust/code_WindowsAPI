#include <stdio.h>
#include <windows.h>

/// 正确定义MessageBoxW的函数指针类型
// typedef int(WINAPI* MESSAGEBOXW)(HWND, LPCWSTR, LPCWSTR, UINT);

int main(void)
{
    HMODULE hModule = ::LoadLibrary(L"LoadLibraryTest.dll");
    // HMODULE hModule = ::LoadLibrary(L"USER32.dll");
    if (hModule != NULL)
    {
        /// 获取函数指针前需要检查是否获取成功
        // MESSAGEBOXW func = (MESSAGEBOXW)::GetProcAddress(hModule, "MessageBoxW");
        FARPROC func = (FARPROC)::GetProcAddress(hModule, "?go");
        if (func != NULL)
        {
            // func(NULL, L"测试导入", L"标题", NULL); /// 调用函数
        }
        else
        {
            printf("在DLL中找不到 'go' 函数\n");
            DWORD error = GetLastError();
            printf("错误代码: %d\n", error);
        }

        /// 卸载DLL
        FreeLibrary(hModule);
    }
    else
    {
        printf("没有找到dll\n");
        DWORD error = GetLastError();
        printf("错误代码: %d\n", error);

        /// 常见错误代码含义
        switch (error)
        {
            case 2:
                printf("文件不存在\n");
                break;
            case 126:
                printf("找不到指定的模块\n");
                break;
            case 193:
                printf("不是有效的Win32应用程序\n");
                break;
            default:
                printf("未知错误\n");
                break;
        }
    }

    getchar();
    return 0;
}
