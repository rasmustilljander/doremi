// Project specific
#include <Game.hpp>
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
        using namespace Utility::Timer;
        using namespace Utility::DebugLog;
        ConsoleManager::Startup();

        MeasureTimer& measure = MeasureTimer::GetInstance();
        measure.GetTimer(FILE_AND_FUNC).Start();

        Doremi::GameMain gameMain = Doremi::GameMain();
        gameMain.Start();


        measure.GetTimer(FILE_AND_FUNC).Stop();
        MeasureTimer::GetInstance().DumpData("clientTiming");
        ConsoleManager::Shutdown();
    }
    catch(const std::exception& e)
    {
        std::cout << "Unhandled exception: " << e.what() << std::endl;
// TODORT log
#ifdef _DEBUG
        system("pause");
#endif
    }
    catch(...)
    {
        std::cout << "Unhandled unknown exception" << std::endl;
// TODORT log
#ifdef _DEBUG
        system("pause");
#endif
    }

    return 0;
}