#include <algorithm>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>


/// 多字节 → 宽字符串
std::wstring A2Wstring(const char* string)
{
    if (string == nullptr || *string == '\0')
    {
        return L"";
    }

    int requiredLen = ::MultiByteToWideChar(CP_ACP, 0, string, -1, nullptr, 0);
    if (requiredLen <= 0)
    {
        return L"";
    }

    wchar_t* wbuf   = new wchar_t[requiredLen];
    int      result = ::MultiByteToWideChar(CP_ACP, 0, string, -1, wbuf, requiredLen);

    std::wstring resultStr;
    if (result > 0)
    {
        resultStr = wbuf;
    }

    delete[] wbuf; /// 确保内存释放
    return resultStr;
}

/// 宽字符串 → 多字节
std::string W2Astring(const wchar_t* wstring)
{
    if (wstring == nullptr || *wstring == L'\0')
    {
        return "";
    }

    int requiredLen = ::WideCharToMultiByte(CP_ACP, 0, wstring, -1, nullptr, 0, nullptr, nullptr);
    if (requiredLen <= 0)
    {
        return "";
    }

    char* buf    = new char[requiredLen];
    int   result = ::WideCharToMultiByte(CP_ACP, 0, wstring, -1, buf, requiredLen, nullptr, nullptr);

    std::string resultStr;
    if (result > 0)
    {
        resultStr = buf;
    }

    delete[] buf;
    return resultStr;
}

//////////////////////////////////////////////////////////////////


static std::string GetCurrentDir()
{
    std::string currentPath;
    char        buffer[MAX_PATH] = { 0 };
    DWORD       result           = ::GetCurrentDirectoryA(MAX_PATH, buffer);
    if (result == 0)
    {
        std::cout << "获取当前目录失败！错误码: " << GetLastError() << std::endl;
    }
    else if (result > MAX_PATH)
    {
        std::cout << "缓冲区太小，需要: " << result << " 字符" << std::endl;
    }
    else
    {
        currentPath = buffer;
    }

    return currentPath;
}


//////////////////////////////////////////////////////////////////


