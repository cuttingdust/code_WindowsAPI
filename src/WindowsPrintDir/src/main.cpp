#include <algorithm>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>

//////////////////////////////////////////////////////////////////

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

/**
 * @brief 获取当前工作目录
 * @return 当前目录的字符串，如果获取失败返回空字符串
 */
std::string GetCurrentDir()
{
    char  buffer[MAX_PATH] = { 0 };
    DWORD result           = ::GetCurrentDirectoryA(MAX_PATH, buffer);
    return (result == 0 || result > MAX_PATH) ? "" : std::string(buffer);
}

/**
 * @brief 格式化文件大小，转换为易读的格式 (B, KB, MB)
 * @param fileSize 文件大小（字节）
 * @return 格式化后的文件大小字符串
 */
std::string FormatFileSize(LARGE_INTEGER fileSize)
{
    if (fileSize.QuadPart == 0)
    {
        return "(空文件)";
    }
    else if (fileSize.QuadPart < 1024)
    {
        return "(" + std::to_string(fileSize.QuadPart) + " B)";
    }
    else if (fileSize.QuadPart < 1024 * 1024)
    {
        return "(" + std::to_string(fileSize.QuadPart / 1024) + " KB)";
    }
    else
    {
        return "(" + std::to_string(fileSize.QuadPart / (1024 * 1024)) + " MB)";
    }
}


/**
 * @brief 检查文件扩展名是否匹配
 * @param fileName 文件名
 * @param extFilter 扩展名过滤器，如 "txt", "cpp", "*" 等
 * @return 如果匹配返回true，否则返回false
 */
bool MatchFileExtension(const std::string& fileName, const std::string& extFilter)
{
    /// 如果过滤器是 "*" 或空，匹配所有文件
    if (extFilter == "*" || extFilter.empty())
    {
        return true;
    }

    /// 如果是目录，总是匹配（目录不受扩展名过滤影响）
    /// 这里我们无法直接判断是否是目录，所以需要在调用处结合目录属性判断

    /// 查找文件扩展名
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos == std::string::npos)
    {
        /// 没有扩展名的文件，只有当过滤器为 "*" 时匹配
        return extFilter == "*";
    }

    /// 提取扩展名（不包含点）
    std::string fileExt = fileName.substr(dotPos + 1);

    /// 转换为小写进行比较（不区分大小写）
    std::string filterLower  = extFilter;
    std::string fileExtLower = fileExt;

    std::ranges::transform(filterLower, filterLower.begin(), ::tolower);
    std::ranges::transform(fileExtLower, fileExtLower.begin(), ::tolower);

    return fileExtLower == filterLower;
}


/**
 * @brief 检查是否需要跳过当前文件
 * @param fileName 文件名
 * @param fileAttributes 文件属性
 * @param showHidden 是否显示隐藏文件
 * @param extFilter 扩展名过滤器
 * @param isDirectory 是否是目录
 * @return 如果需要跳过返回true，否则返回false
 */
