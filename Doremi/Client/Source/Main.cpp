// Project specific
#include <Game.hpp>

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

#ifdef _WIN32
int main(int argc, const char* argv[])
#else
#error Platform not supported
#endif
{
    // TODORT
    // This row is required later as it disables the standard output terminal, we do now want that.
    // However, as people are currently using cout for debugging we'll need this for the moment.
    // FreeConsole();

    try
    {
        Doremi::GameMain gameMain = Doremi::GameMain();
        gameMain.Start();
    }
    catch(const std::exception& e)
    {
        // TODORT more information
        std::cout << "Unknown exception: " << e.what() << std::endl;
        std::cin.get();
    }

#ifdef _DEBUG
    system("pause");
#endif
    return 0;
}