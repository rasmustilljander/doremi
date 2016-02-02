// Project specific
#include <Server.hpp>


// Third party

// Standard libraries
#ifdef _WIN32
#include <windows.h>
#include <DbgHelp.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#else
#error Platform not supported
#endif
#include <exception>
#include <iostream>
#include <exception> // std::set_unexpected

// The method called when shutting down
void attemptGracefulShutdown();

// Methods set as unexpected and terminate functions
void g_unexpected();
void g_terminate();

// Prints the current stack
void printStack();

// Start the important singletons
void startup();

// Shutdown the important singletons
void shutdown();

// Controlhandler
BOOL CtrlHandler(DWORD fdwCtrlType);

void localMain()
{
    try
    {
        Doremi::ServerMain gameMain = Doremi::ServerMain();
        gameMain.Start();
    }
    catch(const std::exception& e)
    {
        // TODORT log
        std::cout << "Unhandled exception: " << e.what() << std::endl;
        attemptGracefulShutdown();
        exit(1);
    }
    catch(...)
    {
        // TODORT log
        std::cout << "Unhandled unknown exception" << std::endl;
        attemptGracefulShutdown();
        exit(1);
    }
}


#ifdef _WIN32
int main(int argc, const char* argv[])
#else
#error Platform not supported
#endif
{
    std::set_terminate(g_terminate);
    std::set_unexpected(g_unexpected);
    SetErrorMode(SEM_FAILCRITICALERRORS);
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

    // Run startup code
    startup();

    // TODORT
    // This row is required later as it disables the standard output terminal, we do now want that.
    // However, as people are currently using cout for debugging we'll need this for the moment.
    // FreeConsole();

    __try
    {
        localMain();
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        attemptGracefulShutdown();
        exit(1);
    }

    // Run shutdown code
    shutdown();
    return 0;
}

void g_unexpected() { attemptGracefulShutdown(); }
void g_terminate() { attemptGracefulShutdown(); }

void startup()
{
    // TODORT
    // TODOLOG
    // using namespace Utility::Timer;
    // using namespace Utility::DebugLog;
    // ConsoleManager::Startup();
    // MeasureTimer::GetInstance().GetTimer(FILE_AND_FUNC).Start();
}

void shutdown()
{
    // TODORT
    // TODOLOG
    // using namespace Utility::Timer;
    // using namespace Utility::DebugLog;
    // MeasureTimer::GetInstance().GetTimer(FILE_AND_FUNC).Stop();
    // MeasureTimer::GetInstance().DumpData("client_timings");
    // ConsoleManager::Shutdown();
}

void attemptGracefulShutdown()
{
    printStack();
    shutdown();
#ifdef _DEBUG
    std::cin.get();
#endif
}

#ifdef _WIN32
void printStack()
{
    unsigned int i;
    void* stack[100];
    unsigned short frames;
    SYMBOL_INFO* symbol;
    HANDLE process;

    process = GetCurrentProcess();

    SymInitialize(process, NULL, TRUE);

    frames = CaptureStackBackTrace(0, 100, stack, NULL);
    symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    // TODORT
    // TODOLOG
    // using namespace Utility::DebugLog;
    // VirtualConsole& console = ConsoleManager::GetInstance().CreateNewConsole("stackPrint", false);
    for(i = 0; i < frames; i++)
    {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        // TODORT
        // TODOLOG
        // console.LogText(LogTag::NOTAG, LogLevel::MASS_DATA_PRINT, "%i: %s - 0x%0X\n", frames - i - 1, symbol->Name, symbol->Address);
    }

    free(symbol);
}

BOOL CtrlHandler(DWORD fdwCtrlType)
{
    switch(fdwCtrlType)
    {
        // Handle the CTRL-C signal.
        case CTRL_C_EVENT:
            attemptGracefulShutdown();
            return (TRUE);

        // CTRL-CLOSE: confirm that the user wants to exit.
        case CTRL_CLOSE_EVENT:
            attemptGracefulShutdown();
            return (TRUE);

        default:
            return FALSE;
    }
}


#endif