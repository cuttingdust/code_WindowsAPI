#include "ModernWindowsService.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <sstream>
#include <iomanip>

//////////////////////////////////////////////////////////////////
/// 配置常量
//////////////////////////////////////////////////////////////////
constexpr const wchar_t* SERVICE_NAME        = L"MemoryMonitorService";
constexpr const wchar_t* DISPLAY_NAME        = L"系统内存监控服务";
constexpr const wchar_t* DESCRIPTION         = L"监控系统内存使用情况并记录到日志文件";
constexpr const char*    LOG_FILE_PATH       = "D:\\MemoryMonitor.log";
constexpr int            MONITOR_INTERVAL_MS = 5000;

//////////////////////////////////////////////////////////////////
/// 内存监控服务
//////////////////////////////////////////////////////////////////
class MemoryMonitorService
{
public:
    MemoryMonitorService() = default;

    bool Initialize()
    {
        std::cout << "内存监控服务初始化..." << std::endl;
        return WriteToLog("=== 内存监控服务启动 ===");
    }

    void Run()
    {
        std::cout << "内存监控服务开始运行..." << std::endl;
        WriteToLog("服务主循环开始");

        while (isRunning_)
        {
            MonitorMemory();
            std::this_thread::sleep_for(std::chrono::milliseconds(MONITOR_INTERVAL_MS));
        }

        WriteToLog("服务主循环结束");
    }

    void Stop()
    {
        std::cout << "停止内存监控服务..." << std::endl;
        isRunning_ = false;
        WriteToLog("=== 内存监控服务停止 ===");
    }

private:
    bool WriteToLog(const std::string& message)
    {
        FILE* file = fopen(LOG_FILE_PATH, "a");
        if (!file)
        {
            std::cerr << "无法打开日志文件: " << LOG_FILE_PATH << std::endl;
            return false;
        }

        auto now    = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);

        char timeStr[100];
        ctime_s(timeStr, sizeof(timeStr), &time_t);
        /// 去掉换行符
        std::string timeString(timeStr);
        if (!timeString.empty() && timeString.back() == '\n')
        {
            timeString.pop_back();
        }

        fprintf(file, "[%s] %s\n", timeString.c_str(), message.c_str());
        fclose(file);
        return true;
    }

    void MonitorMemory()
    {
        MEMORYSTATUSEX memoryStatus;
        memoryStatus.dwLength = sizeof(memoryStatus);

        if (GlobalMemoryStatusEx(&memoryStatus))
        {
            std::stringstream ss;
            ss << "内存状态 - "
               << "可用物理内存: " << (memoryStatus.ullAvailPhys / (1024 * 1024)) << " MB, "
               << "使用率: " << memoryStatus.dwMemoryLoad << "%";

            WriteToLog(ss.str());
            std::cout << ss.str() << std::endl;
        }
        else
        {
            WriteToLog("获取内存状态失败");
            std::cerr << "获取内存状态失败" << std::endl;
        }
    }

private:
    std::atomic<bool> isRunning_{ true };
};

//////////////////////////////////////////////////////////////////
/// 主函数
//////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    /// 配置服务
    ModernWindowsService::ServiceConfig config;
    config.serviceName    = SERVICE_NAME;
    config.displayName    = DISPLAY_NAME;
    config.description    = DESCRIPTION;
    config.executablePath = ModernWindowsService::GetExecutablePath();
    config.startType      = SERVICE_AUTO_START;

    /// 创建内存监控服务实例
    auto memoryMonitor = std::make_unique<MemoryMonitorService>();

    /// 命令行参数处理
    if (argc > 1)
    {
        std::string command = argv[1];

        if (command == "install")
        {
            std::cout << "正在安装服务..." << std::endl;
            if (ModernWindowsService::InstallService(config))
            {
                std::cout << "服务安装成功!" << std::endl;
            }
            else
            {
                std::cout << "服务安装失败!" << std::endl;
            }
            return 0;
        }
        else if (command == "uninstall")
        {
            std::cout << "正在卸载服务..." << std::endl;
            if (ModernWindowsService::UninstallService(SERVICE_NAME))
            {
                std::cout << "服务卸载成功!" << std::endl;
            }
            else
            {
                std::cout << "服务卸载失败!" << std::endl;
            }
            return 0;
        }
        else if (command == "start")
        {
            std::cout << "正在启动服务..." << std::endl;
            if (ModernWindowsService::StartService(SERVICE_NAME))
            {
                std::cout << "服务启动成功!" << std::endl;
            }
            else
            {
                std::cout << "服务启动失败!" << std::endl;
            }
            return 0;
        }
        else if (command == "stop")
        {
            std::cout << "正在停止服务..." << std::endl;
            if (ModernWindowsService::StopService(SERVICE_NAME))
            {
                std::cout << "服务停止成功!" << std::endl;
            }
            else
            {
                std::cout << "服务停止失败!" << std::endl;
            }
            return 0;
        }
        else if (command == "status")
        {
            std::cout << "查询服务状态..." << std::endl;
            if (ModernWindowsService::IsServiceRunning(SERVICE_NAME))
            {
                std::cout << "服务正在运行!" << std::endl;
            }
            else
            {
                std::cout << "服务未运行!" << std::endl;
            }
            return 0;
        }
        else if (command == "console")
        {
            std::cout << "以控制台模式运行..." << std::endl;
            if (memoryMonitor->Initialize())
            {
                memoryMonitor->Run();
            }
            return 0;
        }
        else
        {
            std::cout << "未知命令: " << command << std::endl;
            std::cout << "可用命令: install, uninstall, start, stop, status, console" << std::endl;
            return 1;
        }
    }

    // 无参数时，作为服务运行
    std::cout << "启动服务框架..." << std::endl;

    ModernWindowsService service(config);

    /// 设置服务回调
    service.SetServiceMainHandler(
            [&memoryMonitor]()
            {
                if (memoryMonitor->Initialize())
                {
                    memoryMonitor->Run();
                }
            });

    service.SetServiceControlHandler(
            [&memoryMonitor](DWORD controlCode)
            {
                if (controlCode == SERVICE_CONTROL_STOP || controlCode == SERVICE_CONTROL_SHUTDOWN)
                {
                    memoryMonitor->Stop();
                }
            });

    /// 运行服务
    if (service.Run())
    {
        std::cout << "服务运行完成" << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "服务运行失败" << std::endl;
        return 1;
    }
}
