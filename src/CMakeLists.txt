cmake_minimum_required(VERSION 3.12)

# 使用 vcpkg 工具链文件 Warning  工具连设置必须是在project 前面
set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(XWindowsAPI)

# 查找 MASM 编译器
if(MSVC)
    enable_language(ASM_MASM)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(MSVC)
set(CMAKE_GENERATOR_PLATFORM x64)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++23
# C++26. CMake 3.25 and later recognize 26 as a valid value, no version has support for any compiler.
set(CMAKE_CXX_STANDARD 23) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

# OpenMP
find_package(OpenMP)
message("OpenMP_FOUND = ${OpenMP_FOUND}")

if(OpenMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# MFC
find_package(MFC)
message("MFC_FOUND = ${MFC_FOUND}")

if(MFC_FOUND)
# 1: static MFC library, 2: shared MFC library
set(CMAKE_MFC_FLAG 2)
endif()

message(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")

# 外部依赖项
set(DPS_INCLUDES)
set(DPS_TARGETS)
set(DPS_LIBRARYS)

# 资源文件

# #图标
set(RC_FILE)

# #qrc 文件
set(QRC_SOURCE_FILES)

# 忽略特定库
if(MSVC)
SET(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:"libcmt.lib;libc.lib")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CommonTools)


#############################################
add_subdirectory(CreateProcess)
add_subdirectory(CreateToolhelp32Snapshot)
add_subdirectory(ExitProcess)
add_subdirectory(TerminateProcess)
add_subdirectory(MemChange)
add_subdirectory(MemChange_Testor)
add_subdirectory(CreateMutex)
add_subdirectory(ConsoleHide)
###############################################
add_subdirectory(CreateThread)
add_subdirectory(ExitThread)
add_subdirectory(TerminateThread)
add_subdirectory(SetThreadPriority)
add_subdirectory(CountErr) # 临界区 原子操作
add_subdirectory(EventErr) # 事件 信号量 同步
################################################
add_subdirectory(FindWindow)
add_subdirectory(HelloWindows)
add_subdirectory(WindowsDemo)
add_subdirectory(WindowsTyper)
add_subdirectory(WindowsTimer)
add_subdirectory(WindowsSSP_Test)
add_subdirectory(WindowsSSP_GUI)
################################################
add_subdirectory(WindowsDlg)
add_subdirectory(WindowsCommon)
add_subdirectory(WindowsContorlDemo)
################################################
add_subdirectory(WindowsMFCDemo)
add_subdirectory(WindowsButtonMFC)
add_subdirectory(WindowsMemoryInfo)
################################################
add_subdirectory(WindowsEncrypt)


# # 拷贝assert目录到输出目录
# set(ASSERT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/assert)
# file(COPY ${ASSERT_PATH} DESTINATION ${OUT_RUN_PATH})
