#include <iostream>
#include <Windows.h>

#if defined(_WIN64)
/// mycall x64�����ĺ��� ����
extern "C" UINT64 __stdcall mycall(); /// ������˵��
extern "C" UINT64 __stdcall myadd1(); /// ������˵��
#else
extern "C" UINT32 __stdcall mycall(); /// ������˵��
extern "C" UINT32 __stdcall myadd1(); /// ������˵��
#endif

int main()
{
#if defined(_WIN64)
    std::cout << "Running on x64 platform." << std::endl;
#else
    std::cout << "Running on x86 platform." << std::endl;
#endif
    printf("mycall����ֵ=%llX\r\n", (unsigned long long)mycall());
    printf("myadd1����ֵ=%llX\r\n", (unsigned long long)myadd1());

    getchar();
    return 0;
}
