#include <Windows.h>
#include <stdio.h>

class CMyObj
{
public:
    CMyObj(const char* name) : m_name(name)
    {
        printf("%s 被构造了\n", m_name);
    }
    ~CMyObj()
    {
        printf("%s 被析构了\n", m_name);
    }

private:
    const char* m_name;
};

/// 全局对象
CMyObj globalObj("全局对象");
/// 静态对象
static CMyObj staticObj("静态对象");

/// 终止当前进程也就是进程如何中止自己的运行。方法有两个：
/// – 主线程的入口函数返回：返回时，启动函数调用exit，并将用户的返回值传给它。 exit销毁所有全局的和静态的对象，然后调用ExitProcess促使os终止应用程序。
/// – 程序也可以显式调用ExitProcess，但这样做无法销毁全局的和静态的对象。
/// • VOID ExitProcess(UINT uExitCode);


int main(int argc, char* argv[])
{
    CMyObj localObj("局部对象");
    {
        // printf("main函数正常返回\n");
        // exit(1); /// 直接退出 析构了全局对象和静态资源 但是局部没有
    }

    {
        printf("调用ExitProcess()\n");
        ::ExitProcess(1); /// Windows API /// 没有任何析构
    }

    return 0;
}
