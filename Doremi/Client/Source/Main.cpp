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
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
#error Platform not supported
#endif
{
    // Might not be the best way of doing console writing
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);

    try
    {
        Doremi::GameMain gameMain = Doremi::GameMain();
        gameMain.Start();
    }
    catch(std::exception e)
    {
        // TODO more information
        std::cout << "Unknown exception: " << e.what() << std::endl;
        std::cin.get();
    }

#ifdef _DEBUG
    system("pause");
#endif
    return 0;
}