class PrivilegeManager
{
public:
    // 启用调试权限（用于访问系统进程）
    static bool EnableDebugPrivilege()
    {
        HANDLE token;
        if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
        {
            return false;
        }

        TOKEN_PRIVILEGES privileges;
        privileges.PrivilegeCount = 1;
        
        if (!::LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &privileges.Privileges[0].Luid))
        {
            ::CloseHandle(token);
            return false;
        }

        privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        BOOL result = ::AdjustTokenPrivileges(token, FALSE, &privileges, 0, nullptr, nullptr);
        ::CloseHandle(token);
        
        return result == TRUE && ::GetLastError() == ERROR_SUCCESS;
    }

    // 启用备份权限（用于文件操作）
    static bool EnableBackupPrivilege()
    {
        return EnablePrivilege(SE_BACKUP_NAME);
    }

    // 启用还原权限
    static bool EnableRestorePrivilege()
    {
        return EnablePrivilege(SE_RESTORE_NAME);
    }

    // 启用关机权限
    static bool EnableShutdownPrivilege()
    {
        return EnablePrivilege(SE_SHUTDOWN_NAME);
    }

private:
    static bool EnablePrivilege(const wchar_t* privilegeName)
    {
        HANDLE token;
        if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
        {
            return false;
        }

        TOKEN_PRIVILEGES privileges;
        privileges.PrivilegeCount = 1;
        
        if (!::LookupPrivilegeValue(nullptr, privilegeName, &privileges.Privileges[0].Luid))
        {
            ::CloseHandle(token);
            return false;
        }

        privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        BOOL result = ::AdjustTokenPrivileges(token, FALSE, &privileges, 0, nullptr, nullptr);
        ::CloseHandle(token);
        
        return result == TRUE;
    }
};