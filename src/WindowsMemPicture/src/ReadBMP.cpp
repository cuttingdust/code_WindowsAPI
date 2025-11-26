#include "ReadBMP.h"
#include "resource.h"
#include <afxdlgs.h>
// #include <afxcmn.h>

/// 应用程序类的InitInstance函数 - MFC程序的入口点
BOOL CMyApp::InitInstance()
{
    /// 创建主窗口对象
    m_pMainWnd = new CMainWindow();
    /// 显示窗口（m_nCmdShow参数通常指定显示方式，如最大化、最小化等）
    m_pMainWnd->ShowWindow(m_nCmdShow);
    /// 更新窗口，触发第一次绘制
    m_pMainWnd->UpdateWindow();
    return TRUE;
}

/// 全局应用程序对象 - MFC程序的唯一实例
CMyApp theApp;

/// 主窗口类的构造函数
CMainWindow::CMainWindow()
{
    /// 注册自定义窗口类
    LPCTSTR lpszClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, ///  窗口样式：水平/垂直重绘时重绘整个窗口
                                                ::LoadCursor(NULL, IDC_ARROW), ///  使用标准箭头光标
                                                (HBRUSH)(COLOR_WINDOW + 1),    /// 使用标准窗口背景色
                                                NULL);                         ///  没有图标

    /// 创建窗口实例
    CreateEx(NULL,                                                     /// 扩展样式
             lpszClassName,                                            /// 注册的窗口类名
             _T("BMP浏览器"),                                          /// 窗口标题
             WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, /// 窗口样式：有标题栏、系统菜单、最小化按钮
             CW_USEDEFAULT, CW_USEDEFAULT,                             /// 位置使用默认值
             CW_USEDEFAULT, CW_USEDEFAULT,                             /// 大小使用默认值
             NULL,                                                     /// 父窗口
             NULL);                                                    /// 菜单
}

/// MFC消息映射宏 - 将消息与处理函数关联
BEGIN_MESSAGE_MAP(CMainWindow, CWnd)
ON_WM_CREATE()                                  /// WM_CREATE 消息 - 窗口创建时调用
ON_WM_PAINT()                                   /// WM_PAINT 消息 - 窗口需要重绘时调用
ON_WM_DESTROY()                                 /// WM_DESTROY 消息 - 窗口销毁时调用
ON_COMMAND(FILE_OPEN, &CMainWindow::OnFileOpen) /// 菜单命令 - 文件打开
END_MESSAGE_MAP()

/// 窗口非客户区销毁后的清理函数
void CMainWindow::PostNcDestroy()
{
    delete this; /// 删除窗口对象自身
}

/// 窗口创建消息处理函数
int CMainWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    /// 加载并设置菜单
    ::SetMenu(m_hWnd, ::LoadMenu(theApp.m_hInstance, (LPCTSTR)IDR_MAIN));

    /// 获取窗口客户区设备上下文
    CClientDC dc(this);

    /// 创建与窗口兼容的内存设备上下文
    /// 用于离屏绘制BMP图像，避免直接绘制到屏幕导致的闪烁
    /// 双缓冲绘图
    m_hMemDC = ::CreateCompatibleDC(dc);

    /// 初始化图像尺寸为0
    m_nHeight = 0;
    m_nWidth  = 0;

    return TRUE; /// 创建成功
}

/// 窗口绘制消息处理函数
void CMainWindow::OnPaint()
{
    // CPaintDC  dcClient(this);    /// 客户区设备上下文
    // CWindowDC dcWindow(this);    /// 整个窗口设备上下文
    // /// 将窗口左上角的图像拷贝到客户区
    // ::BitBlt(dcClient,           /// 目标DC
    //          10, 10, 30, 30,     /// 目标位置和尺寸
    //          dcWindow,           /// 源DC
    //          0, 0, SRCCOPY);     /// 源位置和复制方式

    /// 创建绘制设备上下文（自动调用BeginPaint）
    CPaintDC dc(this);

    /// 将内存DC中的BMP图像复制到屏幕
    /// 实现双缓冲绘制，避免闪烁
    ::BitBlt(dc,        /// 目标设备上下文（屏幕）
             0, 0,      /// 目标起始坐标
             m_nWidth,  /// 要复制的宽度（BMP图像宽度）
             m_nHeight, /// 要复制的高度（BMP图像高度）
             m_hMemDC,  /// 源设备上下文（内存中的BMP图像）
             0, 0,      /// 源起始坐标
             SRCCOPY);  /// 复制方式：直接复制
}

/// 窗口销毁消息处理函数
void CMainWindow::OnDestroy()
{
    /// 删除内存设备上下文，释放系统资源
    /// 这是重要的资源清理，防止GDI资源泄漏
    ::DeleteDC(m_hMemDC);
}

