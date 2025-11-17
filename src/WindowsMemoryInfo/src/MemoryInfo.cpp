#include "MemoryInfo.h"

#define IDT_TIMER 10001 /// 定时器ID

CMemoryApp theApp; /// 全局应用程序对象

/**
 * @brief 应用程序初始化实例
 * @return 初始化成功返回TRUE
 */
BOOL CMemoryApp::InitInstance()
{
    /// 创建主窗口
    m_pMainWnd = new CMemoryWindow();
    /// 显示窗口
    m_pMainWnd->ShowWindow(m_nCmdShow);
    /// 更新窗口
    m_pMainWnd->UpdateWindow();
    return TRUE;
}

/**
 * @brief 内存监视窗口构造函数
 */
CMemoryWindow::CMemoryWindow()
{
    /// 初始化文本缓冲区
    m_szText[0] = { 0 };

    /// 注册窗口类
    LPCTSTR lpszClassName = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,       /// 窗口样式：水平垂直重绘
                                                ::LoadCursor(NULL, IDC_ARROW), /// 光标：箭头
                                                (HBRUSH)(COLOR_3DFACE + 1)     /// 背景画刷：3D界面颜色
    );

    /// 创建窗口
    CWnd::CreateEx(WS_EX_CLIENTEDGE,             /// 扩展样式：客户端边缘
                   lpszClassName,                /// 窗口类名
                   _T("内存监视器"),             /// 窗口标题
                   WS_OVERLAPPEDWINDOW,          /// 窗口样式：重叠窗口
                   CW_USEDEFAULT, CW_USEDEFAULT, /// 位置：默认
                   300, 230,                     /// 大小：300x230
                   NULL, NULL                    /// 父窗口、菜单
    );

    /// 窗口居中显示
    CenterWindow();
}

/// 构建消息映射表
BEGIN_MESSAGE_MAP(CMemoryWindow, CWnd)
ON_WM_CREATE() ///< 窗口创建消息
ON_WM_PAINT()  ///< 绘制消息
ON_WM_TIMER()  ///< 定时器消息
END_MESSAGE_MAP()

/**
 * @brief 析构函数
 */
CMemoryWindow::~CMemoryWindow()
{
}

/**
 * @brief 窗口非客户端区域销毁后调用
 */
void CMemoryWindow::PostNcDestroy()
{
    delete this; /// 删除窗口对象
}

/**
 * @brief 窗口创建时调用
 * @param lpCreateStruct 创建结构体
 * @return 创建成功返回TRUE
 */
BOOL CMemoryWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    /// 获取客户端区域大小
    ::GetClientRect(m_hWnd, &m_rcInfo);

    /// 设置文本框的显示区域（留出边距）
    m_rcInfo.left = 30;    ///<  左边距
    m_rcInfo.top  = 20;    ///<  上边距
    m_rcInfo.right -= 30;  ///<  右边距
    m_rcInfo.bottom -= 30; ///<  下边距

    /// 创建定时器，每500ms触发一次
    ::SetTimer(m_hWnd, IDT_TIMER, 500, NULL);

    return TRUE;
}

/**
 * @brief 窗口绘制函数
 */
void CMemoryWindow::OnPaint()
{
    PAINTSTRUCT ps;
    /// 开始绘制
    HDC hdc = ::BeginPaint(m_hWnd, &ps);

    /// 绘制圆角矩形边框
    ::RoundRect(hdc, m_rcInfo.left, m_rcInfo.top, m_rcInfo.right, m_rcInfo.bottom, 5, 5 /// 圆角半径
    );

    /// 在矩形内绘制文本
    ::DrawText(hdc,
               (LPCWSTR)m_szText, ///< 要绘制的文本
               wcslen(m_szText),  ///<  文本长度
               &m_rcInfo,         ///<  绘制区域
               0                  ///<  格式标志
    );

    /// 结束绘制
    ::EndPaint(m_hWnd, &ps);
}

/**
 * @brief 定时器消息处理函数
 * @param nIDEvent 定时器ID
 */
void CMemoryWindow::OnTimer(UINT_PTR nIDEvent)
{
    /// 检查是否为我们的定时器
    if (nIDEvent == IDT_TIMER)
    {
        wchar_t        szBuff[128]; /// 临时缓冲区
        MEMORYSTATUSEX ms;          /// 内存状态结构体
        ms.dwLength = sizeof(ms);   /// 设置结构体大小

        /// 获取系统内存状态
        ::GlobalMemoryStatusEx(&ms);

        /// 清空文本缓冲区
        m_szText[0] = '\0';

        /// 格式化并拼接各个内存信息

        /// 物理内存总量
        wsprintf(szBuff, _T("\n物理内存总量:%d MB"), ms.ullTotalPhys / (1024 * 1024));
        wcscat(m_szText, szBuff);

        /// 可用物理内存
        wsprintf(szBuff, _T("\n可用内存总量:%d MB"), ms.ullAvailPhys / (1024 * 1024));
        wcscat(m_szText, szBuff);

        /// 虚拟内存总量
        wsprintf(szBuff, _T("\n虚拟内存总量:%d MB"), ms.ullTotalVirtual / (1024 * 1024));
        wcscat(m_szText, szBuff);

        /// 可用虚拟内存
        wsprintf(szBuff, _T("\n可用虚拟内存总量:%d MB"), ms.ullAvailVirtual / (1024 * 1024));
        wcscat(m_szText, szBuff);

        /// 内存使用率
        wsprintf(szBuff, _T("\n内存的使用率:%d %%"), ms.dwMemoryLoad);
        wcscat(m_szText, szBuff);

        /// 请求重绘显示区域，更新显示内容
        ::InvalidateRect(m_hWnd, &m_rcInfo, TRUE);
    }
}
