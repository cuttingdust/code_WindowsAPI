#include <stdio.h>
#include <time.h>

#include <process.h>
#include <Windows.h>

/// 总运行时间: 07.829秒 /// 临界区

/// 总运行时间: 02.672秒 /// 原子 直接增加

/// 总运行时间: 02.672秒 /// 原子 CAS 方案

/// 票务信息
int g_totalTickets  = 100; /// 总票数
int g_currentTicket = 1;   /// 当前票号
int g_soldCount     = 0;   /// 已售出票数

/// 时间统计
DWORD g_startTime; /// 开始时间（毫秒）

CRITICAL_SECTION g_cs;

/// 获取当前时间字符串
void GetCurrentTimeString(char* buffer, int bufferSize)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    snprintf(buffer, bufferSize, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour,
             st.wMinute, st.wSecond, st.wMilliseconds);
}

/// 获取运行时间（毫秒）
DWORD GetElapsedTime()
{
    return ::GetTickCount() - g_startTime;
}

/// 格式化运行时间
void FormatElapsedTime(DWORD milliseconds, char* buffer, int bufferSize)
{
    DWORD hours = milliseconds / (1000 * 60 * 60);
    milliseconds %= (1000 * 60 * 60);

    DWORD minutes = milliseconds / (1000 * 60);
    milliseconds %= (1000 * 60);

    DWORD seconds = milliseconds / 1000;
    milliseconds %= 1000;

    if (hours > 0)
    {
        snprintf(buffer, bufferSize, "%02lu:%02lu:%02lu.%03lu", hours, minutes, seconds, milliseconds);
    }
    else if (minutes > 0)
    {
        snprintf(buffer, bufferSize, "%02lu:%02lu.%03lu", minutes, seconds, milliseconds);
    }
    else
    {
        snprintf(buffer, bufferSize, "%02lu.%03lu秒", seconds, milliseconds);
    }
}

/// 售票员线程函数
UINT WINAPI TicketSeller(LPVOID lpParam)
{
    int  sellerId = (int)(INT_PTR)lpParam; ///  修正类型转换
    char timeBuffer[64];
    char elapsedBuffer[32];
    int  ticketNumber = 0;
    int  oldValue, newValue;

    while (true)
    {
        {
            do
            {
                oldValue = g_currentTicket; /// 读取当前值

                if (oldValue > g_totalTickets)
                {
                    /// 票已售完，退出线程
                    GetCurrentTimeString(timeBuffer, sizeof(timeBuffer));
                    FormatElapsedTime(GetElapsedTime(), elapsedBuffer, sizeof(elapsedBuffer));
                    printf("[%s] 售票员%d: 工作完成 (总运行时间: %s)\n", timeBuffer, sellerId, elapsedBuffer);
                    return 0;
                }

                newValue = oldValue + 1; /// 计算新票号

                /// CAS 操作：如果 g_currentTicket == oldValue，则设置为 newValue
                /// 如果成功，返回 oldValue；如果失败，返回当前 g_currentTicket 的值
            }
            while (::InterlockedCompareExchange((long*)&g_currentTicket, newValue, oldValue) != oldValue);

            /// CAS 成功，获取到票号
            ticketNumber = oldValue;

            /// 增加售出票数
            ::InterlockedIncrement((long*)&g_soldCount);
        }

        {
            // /// 检查是否还有票
            // if (g_currentTicket > g_totalTickets)
            // {
            //     // ::LeaveCriticalSection(&g_cs);
            //     break;
            // }
            //
            // /// 原子操作
            // ticketNumber = ::InterlockedExchangeAdd((long*)&g_currentTicket, 1);
            // ::InterlockedIncrement((long*)&g_soldCount);
        }

        {
            // ::EnterCriticalSection(&g_cs);
            // /// 检查是否还有票
            // if (g_currentTicket > g_totalTickets)
            // {
            //     // ::LeaveCriticalSection(&g_cs);
            //     break;
            // }
            // ticketNumber = g_currentTicket;
            // g_currentTicket++;
            // g_soldCount++;

            // ::LeaveCriticalSection(&g_cs);
        }

        ::Sleep(10); /// 模拟处理时间（制造竞争窗口）

        GetCurrentTimeString(timeBuffer, sizeof(timeBuffer));
        FormatElapsedTime(GetElapsedTime(), elapsedBuffer, sizeof(elapsedBuffer));

        printf("[%s] 售票员%d: 售出票号 %d (运行时间: %s)\n", timeBuffer, sellerId, ticketNumber, elapsedBuffer);

        ::Sleep(50); /// 模拟售票时间
    }

    GetCurrentTimeString(timeBuffer, sizeof(timeBuffer));
    FormatElapsedTime(GetElapsedTime(), elapsedBuffer, sizeof(elapsedBuffer));

    printf("[%s] 售票员%d: 工作完成 (总运行时间: %s)\n", timeBuffer, sellerId, elapsedBuffer);

    return 0;
}

