#include <ClassLib.h>

#include <iostream>
#include <stdio.h>
#include <windows.h>


int main(void)
{
    ClassLib cl;
    cl.count++;
    std::cout << "count = " << cl.count << std::endl;

    getchar();
    return 0;
}
