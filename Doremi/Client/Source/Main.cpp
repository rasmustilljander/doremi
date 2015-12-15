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
// TODORT Example of using the logger
#include <Utility/DebugLog/Include/ConsoleManager.hpp> // Used to create Consoles. // TODORT -maybe- remove later
#include <Utility/DebugLog/Include/VirtualConsole.hpp> // Used to actually interact with consoles. // TODORT -maybe- remove later

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


    /*
        TODORT
        Example of using the logger
    */
    using namespace Utility::DebugLog;
    ConsoleManager::Startup(); // DO NOT REPEAT THIS LINE, REQUIRED ONLY ONCE PER STARTUP
    ConsoleManager& manager = ConsoleManager::GetInstance();


    // Default settings requires no parameters, will be named "standard"
    VirtualConsole& defaultConsole = manager.CreateNewConsole();
    defaultConsole.Initialize();
    defaultConsole.LogText(LogTag::GRAPHIC, LogLevel::DEBUG_PRINT, "This console can be removed from Client Main.cpp.");
    defaultConsole.LogText(LogTag::GRAPHIC, LogLevel::DEBUG_PRINT, "The file results can be found in build/Doremi/Client/logs");
    defaultConsole.LogText(LogTag::GRAPHIC, LogLevel::DEBUG_PRINT, "Beware, to fast logging from main thread may still lag behind.");
    defaultConsole.LogText(LogTag::GENERAL, LogLevel::DEBUG_PRINT, "Text example: %s", "text");


    // Creates an console with the name physics
    VirtualConsole& anotherConsole = manager.CreateNewConsole("physics");
    anotherConsole.Initialize(true, true, ConsoleColorEnum::YELLOW); // Some custom settings
    anotherConsole.LogText(LogTag::GRAPHIC, LogLevel::DEBUG_PRINT, "This console can be removed from Client Main.cpp.");
    anotherConsole.LogText(LogTag::PHYSICS, LogLevel::DEBUG_PRINT, "Decimal example: %d", 1);


    // Creates an console with the name physics
    VirtualConsole& yetAnotherConsole = manager.CreateNewConsole("graphics");
    yetAnotherConsole.Initialize(true, true, ConsoleColorEnum::YELLOW, ConsoleColorEnum::DARK_RED); // Custom settings
    yetAnotherConsole.LogText(LogTag::GRAPHIC, LogLevel::DEBUG_PRINT, "This console can be removed from Client Main.cpp.");
    yetAnotherConsole.LogText(LogTag::GRAPHIC, LogLevel::DEBUG_PRINT, "Float example: %f", 1.4f);

    // There are more types that can be used in formatting.
    
    // Final notes
    // Not perfect, still some problems that we may need to adress in the future.
    // I can see now that some naming conventions should be rethought, later.... //TODORT
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