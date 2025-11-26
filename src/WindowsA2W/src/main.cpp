#include <windows.h>
#include <atlconv.h>

#include <iostream>
#include <string>

/// 多字节 → 宽字符串
std::wstring A2Wstring(const char *string)
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

    wchar_t *wbuf   = new wchar_t[requiredLen];
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
std::string W2Astring(const wchar_t *wstring)
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

    char *buf    = new char[requiredLen];
    int   result = ::WideCharToMultiByte(CP_ACP, 0, wstring, -1, buf, requiredLen, nullptr, nullptr);

    std::string resultStr;
    if (result > 0)
    {
        resultStr = buf;
    }

    delete[] buf;
    return resultStr;
}


int main(int argc, char *argv[])
{
    const wchar_t *lp_text = L"";
    std::wstring   temp    = L"";
    if (argc > 1)
    {
        {
            // lp_text = (const wchar_t *)argv[1]; /// 强转是失败的，需要用多字节转宽字符函数
        }

        {
            // USES_CONVERSION;
            // /// W2A  宽字节转多字节  在栈中分配空间 ,在函数退出时释放
            // /// A2W  多字节转宽字节
            // lp_text = A2W(argv[1]); /// 栈上分配内存转换
            // /// 在栈中分配空间 ,在函数退出时释放,for中会栈溢出 内部调用WINAPI -》方法三
            // for (;;)
            // {
            //     A2W(argv[1]);
            // }
        }

        {
            // for (;;)
            // {
            //     A2Wstring(argv[1]); /// 没有泄露
            // }

            // lp_text = A2Wstring(argv[1]).data(); /// 错误的用法，临时对象析构后指针失效

            // const std::wstring &tmp2 = A2Wstring(argv[1]); /// 错误的用法 出括号后临时对象析构 指针失效
            // lp_text                  = tmp2.data();

            ///WinAPI 函数 MultiByteToWideChar
            temp    = A2Wstring(argv[1]);
            lp_text = temp.c_str();
        }
    }
    else
    {
        lp_text = L"测试多字节转宽字符";
    }

    MessageBox(NULL, lp_text, L"标题", NULL);

    return 0;
}