int main()
{
    const int SELLER_COUNT = 3; /// 3个售票员
    HANDLE    hSellers[SELLER_COUNT];
    char      startTimeStr[64];
    char      endTimeStr[64];
    char      totalTimeBuffer[32];

    // ::InitializeCriticalSection(&g_cs);

    /// 记录开始时间
    g_startTime = ::GetTickCount();
    GetCurrentTimeString(startTimeStr, sizeof(startTimeStr));

    printf("=== 多线程售票竞争演示 ===\n");
    printf("开始时间: %s\n", startTimeStr);
    printf("总票数: %d, 售票员: %d个\n\n", g_totalTickets, SELLER_COUNT);

    /// 创建售票员线程
    for (int i = 0; i < SELLER_COUNT; i++)
    {
        hSellers[i] = (HANDLE)::_beginthreadex(NULL, 0, TicketSeller, (LPVOID)(INT_PTR)(i + 1), 0, NULL);
        if (hSellers[i] == NULL)
        {
            printf("创建线程 %d 失败\n", i + 1);
            // ::DeleteCriticalSection(&g_cs);
            return -1;
        }
    }

    /// 等待所有售票员完成
    ::WaitForMultipleObjects(SELLER_COUNT, hSellers, TRUE, INFINITE);

    /// 清理
    for (int i = 0; i < SELLER_COUNT; i++)
    {
        ::CloseHandle(hSellers[i]);
    }

    // ::DeleteCriticalSection(&g_cs);

    /// 计算总运行时间
    DWORD totalElapsed = GetElapsedTime();
    GetCurrentTimeString(endTimeStr, sizeof(endTimeStr));
    FormatElapsedTime(totalElapsed, totalTimeBuffer, sizeof(totalTimeBuffer));

    printf("\n=== 售票统计 ===\n");
    printf("开始时间: %s\n", startTimeStr);
    printf("结束时间: %s\n", endTimeStr);
    printf("总运行时间: %s\n", totalTimeBuffer);
    printf("应售票数: %d\n", g_totalTickets);
    printf("实际售出: %d\n", g_soldCount);
    printf("最后票号: %d\n", g_currentTicket - 1);

    /// 性能统计
    double ticketsPerSecond = (double)g_soldCount / (totalElapsed / 1000.0);
    printf("售票速度: %.2f 张/秒\n", ticketsPerSecond);

    if (g_soldCount != g_totalTickets)
    {
        printf("❌ 错误: 票数不一致！多卖了 %d 张票\n", g_soldCount - g_totalTickets);
        printf("错误: 票数不一致！多卖了 %d 张票\n", g_soldCount - g_totalTickets);
    }
    else
    {
        printf("✅ 票数正确\n");
        printf("票数正确\n");
    }

    printf("\n按回车键退出...");
    getchar();
    return 0;
}
