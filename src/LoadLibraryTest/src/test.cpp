#include <stdio.h>
#include <windows.h>

///如果需要生成可供外部程序调用的函数，必须用dllexport
_declspec(dllexport) void go()
{
    MessageBox(NULL, L"我是来自dll", L"hello Dll", MB_OK);
}
