/**
 * @file   ModernWindowsService.h
 * @brief  
 *
 * @details   
 *
 * @author 31667
 * @date   2025-11-27
 */

#ifndef MODERNWINDOWSSERVICE_H
#define MODERNWINDOWSSERVICE_H

#pragma once
#include <windows.h>
#include <string>
#include <functional>
#include <memory>
#include <atomic>
class AdminPrivilegeManager
{
public:
    ///
    /// \brief 确保程序以管理员权限运行，如果不是则尝试提升权限
    /// \param actionDescription 需要管理员权限的操作描述（用于UAC提示）
    /// \return bool 如果当前已是管理员或成功提升权限返回true，否则false
    ///
    static bool EnsureAdminPrivilege(const std::wstring& actionDescription = L"执行需要管理员权限的操作");

    ///
    /// \brief 检查当前是否以管理员权限运行
    ///
    static bool IsRunningAsAdmin();

    ///
    /// \brief 以管理员身份重新启动当前程序
    ///
    static bool RestartAsAdmin();
};


//////////////////////////////////////////////////////////////////


/**
 * @brief 现代化 Windows 服务框架
 */
class ModernWindowsService
{
public:
    struct ServiceConfig
    {
        std::wstring serviceName;
        std::wstring displayName;
        std::wstring description;
        std::wstring executablePath;
        DWORD        startType   = SERVICE_DEMAND_START;
        DWORD        serviceType = SERVICE_WIN32_OWN_PROCESS;
    };

    using ServiceMainCallback    = std::function<void()>;
    using ServiceControlCallback = std::function<void(DWORD)>;

    ModernWindowsService(const ServiceConfig& config);
    ~ModernWindowsService();

    bool Run();

    /// 服务管理静态方法
    static bool InstallService(const ServiceConfig& config);
    static bool UninstallService(const std::wstring& serviceName);
    static bool StartService(const std::wstring& serviceName);
    static bool StopService(const std::wstring& serviceName);
    static bool IsServiceRunning(const std::wstring& serviceName);

    /// 设置回调函数
    void SetServiceMainHandler(ServiceMainCallback callback);
    void SetServiceControlHandler(ServiceControlCallback callback);

    /// 工具函数
    static std::wstring GetExecutablePath();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;

    /// 单例支持
    static ModernWindowsService* instance_;
    static void                  SetInstance(ModernWindowsService* instance);
    static ModernWindowsService* GetInstance();
};


#endif // MODERNWINDOWSSERVICE_H