bool ShouldSkipFile(const std::string& fileName, DWORD fileAttributes, bool showHidden, const std::string& extFilter,
                    bool isDirectory)
{
    /// 跳过特殊目录
    if (fileName == "." || fileName == "..")
    {
        return true;
    }

    /// 跳过隐藏文件（如果不需要显示）
    if (!showHidden && (fileAttributes & FILE_ATTRIBUTE_HIDDEN))
    {
        return true;
    }

    /// 如果是文件，检查扩展名过滤
    if (!isDirectory && !MatchFileExtension(fileName, extFilter))
    {
        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////


/**
 * @brief 树形打印目录结构（支持扩展名过滤）
 * @param path 要打印的目录路径，如果为NULL则使用当前目录
 * @param ext 文件扩展名过滤，默认"*"表示所有文件，如 "txt", "cpp", "exe" 等
 * @param showHidden 是否显示隐藏文件，默认false
 * 
 * 使用深度优先遍历算法，以树形结构显示目录和文件
 * 支持文件大小显示、隐藏文件过滤和扩展名过滤
 */
void PrintDirTree(const char* path = NULL, const char* ext = "*", bool showHidden = false)
{
    /// 1. 初始化路径和过滤器
    std::string currentPath = (path == NULL) ? GetCurrentDir() : std::string(path);
    std::string extFilter   = (ext == NULL) ? "*" : std::string(ext);

    if (currentPath.empty())
    {
        std::cout << "错误: 无法获取或访问目录路径" << std::endl;
        return;
    }

    std::cout << "目录: " << currentPath;
    if (extFilter != "*")
    {
        std::cout << " [过滤: *." << extFilter << "]";
    }
    std::cout << std::endl;

    /// 2. 定义遍历状态结构
    struct DirState
    {
        std::string      path;         /// 当前目录完整路径
        std::string      prefix;       /// 树形显示前缀（用于构建缩进）
        HANDLE           findHandle;   /// 文件查找句柄
        WIN32_FIND_DATAA findData;     /// 当前找到的文件数据
        bool             hasMoreFiles; /// 是否还有更多文件待处理
    };

    /// 3. 初始化栈和计数器
    std::stack<DirState> dirStack;
    int                  fileCount = 0, dirCount = 0;

    /// 4. 创建根目录状态并压栈
    DirState rootState;
    rootState.path   = currentPath;
    rootState.prefix = "";

    std::string searchPattern = currentPath + "\\*";
    rootState.findHandle      = FindFirstFileA(searchPattern.c_str(), &rootState.findData);
    rootState.hasMoreFiles    = (rootState.findHandle != INVALID_HANDLE_VALUE);

    if (rootState.hasMoreFiles)
    {
        dirStack.push(rootState);
    }

    /// 5. 主循环：处理目录栈
    while (!dirStack.empty())
    {
        DirState& current = dirStack.top();

        /// 5.1 处理当前目录的文件
        if (current.hasMoreFiles)
        {
            std::string fileName       = current.findData.cFileName;
            DWORD       fileAttributes = current.findData.dwFileAttributes;
            bool        isDirectory    = (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

            /// 检查是否需要跳过当前文件
            if (ShouldSkipFile(fileName, fileAttributes, showHidden, extFilter, isDirectory))
            {
                /// 直接移动到下一个文件
                current.hasMoreFiles = (FindNextFileA(current.findHandle, &current.findData) != 0);
                if (!current.hasMoreFiles && current.findHandle != INVALID_HANDLE_VALUE)
                {
                    FindClose(current.findHandle);
                }
                continue;
            }

            /// 检查下一个文件是否存在（用于确定前缀符号）
            WIN32_FIND_DATAA nextFileData;
            bool             hasNextFile = (FindNextFileA(current.findHandle, &nextFileData) != 0);

            /// 5.2 构建并显示当前项
            std::string displayLine = current.prefix + (hasNextFile ? "├── " : "└── ") + fileName;

            if (isDirectory)
            {
                std::cout << displayLine << " [DIR]" << std::endl;
                dirCount++;
            }
            else
            {
                /// 显示文件大小信息
                LARGE_INTEGER fileSize;
                fileSize.LowPart  = current.findData.nFileSizeLow;
                fileSize.HighPart = current.findData.nFileSizeHigh;

                std::cout << displayLine << " " << FormatFileSize(fileSize) << std::endl;
                fileCount++;
            }

            /// 5.3 如果是目录，创建新状态并深入遍历
            if (isDirectory)
            {
                DirState newDir;
                newDir.path = current.path + "\\" + fileName;

                /// 构建子目录前缀：根据是否有后续文件决定使用"│   "或"    "
                newDir.prefix = current.prefix + (hasNextFile ? "│   " : "    ");

                /// 初始化子目录的查找状态
                std::string subDirPattern = newDir.path + "\\*";
                newDir.findHandle         = FindFirstFileA(subDirPattern.c_str(), &newDir.findData);
                newDir.hasMoreFiles       = (newDir.findHandle != INVALID_HANDLE_VALUE);

                if (newDir.hasMoreFiles)
                {
                    /// 更新当前状态并压入新目录
                    current.findData     = nextFileData;
                    current.hasMoreFiles = hasNextFile;
                    dirStack.push(newDir);
                    continue; /// 立即开始处理新目录
                }
            }

            /// 5.4 移动到下一个文件
            current.findData     = nextFileData;
            current.hasMoreFiles = hasNextFile;

            if (!current.hasMoreFiles && current.findHandle != INVALID_HANDLE_VALUE)
            {
                FindClose(current.findHandle);
            }
        }
        else
        {
            /// 5.5 当前目录处理完成，弹出栈
            dirStack.pop();
        }
    }

    /// 6. 输出统计信息
    std::cout << std::endl;
    std::cout << "统计: " << dirCount << " 个目录, " << fileCount << " 个文件";
    if (extFilter != "*")
    {
        std::cout << " [*." << extFilter << " 文件]";
    }
    std::cout << std::endl;
}


/**
 * @brief 简化版树形目录打印（支持扩展名过滤）
 * @param path 目录路径
 * @param ext 扩展名过滤器
 * 
 * 更简洁的版本，专注于目录结构展示
 */
void SimpleDirTree(const char* path = NULL, const char* ext = "*")
{
    std::string currentPath = (path == NULL) ? GetCurrentDir() : std::string(path);
    std::string extFilter   = (ext == NULL) ? "*" : std::string(ext);

    if (currentPath.empty())
    {
        std::cout << "错误: 无法获取目录路径" << std::endl;
        return;
    }

    std::cout << "目录树: " << currentPath;
    if (extFilter != "*")
    {
        std::cout << " [过滤: *." << extFilter << "]";
    }
    std::cout << std::endl;

    struct SimpleState
    {
        std::string      path;
        std::string      prefix;
        HANDLE           findHandle;
        WIN32_FIND_DATAA findData;
        bool             hasMore;
    };

    std::stack<SimpleState> stack;

    /// 初始化根目录
    SimpleState root = { .path = currentPath, .prefix = "", .findHandle = NULL, .findData = {}, .hasMore = false };
    root.findHandle  = FindFirstFileA((currentPath + "\\*").c_str(), &root.findData);
    root.hasMore     = (root.findHandle != INVALID_HANDLE_VALUE);

    if (root.hasMore)
        stack.push(root);

    int dirCount = 0, fileCount = 0;

    while (!stack.empty())
    {
        SimpleState& current = stack.top();

        if (current.hasMore)
        {
            std::string name        = current.findData.cFileName;
            DWORD       attributes  = current.findData.dwFileAttributes;
            bool        isDirectory = (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

            /// 跳过特殊目录和不符合扩展名过滤的文件
            if (name == "." || name == ".." || (!isDirectory && !MatchFileExtension(name, extFilter)))
            {
                current.hasMore = (FindNextFileA(current.findHandle, &current.findData) != 0);
                if (!current.hasMore && current.findHandle != INVALID_HANDLE_VALUE)
                {
                    FindClose(current.findHandle);
                }
                continue;
            }

            /// 检查下一个文件
            WIN32_FIND_DATAA nextData;
            bool             hasNext = (FindNextFileA(current.findHandle, &nextData) != 0);

            /// 显示当前项
            std::cout << current.prefix << (hasNext ? "├── " : "└── ") << name;
            if (isDirectory)
                std::cout << " [DIR]";
            std::cout << std::endl;

            /// 处理子目录
            if (isDirectory)
            {
                dirCount++;
                SimpleState newState;
                newState.path       = current.path + "\\" + name;
                newState.prefix     = current.prefix + (hasNext ? "│   " : "    ");
                newState.findHandle = FindFirstFileA((newState.path + "\\*").c_str(), &newState.findData);
                newState.hasMore    = (newState.findHandle != INVALID_HANDLE_VALUE);

                if (newState.hasMore)
                {
                    current.findData = nextData;
                    current.hasMore  = hasNext;
                    stack.push(newState);
                    continue;
                }
            }
            else
            {
                fileCount++;
            }

            current.findData = nextData;
            current.hasMore  = hasNext;

            if (!current.hasMore && current.findHandle != INVALID_HANDLE_VALUE)
            {
                FindClose(current.findHandle);
            }
        }
        else
        {
            stack.pop();
        }
    }

    std::cout << std::endl;
    std::cout << "统计: " << dirCount << " 个目录, " << fileCount << " 个文件";
    if (extFilter != "*")
    {
        std::cout << " [*." << extFilter << " 文件]";
    }
    std::cout << std::endl;
}

/// 测试函数
int main()
{
    std::cout << "=== 显示所有文件 ===" << std::endl;
    // PrintDirTree();
    SimpleDirTree();

    // std::cout << "\n=== 只显示 .exe 文件 ===" << std::endl;
    // PrintDirTree(NULL, "exe");

    // std::cout << "\n=== 只显示 .cpp 文件 ===" << std::endl;
    // PrintDirTree(NULL, "cpp");

    // std::cout << "\n=== 只显示 .txt 文件 (简化版) ===" << std::endl;
    // SimpleDirTree(NULL, "txt");

    // std::cout << "\n=== 只显示 .h 文件 ===" << std::endl;
    // PrintDirTree(NULL, "h");

    return 0;
}
