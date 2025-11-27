#include "ModernWindowsService.h"
#include <iostream>
#include <sstream>

bool AdminPrivilegeManager::EnsureAdminPrivilege(const std::wstring& actionDescription)
{
    if (IsRunningAsAdmin())
    {
        return true;
    }

    std::wcerr << L"需要管理员权限来" << actionDescription << std::endl;

    /// 尝试以管理员身份重新启动
    if (RestartAsAdmin())
    {
        std::wcout << L"正在以管理员权限重新启动..." << std::endl;
        return true; /// 程序会重新启动，当前实例退出
    }
    else
    {
        std::wcerr << L"无法以管理员权限重新启动，请手动以管理员身份运行此程序" << std::endl;
        return false;
    }
}

bool AdminPrivilegeManager::IsRunningAsAdmin()
{
    BOOL isAdmin    = FALSE;
    PSID adminGroup = nullptr;

    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (::AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0,
                                   0, &adminGroup))
    {
        if (::CheckTokenMembership(nullptr, adminGroup, &isAdmin))
        {
            ::FreeSid(adminGroup);
            return isAdmin == TRUE;
        }
        ::FreeSid(adminGroup);
    }

    return false;
}

bool AdminPrivilegeManager::RestartAsAdmin()
{
    wchar_t modulePath[MAX_PATH] = { 0 };
    if (::GetModuleFileName(nullptr, modulePath, MAX_PATH) == 0)
    {
        return false;
    }

    SHELLEXECUTEINFO shellExecuteInfo = { 0 };
    shellExecuteInfo.cbSize           = sizeof(SHELLEXECUTEINFO);
    shellExecuteInfo.lpVerb           = L"runas"; /// 请求管理员权限
    shellExecuteInfo.lpFile           = modulePath;
    shellExecuteInfo.lpParameters     = L""; /// 可以添加命令行参数
    shellExecuteInfo.nShow            = SW_SHOWNORMAL;
    shellExecuteInfo.fMask            = SEE_MASK_NOCLOSEPROCESS;

    if (::ShellExecuteEx(&shellExecuteInfo))
    {
        /// 成功启动管理员进程，退出当前实例
        ::ExitProcess(0);
        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////


ModernWindowsService* ModernWindowsService::instance_ = nullptr;

class ModernWindowsService::Impl
{
public:
    Impl(const ServiceConfig& config);
    ~Impl();

    bool RunService();
    void ServiceMain(DWORD argc, wchar_t** argv);
    void ControlHandler(DWORD controlCode);
    void ReportStatus(DWORD state, DWORD exitCode = 0, DWORD waitHint = 0);

    static VOID WINAPI ServiceMainEntry(DWORD argc, wchar_t* argv[]);
    static VOID WINAPI ControlHandlerEntry(DWORD controlCode);

public:
    ServiceConfig          config_;
    SERVICE_STATUS         status_;
    SERVICE_STATUS_HANDLE  statusHandle_;
    ServiceMainCallback    serviceMainCallback_;
    ServiceControlCallback serviceControlCallback_;
    std::atomic<bool>      isRunning_;
};

ModernWindowsService::Impl::Impl(const ServiceConfig& config) :
    config_(config), statusHandle_(nullptr), isRunning_(false)
{
    ZeroMemory(&status_, sizeof(status_));
    status_.dwServiceType      = SERVICE_WIN32_OWN_PROCESS;
    status_.dwCurrentState     = SERVICE_STOPPED;
    status_.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    status_.dwWin32ExitCode    = NO_ERROR;
}

ModernWindowsService::Impl::~Impl()
{
    if (statusHandle_)
    {
        ReportStatus(SERVICE_STOPPED);
    }
}

bool ModernWindowsService::Impl::RunService()
{
    SERVICE_TABLE_ENTRY serviceTable[] = { { const_cast<LPWSTR>(config_.serviceName.c_str()), ServiceMainEntry },
                                           { nullptr, nullptr } };

    BOOL result = ::StartServiceCtrlDispatcher(serviceTable);
    if (!result)
    {
        DWORD error = ::GetLastError();
        if (error == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
        {
            // 以控制台模式运行
            std::wcout << L"以控制台模式运行服务..." << std::endl;
            if (serviceMainCallback_)
            {
                serviceMainCallback_();
            }
            return true;
        }
        else
        {
            std::wcerr << L"启动服务控制分发器失败，错误代码: " << error << std::endl;
            return false;
        }
    }

    return true;
}

VOID WINAPI ModernWindowsService::Impl::ServiceMainEntry(DWORD argc, wchar_t* argv[])
{
    auto instance = ModernWindowsService::GetInstance();
    if (instance && instance->pImpl)
    {
        instance->pImpl->ServiceMain(argc, argv);
    }
}

VOID WINAPI ModernWindowsService::Impl::ControlHandlerEntry(DWORD controlCode)
{
    auto instance = ModernWindowsService::GetInstance();
    if (instance && instance->pImpl)
    {
        instance->pImpl->ControlHandler(controlCode);
    }
}

void ModernWindowsService::Impl::ServiceMain(DWORD argc, wchar_t** argv)
{
    statusHandle_ = ::RegisterServiceCtrlHandlerEx(config_.serviceName.c_str(),
                                                   (LPHANDLER_FUNCTION_EX)ControlHandlerEntry, nullptr);

    if (!statusHandle_)
    {
        return;
    }

    ReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    /// 报告服务运行中
    ReportStatus(SERVICE_RUNNING);

    /// 执行服务主逻辑
    isRunning_ = true;
    if (serviceMainCallback_)
    {
        serviceMainCallback_();
    }

    /// 服务主循环
    while (isRunning_)
    {
        Sleep(1000);
    }

    ReportStatus(SERVICE_STOPPED);
}

void ModernWindowsService::Impl::ControlHandler(DWORD controlCode)
{
    switch (controlCode)
    {
        case SERVICE_CONTROL_STOP:
        case SERVICE_CONTROL_SHUTDOWN:
            isRunning_ = false;
            ReportStatus(SERVICE_STOP_PENDING);
            break;
        case SERVICE_CONTROL_INTERROGATE:
            ReportStatus(status_.dwCurrentState);
            break;
        default:
            break;
    }

    if (serviceControlCallback_)
    {
        serviceControlCallback_(controlCode);
    }
}

void ModernWindowsService::Impl::ReportStatus(DWORD state, DWORD exitCode, DWORD waitHint)
{
    static DWORD checkPoint = 1;

    status_.dwCurrentState  = state;
    status_.dwWin32ExitCode = exitCode;
    status_.dwWaitHint      = waitHint;

    if (state == SERVICE_START_PENDING || state == SERVICE_STOP_PENDING)
    {
        status_.dwCheckPoint = checkPoint++;
    }
    else
    {
        status_.dwCheckPoint = 0;
    }

    ::SetServiceStatus(statusHandle_, &status_);
}

// ModernWindowsService 公共接口实现
ModernWindowsService::ModernWindowsService(const ServiceConfig& config) : pImpl(std::make_unique<Impl>(config))
{
    SetInstance(this);
}

ModernWindowsService::~ModernWindowsService() = default;

bool ModernWindowsService::Run()
{
    return pImpl->RunService();
}

void ModernWindowsService::SetServiceMainHandler(ServiceMainCallback callback)
{
    pImpl->serviceMainCallback_ = callback;
}

void ModernWindowsService::SetServiceControlHandler(ServiceControlCallback callback)
{
    pImpl->serviceControlCallback_ = callback;
}

void ModernWindowsService::SetInstance(ModernWindowsService* instance)
{
    instance_ = instance;
}

ModernWindowsService* ModernWindowsService::GetInstance()
{
    return instance_;
}

/// 服务管理静态方法
bool ModernWindowsService::InstallService(const ServiceConfig& config)
{
    if (!AdminPrivilegeManager::EnsureAdminPrivilege(L"安装服务"))
    {
        return false; /// 没有管理员权限，安装失败
    }

    SC_HANDLE scm = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    if (!scm)
    {
        std::wcerr << L"无法打开服务控制管理器" << std::endl;
        return false;
    }

    std::wstring binPath = L"\"" + config.executablePath + L"\"";

    SC_HANDLE service = ::CreateService(scm, config.serviceName.c_str(), config.displayName.c_str(), SERVICE_ALL_ACCESS,
                                        config.serviceType, config.startType, SERVICE_ERROR_NORMAL, binPath.c_str(),
                                        nullptr, nullptr, nullptr, nullptr, nullptr);

    if (!service)
    {
        DWORD error = ::GetLastError();
        std::wcerr << L"创建服务失败，错误代码: " << error << std::endl;
        ::CloseServiceHandle(scm);
        return false;
    }

    /// 设置服务描述
    if (!config.description.empty())
    {
        SERVICE_DESCRIPTION sd = { const_cast<LPWSTR>(config.description.c_str()) };
        ::ChangeServiceConfig2(service, SERVICE_CONFIG_DESCRIPTION, &sd);
    }

    std::wcout << L"服务安装成功: " << config.serviceName << std::endl;

    ::CloseServiceHandle(service);
    ::CloseServiceHandle(scm);
    return true;
}

bool ModernWindowsService::UninstallService(const std::wstring& serviceName)
{
    if (!AdminPrivilegeManager::EnsureAdminPrivilege(L"卸载服务"))
    {
        return false; /// 没有管理员权限，安装失败
    }

    SC_HANDLE scm = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    if (!scm)
    {
        std::wcerr << L"无法打开服务控制管理器" << std::endl;
        return false;
    }

    SC_HANDLE service = ::OpenService(scm, serviceName.c_str(), DELETE);
    if (!service)
    {
        std::wcerr << L"无法打开服务: " << serviceName << std::endl;
        ::CloseServiceHandle(scm);
        return false;
    }

    /// 先停止服务
    SERVICE_STATUS status;
    ::ControlService(service, SERVICE_CONTROL_STOP, &status);
    Sleep(2000);

    BOOL result = ::DeleteService(service);
    if (result)
    {
        std::wcout << L"服务卸载成功: " << serviceName << std::endl;
    }
    else
    {
        DWORD error = ::GetLastError();
        std::wcerr << L"服务卸载失败，错误代码: " << error << std::endl;
    }

    ::CloseServiceHandle(service);
    ::CloseServiceHandle(scm);
    return result;
}

bool ModernWindowsService::StartService(const std::wstring& serviceName)
{
    if (!AdminPrivilegeManager::EnsureAdminPrivilege(L"卸载服务"))
    {
        return false; /// 没有管理员权限，安装失败
    }

    SC_HANDLE scm = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!scm)
        return false;

    SC_HANDLE service = ::OpenService(scm, serviceName.c_str(), SERVICE_START);
    if (!service)
    {
        ::CloseServiceHandle(scm);
        return false;
    }

    BOOL result = ::StartService(service, 0, nullptr);
    ::CloseServiceHandle(service);
    ::CloseServiceHandle(scm);

    return result;
}

bool ModernWindowsService::StopService(const std::wstring& serviceName)
{
    if (!AdminPrivilegeManager::EnsureAdminPrivilege(L"卸载服务"))
    {
        return false; /// 没有管理员权限，安装失败
    }

    SC_HANDLE scm = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!scm)
        return false;

    SC_HANDLE service = ::OpenService(scm, serviceName.c_str(), SERVICE_STOP);
    if (!service)
    {
        ::CloseServiceHandle(scm);
        return false;
    }

    SERVICE_STATUS status;
    BOOL           result = ::ControlService(service, SERVICE_CONTROL_STOP, &status);
    ::CloseServiceHandle(service);
    ::CloseServiceHandle(scm);

    return result;
}

bool ModernWindowsService::IsServiceRunning(const std::wstring& serviceName)
{
    if (!AdminPrivilegeManager::EnsureAdminPrivilege(L"卸载服务"))
    {
        return false; /// 没有管理员权限，安装失败
    }

    SC_HANDLE scm = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!scm)
        return false;

    SC_HANDLE service = ::OpenService(scm, serviceName.c_str(), SERVICE_QUERY_STATUS);
    if (!service)
    {
        ::CloseServiceHandle(scm);
        return false;
    }

    SERVICE_STATUS status;
    BOOL           result = ::QueryServiceStatus(service, &status);
    ::CloseServiceHandle(service);
    ::CloseServiceHandle(scm);

    return result && status.dwCurrentState == SERVICE_RUNNING;
}

std::wstring ModernWindowsService::GetExecutablePath()
{
    wchar_t path[MAX_PATH];
    ::GetModuleFileName(nullptr, path, MAX_PATH);
    return path;
}
