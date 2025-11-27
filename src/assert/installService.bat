@echo off
setlocal enabledelayedexpansion

:: 获取脚本所在目录（不受管理员权限影响）
set "script_dir=%~dp0"
set "script_dir=!script_dir:~0,-1!"  :: 去除末尾的反斜杠

echo === Windows 服务管理 ===
echo 脚本真实路径: %script_dir%

set SERVICE_NAME=MyMemoryMonitor
set SERVICE_EXE=WindowsTestService

:: 构建服务路径 - 根据你的项目结构调整
set "SERVICE_PATH=%script_dir%\%SERVICE_EXE%.exe"

:: 如果不在当前目录，尝试上级目录
if not exist "%SERVICE_PATH%" (
    set "SERVICE_PATH=%script_dir%\..\%SERVICE_EXE%.exe"
)

:: 如果还不在，尝试在脚本目录搜索
if not exist "%SERVICE_PATH%" (
    echo 在脚本目录搜索可执行文件...
    for /r "%script_dir%" %%i in (%SERVICE_EXE%.exe) do (
        if exist "%%i" (
            set "SERVICE_PATH=%%i"
            echo 找到文件: !SERVICE_PATH!
            goto :PATH_FOUND
        )
    )
)

:PATH_FOUND
echo 服务路径: %SERVICE_PATH%

if not exist "%SERVICE_PATH%" (
    echo.
    echo [ERROR] 错误: 找不到服务可执行文件!
    echo 请确认:
    echo 1. 项目已编译
    echo 2. 可执行文件名为: %SERVICE_EXE%.exe
    echo 3. 文件位于脚本目录或子目录中
    echo.
    echo 当前搜索范围: %script_dir%
    dir "%script_dir%\*.exe" /b
    echo.
    pause
    exit /b 1
)

:MAIN_MENU
cls
echo.
echo [服务信息]
echo 服务名称: %SERVICE_NAME%
echo 文件路径: %SERVICE_PATH%
echo.
echo [操作菜单]
echo 1. 安装服务
echo 2. 启动服务  
echo 3. 停止服务
echo 4. 删除服务
echo 5. 查看状态
echo 6. 验证文件
echo 7. 退出
echo.
set /p CHOICE=请选择操作 [1-7]:

if "%CHOICE%"=="1" goto INSTALL
if "%CHOICE%"=="2" goto START
if "%CHOICE%"=="3" goto STOP
if "%CHOICE%"=="4" goto DELETE
if "%CHOICE%"=="5" goto QUERY
if "%CHOICE%"=="6" goto VERIFY
if "%CHOICE%"=="7" goto EXIT

echo 无效选择，请重新输入
pause
goto MAIN_MENU

:VERIFY
echo.
echo [文件验证]
echo 脚本目录: %script_dir%
echo 服务文件: %SERVICE_PATH%
if exist "%SERVICE_PATH%" (
    echo [OK] 文件状态: 存在
    echo [SIZE] 文件大小: 
    for %%F in ("%SERVICE_PATH%") do echo     %%~zF 字节
    echo [TIME] 修改时间: 
    for %%F in ("%SERVICE_PATH%") do echo     %%~tF
) else (
    echo [ERROR] 文件状态: 不存在
    echo.
    echo 当前目录文件列表:
    dir "%script_dir%\*.exe" /b
)
pause
goto MAIN_MENU

:INSTALL
echo.
echo [安装服务]
echo 服务名称: %SERVICE_NAME%
echo 文件路径: %SERVICE_PATH%

:: 检查服务是否已存在
sc query "%SERVICE_NAME%" >nul 2>&1
if %errorlevel% equ 0 (
    echo [INFO] 服务已存在，先删除旧服务...
    sc stop "%SERVICE_NAME%" >nul 2>&1
    timeout /t 2 /nobreak >nul
    sc delete "%SERVICE_NAME%" >nul 2>&1
    timeout /t 1 /nobreak >nul
)

echo 正在安装新服务...
sc create "%SERVICE_NAME%" binPath= "%SERVICE_PATH%" DisplayName= "内存监控服务" start= auto
if %errorlevel% equ 0 (
    echo [SUCCESS] 服务安装成功
) else (
    echo [FAILED] 服务安装失败，错误代码: %errorlevel%
)
pause
goto MAIN_MENU

:START
echo.
echo [启动服务]
sc start "%SERVICE_NAME%"
if %errorlevel% equ 0 (
    echo [SUCCESS] 服务启动成功
) else (
    echo [FAILED] 服务启动失败，错误代码: %errorlevel%
    echo 可能原因:
    echo - 服务未安装
    echo - 文件路径错误
    echo - 权限问题
)
pause
goto MAIN_MENU

:STOP
echo.
echo [停止服务]
sc stop "%SERVICE_NAME%"
if %errorlevel% equ 0 (
    echo [SUCCESS] 服务停止成功
) else (
    echo [FAILED] 服务停止失败，错误代码: %errorlevel%
)
pause
goto MAIN_MENU

:DELETE
echo.
echo [删除服务]
sc stop "%SERVICE_NAME%" >nul 2>&1
echo 等待服务停止...
timeout /t 3 /nobreak >nul
sc delete "%SERVICE_NAME%"
if %errorlevel% equ 0 (
    echo [SUCCESS] 服务删除成功
) else (
    echo [FAILED] 服务删除失败，错误代码: %errorlevel%
)
pause
goto MAIN_MENU

:QUERY
echo.
echo [服务状态]
sc query "%SERVICE_NAME%"
if %errorlevel% neq 0 (
    echo [INFO] 服务未安装或不存在
)
pause
goto MAIN_MENU

:EXIT
echo 退出服务管理
exit