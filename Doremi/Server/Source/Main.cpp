// Project specific


// Third party

// Standard libraries
#ifdef _WIN32
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#else
#error Platform not supported
#endif
#include <exception>
#include <iostream>

int main(int argc, char** argv)
{
    // Might not be the best way of doing console writing
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);

    try
    {
    }
    catch(std::exception e)
    {
        // TODO more information
        std::cout << "Unknown exception: " << e.what() << std::endl;
        std::cin.get();
    }

#ifdef _WIN32
#ifdef _DEBUG
    system("pause");
#endif
#endif
    return 0;
}