void PrintDirTree(const char* path = NULL, const char* ext = "*", bool showHidden = false)
{
    std::string currentPath = path == NULL ? GetCurrentDir() : path;

    if (currentPath.empty())
    {
        std::cout << "无效的目录路径！" << std::endl;
        return;
    }

    std::cout << "目录: " << currentPath << std::endl;
    std::cout << "." << std::endl;

    /// 使用栈来保存遍历状态
    struct TraversalState
    {
        std::string      path;         /// 当前目录路径
        int              depth;        /// 当前深度
        std::string      prefix;       /// 显示前缀
        HANDLE           findHandle;   /// 查找句柄
        WIN32_FIND_DATAA findData;     /// 当前文件数据
        bool             hasMoreFiles; /// 是否还有更多文件要处理
    };

    std::stack<TraversalState> stack;

    /// 初始化根目录状态
    TraversalState rootState;
    rootState.path   = currentPath;
    rootState.depth  = 0;
    rootState.prefix = "";

    std::string searchPattern = currentPath + "\\*";
    rootState.findHandle      = FindFirstFileA(searchPattern.c_str(), &rootState.findData);
    rootState.hasMoreFiles    = (rootState.findHandle != INVALID_HANDLE_VALUE);

    if (rootState.hasMoreFiles)
    {
        stack.push(rootState);
    }

    int fileCount = 0;
    int dirCount  = 0;

    while (!stack.empty())
    {
        TraversalState& current = stack.top();

        /// 如果当前目录还有文件需要处理
        if (current.hasMoreFiles)
        {
            std::string filename = current.findData.cFileName;

            /// 跳过 "." 和 ".."
            if (filename == "." || filename == "..")
            {
                /// 直接移动到下一个文件
                current.hasMoreFiles = (FindNextFileA(current.findHandle, &current.findData) != 0);
                if (!current.hasMoreFiles)
                {
                    FindClose(current.findHandle);
                }
                continue;
            }

            /// 跳过隐藏文件（如果需要）
            if (!showHidden && (current.findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
            {
                /// 移动到下一个文件
                current.hasMoreFiles = (FindNextFileA(current.findHandle, &current.findData) != 0);
                if (!current.hasMoreFiles)
                {
                    FindClose(current.findHandle);
                }
                continue;
            }

            bool isDirectory = (current.findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

            /// 先检查是否还有更多文件（在当前文件处理前检查下一个）
            WIN32_FIND_DATAA nextData;
            bool             hasNext = (FindNextFileA(current.findHandle, &nextData) != 0);

            /// 构建显示前缀
            std::string displayPrefix = current.prefix;
            if (current.depth > 0)
            {
                displayPrefix += (hasNext ? "├── " : "└── ");
            }

            /// 显示当前项目
            std::cout << displayPrefix << filename;

            if (isDirectory)
            {
                std::cout << " [DIR]";
                dirCount++;
            }
            else
            {
                fileCount++;

                /// 显示文件大小
                LARGE_INTEGER fileSize;
                fileSize.LowPart  = current.findData.nFileSizeLow;
                fileSize.HighPart = current.findData.nFileSizeHigh;

                if (fileSize.QuadPart == 0)
                {
                    std::cout << " (空文件)";
                }
                else if (fileSize.QuadPart < 1024)
                {
                    std::cout << " (" << fileSize.QuadPart << " B)";
                }
                else if (fileSize.QuadPart < 1024 * 1024)
                {
                    std::cout << " (" << (fileSize.QuadPart / 1024) << " KB)";
                }
                else
                {
                    std::cout << " (" << (fileSize.QuadPart / (1024 * 1024)) << " MB)";
                }
            }
            std::cout << std::endl;

            /// 如果是目录，立即开始遍历它
            if (isDirectory)
            {
                TraversalState newState;
                newState.path  = current.path + "\\" + filename;
                newState.depth = current.depth + 1;

                /// 构建新的前缀
                newState.prefix = current.prefix;
                if (current.depth > 0)
                {
                    newState.prefix += (hasNext ? "│   " : "    ");
                }

                std::string newSearchPattern = newState.path + "\\*";
                newState.findHandle          = FindFirstFileA(newSearchPattern.c_str(), &newState.findData);
                newState.hasMoreFiles        = (newState.findHandle != INVALID_HANDLE_VALUE);

                if (newState.hasMoreFiles)
                {
                    /// 更新当前状态，保存下一个文件信息
                    current.findData     = nextData;
                    current.hasMoreFiles = hasNext;

                    stack.push(newState);
                    continue; /// 立即开始处理新目录
                }
            }

            /// 更新当前状态为下一个文件
            current.findData     = nextData;
            current.hasMoreFiles = hasNext;

            if (!current.hasMoreFiles)
            {
                FindClose(current.findHandle);
            }
        }
        else
        {
            /// 当前目录处理完成，弹出栈
            stack.pop();
        }
    }

    std::cout << std::endl;
    std::cout << dirCount << " 个目录, " << fileCount << " 个文件" << std::endl;
}

/// 简化版本：更清晰的树形显示
void SimpleDirTree(const char* path = NULL)
{
    std::string currentPath = path == NULL ? GetCurrentDir() : path;
    if (currentPath.empty())
        return;

    std::cout << currentPath << std::endl;

    struct TreeState
    {
        std::string      path;
        std::string      prefix;
        HANDLE           findHandle;
        WIN32_FIND_DATAA findData;
        bool             hasMoreSiblings;
    };

    std::stack<TreeState> stack;

    TreeState root;
    root.path            = currentPath;
    root.prefix          = "";
    root.hasMoreSiblings = false;
    root.findHandle      = FindFirstFileA((currentPath + "\\*").c_str(), &root.findData);

    if (root.findHandle != INVALID_HANDLE_VALUE)
    {
        stack.push(root);
    }

    while (!stack.empty())
    {
        TreeState& current = stack.top();

        if (current.findHandle != INVALID_HANDLE_VALUE)
        {
            std::string name = current.findData.cFileName;
            if (name != "." && name != "..")
            {
                /// 检查是否有兄弟节点
                WIN32_FIND_DATAA nextData;
                bool             hasNext = FindNextFileA(current.findHandle, &nextData) != 0;

                /// 显示当前节点
                std::cout << current.prefix << (hasNext ? "├── " : "└── ") << name;

                bool isDir = (current.findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                if (isDir)
                    std::cout << " [DIR]";
                std::cout << std::endl;

                /// 如果是目录，立即深入遍历
                if (isDir)
                {
                    TreeState newState;
                    newState.path            = current.path + "\\" + name;
                    newState.prefix          = current.prefix + (hasNext ? "│   " : "    ");
                    newState.hasMoreSiblings = hasNext;
                    newState.findHandle      = FindFirstFileA((newState.path + "\\*").c_str(), &newState.findData);

                    if (newState.findHandle != INVALID_HANDLE_VALUE)
                    {
                        stack.push(newState);
                        continue;
                    }
                }

                /// 移动到下一个文件
                if (!hasNext)
                {
                    FindClose(current.findHandle);
                    current.findHandle = INVALID_HANDLE_VALUE;
                }
                else
                {
                    current.findData = nextData;
                }
            }
            else
            {
                /// 移动到下一个文件
                if (FindNextFileA(current.findHandle, &current.findData) == 0)
                {
                    FindClose(current.findHandle);
                    current.findHandle = INVALID_HANDLE_VALUE;
                }
            }
        }
        else
        {
            stack.pop();
        }
    }
}

/// 测试函数
int main()
{
    std::cout << "=== 真正的树形遍历（遇到目录立即深入）===" << std::endl;
    PrintDirTree();

    // std::cout << "\n=== 简化树形显示 ===" << std::endl;
    // SimpleDirTree();
    //
    // std::cout << "\n=== 显示指定目录 ===" << std::endl;
    // PrintDirTree("C:\\Windows\\System32\\drivers"); /// 显示一个较小的目录

    return 0;
}
