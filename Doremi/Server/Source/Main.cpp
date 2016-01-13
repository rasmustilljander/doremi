// Project specific
#include <Server.hpp>
#include <Utility/Timer/Include/Measure/MeasureTimer.hpp>
#include <Utility/DebugLog/Include/ConsoleManager.hpp>

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
    // TODORT
    // This row is required later as it disables the standard output terminal, we do now want that.
    // However, as people are currently using cout for debugging we'll need this for the moment.
    AllocConsole();

    try
    {
        using namespace Utility::Timer;
        using namespace Utility::DebugLog;
        ConsoleManager::Startup();
        MeasureTimer& measure = MeasureTimer::GetInstance();
        measure.GetTimer(FILE_AND_FUNC).Start();

        // The game
        Doremi::ServerMain serverMain = Doremi::ServerMain();
        serverMain.Start();

        measure.GetTimer(FILE_AND_FUNC).Stop();
        MeasureTimer::GetInstance().DumpData("clientTiming");
        ConsoleManager::Shutdown();
    }
    catch(std::exception e)
    {
        // TODORT more information
        std::cout << "Unhandled exception: " << e.what() << std::endl;
#ifdef _DEBUG
        std::cin.get();
#endif
    }
    catch(...)
    {
        // TODORT more information
        std::cout << "Unhandled unknown exception" << std::endl;
#ifdef _DEBUG
        std::cin.get();
#endif
    }
    return 0;
}