/// "文件打开"菜单命令处理函数
void CMainWindow::OnFileOpen()
{
    /// 创建文件打开对话框
    CFileDialog file(TRUE); /// TRUE表示打开文件对话框
    if (!file.DoModal())    /// 显示模态对话框，如果用户取消则返回
    {
        return;
    }

    /// 获取用户选择的文件完整路径
    CString str = file.GetPathName();

    /// 1. 打开磁盘文件
    HANDLE hFile = ::CreateFile(file.GetPathName(),    /// 文件路径
                                GENERIC_READ,          /// 只读访问
                                FILE_SHARE_READ,       /// 共享读取
                                NULL,                  /// 安全属性
                                OPEN_EXISTING,         /// 只打开已存在的文件
                                FILE_ATTRIBUTE_NORMAL, /// 普通文件属性
                                NULL);                 /// 模板文件句柄

    if (hFile == INVALID_HANDLE_VALUE)
    {
        AfxMessageBox(_T("读取文件出错"));
        return;
    }

    /// 2. 创建内存映射对象 - 将文件映射到进程地址空间
    HANDLE hMap = ::CreateFileMapping(hFile,         /// 文件句柄
                                      NULL,          /// 安全属性
                                      PAGE_READONLY, /// 只读保护
                                      0, 0,          /// 文件映射的大小（0表示整个文件）
                                      NULL);         /// 映射对象名称

    /// 3. 将文件映射到进程的地址空间
    LPVOID lpBase = ::MapViewOfFile(hMap,          /// 内存映射对象
                                    FILE_MAP_READ, /// 只读访问
                                    0, 0,          /// 文件偏移量
                                    0);            /// 映射的字节数（0表示到文件结尾）

    /// 定义BMP文件头和信息头指针
    BITMAPFILEHEADER *pFileHeader;
    BITMAPINFO       *pInfoHeader;

    /// 将映射的内存地址转换为BMP文件头结构
    pFileHeader = (BITMAPFILEHEADER *)lpBase;

    /// 检查BMP文件标识："BM"
    if (pFileHeader->bfType != MAKEWORD('B', 'M'))
    {
        AfxMessageBox(_T("不是一个合法的BMP文件"));
        /// 清理资源
        ::UnmapViewOfFile(lpBase);
        ::CloseHandle(hMap);
        ::CloseHandle(hFile);
        return;
    }

    /// 4. 计算像素数据起始位置
    /// 文件头 + 偏移量 = 像素数据开始位置
    BYTE *pBits = (BYTE *)lpBase + pFileHeader->bfOffBits;

    /// 5. 获取BMP信息头（紧接在文件头之后）
    pInfoHeader = (BITMAPINFO *)((BYTE *)lpBase + sizeof(BITMAPFILEHEADER));

    /// 6. 获取BMP图像的宽度和高度
    m_nHeight = pInfoHeader->bmiHeader.biHeight;
    m_nWidth  = pInfoHeader->bmiHeader.biWidth;

    /// 7. 准备显示位图
    CClientDC dc(this); /// 获取窗口设备上下文

    /// 8. 创建与窗口兼容的位图（大小与BMP图像相同）
    HBITMAP hBitmap = ::CreateCompatibleBitmap(dc, m_nWidth, m_nHeight);

    /// 9. 将位图选入内存设备上下文
    /// 现在可以在内存DC上绘制了
    ::SelectObject(m_hMemDC, hBitmap);

    /// 10. 将BMP像素数据设置到设备上下文中
    int nRes = ::SetDIBitsToDevice(m_hMemDC,        /// 目标设备上下文
                                   0, 0,            /// 目标起始坐标
                                   m_nWidth,        /// 显示宽度
                                   m_nHeight,       /// 显示高度
                                   0,               /// 源BMP的X偏移
                                   0,               /// 源BMP的Y偏移
                                   0,               /// 起始扫描线
                                   m_nHeight,       /// 扫描线数量
                                   pBits,           /// 像素数据指针
                                   pInfoHeader,     /// BMP信息结构
                                   DIB_RGB_COLORS); /// 颜色表类型

    /// 11. 强制窗口重绘，显示新加载的BMP图像
    ::InvalidateRect(m_hWnd, NULL, TRUE);

    /// 12. 清理资源
    ::DeleteObject(hBitmap);   /// 删除临时位图
    ::UnmapViewOfFile(lpBase); /// 取消内存映射
    ::CloseHandle(hMap);       /// 关闭内存映射对象
    ::CloseHandle(hFile);      /// 关闭文件句柄
}

IMPLEMENT_DYNCREATE(CMainWindow, CWnd)
