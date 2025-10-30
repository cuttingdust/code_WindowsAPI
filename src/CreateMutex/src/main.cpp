#include <stdio.h>
#include <Windows.h>

int main(int argc, char *argv[])
{
    printf("In this process!\n");
    HANDLE hMutex = ::CreateMutex(NULL, true, L"XONE");
    if (::GetLastError() == ERROR_ALREADY_EXISTS)
    {
        if (hMutex)
        {
            CloseHandle(hMutex);
        }
        MessageBox(NULL, TEXT("XOne already runing!"), TEXT("XOne"), NULL);
        return -1;
    }
    getchar();

    return 0;
}
