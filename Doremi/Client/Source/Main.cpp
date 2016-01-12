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
        MeasureTimer& measureTimer = MeasureTimer::GetInstance();
        measureTimer.GetTimer("client").Reset().Start();

        Doremi::GameMain gameMain = Doremi::GameMain();
        gameMain.Start();

        measureTimer.GetTimer("client").Stop();
        measureTimer.DumpData("client");
        ConsoleManager::Shutdown();
    }
    catch(const std::exception& e)
    {
        std::cout << "Unhandled exception: " << e.what() << std::endl;
        // TODORT log
    }
    catch(...)
    {
        std::cout << "Unhandled unknown exception" << std::endl;
        // TODORT log
    }

#ifdef _DEBUG
// system("pause"); TODOLH removed to enable fast program shutdown with esc this calls forces you to cin in console window to shut down after esc
#endif
    return 0;
}