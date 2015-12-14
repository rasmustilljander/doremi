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
#include <Utility\MemoryManager\Include\MainMemoryManager.hpp>
#include <Utility/DebugLog/Include/VirtualConsole.hpp>
#ifdef _WIN32
// int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int main(int argc, const char* argv[])
#else
#error Platform not supported
#endif
{
    // while(true)
    {
        //       Utility::MemoryManager::MainMemoryManager::Startup();
        //      auto& a = Utility::MemoryManager::MainMemoryManager::GetInstance();
        //    Utility::MemoryManager::MainMemoryManager::Shutdown();
    }
    FreeConsole();
    Utility::DebugLog::VirtualConsole a("ConsoleApplication.exe", 15);
    std::string b = "hrj";
    a.LogText(Utility::DebugLog::LogTag::NETWORK, Utility::DebugLog::LogLevel::INIT_PRINT, "Test %s", 5);

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