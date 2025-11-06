#include <stdio.h>
#include <Windows.h>

int main(int argc, char *argv[])
{
    /// 查找记事本程序，然后将它关闭
    /// Win 11
    HWND hWnd = ::FindWindow(NULL, L"无标题 - Notepad"); /// spy++ 抓取整个
    if (hWnd != NULL)
    {
        /// 发一个消息，WM_Close /// 可以看见 所有的窗体软件就是
        /// 通过调用Windows内核对象 并且通过windows 内部的消息机制完成的
        /// 你的程序 → SendMessage() → Windows 内核 → 目标窗口过程 → 处理消息
        ///
        /// 消息分为三类
        /// 系统消息 (System Messages)  WM_CREATE, WM_DESTROY, WM_CLOSE, WM_QUIT
        ///
        /// 用户界面消息 (UI Messages) WM_PAINT, WM_SIZE, WM_MOVE, WM_MOUSEMOVE, WM_KEYDOWN
        ///
        /// 应用程序自定义消息#define WM_MYMSG (WM_USER + 100)
        /// SendMessage(hWnd, WM_MYMSG, wParam, lParam);
        ///
        ///
        ::SendMessage(hWnd, WM_CLOSE, 0, 0);
    }
    getchar();
    return 0;
}
