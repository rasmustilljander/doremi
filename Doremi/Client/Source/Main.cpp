// Project specific
#include <Game.hpp>

// Third party

// Standard libraries
#ifdef _WIN32
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include <thread> // std::this_thread::sleep_for
#include <chrono> // std::chrono::seconds
#else
#error Platform not supported
#endif
#include <exception>
#include <iostream>
#include <Utility\MemoryManager\Include\MainMemoryManager.hpp>
#include <Utility/DebugLog/Include/VirtualConsole.hpp>
#ifdef _WIN32
// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int main(int argc, const char* argv[])
#else
#error Platform not supported
#endif
{
    FreeConsole();
    Utility::DebugLog::VirtualConsole a;
    Utility::DebugLog::VirtualConsole b;
    a.Initialize();
    b.Initialize("physics", true, true, Utility::DebugLog::ConsoleColorEnum::YELLOW);
    int i = 0;
    int j = 0;
    while(true)
    {
        a.LogText(Utility::DebugLog::LogTag::NETWORK, Utility::DebugLog::LogLevel::INIT_PRINT, "Test %d", ++i);
        b.LogText(Utility::DebugLog::LogTag::WATER, Utility::DebugLog::LogLevel::FATAL_ERROR, "T3st %d", ++j);


        //      Utility::MemoryManager::MainMemoryManager::Startup();
        //      auto& a = Utility::MemoryManager::MainMemoryManager::GetInstance();
        //    Utility::MemoryManager::MainMemoryManager::Shutdown();
        if(i == 10)
        {
            break;
        }
    }

    try
    {
        Doremi::GameMain gameMain = Doremi::GameMain();
        gameMain.Start();
    }
    catch(const std::exception& e)
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