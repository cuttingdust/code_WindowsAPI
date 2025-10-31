#include <stdio.h>
#include <process.h>
#include <windows.h>
#include <iostream>

/// 票务信息
int g_totalTickets  = 100; /// 总票数
int g_currentTicket = 1;   /// 当前票号
int g_soldCount     = 0;   /// 已售出票数

/// 售票员线程函数
UINT __stdcall TicketSeller(LPVOID lpParam)
{
    int sellerId = (int)lpParam;

    while (true)
    {
        /// 检查是否还有票
        if (g_currentTicket > g_totalTickets)
        {
            break;
        }

        /// 模拟售票过程（存在竞争！）
        int ticketNumber = g_currentTicket; /// 读取当前票号
        Sleep(10);                          /// 模拟处理时间（制造竞争窗口）
        g_currentTicket = ticketNumber + 1; /// 更新票号

        /// 售出这张票
        g_soldCount++;

        printf("售票员%d: 售出票号 %d\n", sellerId, ticketNumber);

        Sleep(50); /// 模拟售票时间
    }

    printf("售票员%d: 工作完成\n", sellerId);
    return 0;
}

int main()
{
    const int SELLER_COUNT = 3; // 3个售票员
    HANDLE    hSellers[SELLER_COUNT];

    printf("=== 多线程售票竞争演示 ===\n");
    printf("总票数: %d, 售票员: %d个\n\n", g_totalTickets, SELLER_COUNT);

    // 创建售票员线程
    for (int i = 0; i < SELLER_COUNT; i++)
    {
        hSellers[i] = (HANDLE)::_beginthreadex(NULL, 0, TicketSeller, (LPVOID)(i + 1), 0, NULL);
    }

    /// 等待所有售票员完成
    ::WaitForMultipleObjects(SELLER_COUNT, hSellers, TRUE, INFINITE);

    /// 清理
    for (int i = 0; i < SELLER_COUNT; i++)
    {
        CloseHandle(hSellers[i]);
    }

    printf("\n=== 售票统计 ===\n");
    printf("应售票数: %d\n", g_totalTickets);
    printf("实际售出: %d\n", g_soldCount);
    printf("最后票号: %d\n", g_currentTicket - 1);

    if (g_soldCount != g_totalTickets)
    {
        printf("❌ 错误: 票数不一致！多卖了 %d 张票\n", g_soldCount - g_totalTickets);
    }
    else
    {
        printf("✅ 票数正确\n");
    }

    getchar();
    return 0;
}
