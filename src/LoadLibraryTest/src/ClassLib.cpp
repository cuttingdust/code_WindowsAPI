#include "ClassLib.h"
#include <iostream>

#include <Windows.h>


int ClassLib::count = 0;
ClassLib::ClassLib()
{
    std::cout << "ClassLib Create" << std::endl;
}


ClassLib::~ClassLib()
{
}
