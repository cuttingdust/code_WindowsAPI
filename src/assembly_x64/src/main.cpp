#include <iostream>
#include <Windows.h>
/// mycall x64�����ĺ��� ����

extern "C" UINT64 __stdcall mycall(); /// ������˵��
extern "C" UINT64 __stdcall myadd1(); /// ������˵��


int main()
{
#if defined(_WIN64)
    std::cout << "Running on x64 platform." << std::endl;
#else
    std::cout << "Running on x86 platform." << std::endl;
#endif
    printf("mycall����ֵ=%llX\r\n", mycall());
    printf("myadd1����ֵ=%llX\r\n", myadd1());

    getchar();
    return 0;
}
