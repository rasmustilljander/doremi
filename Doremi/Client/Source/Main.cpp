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
#include <exception> // std::set_unexpected

// The method called when shutting down
void attemptGracefulShutdown();

// Methods set as unexpected and terminate functions
void g_unexpected();
void g_terminate();

// Start the important singletons
void startup();

// Shutdown the important singletons
void shutdown();

// Controlhandler
BOOL CtrlHandler(DWORD fdwCtrlType);

namespace
{
    Doremi::GameMain* gameMain = nullptr;
}

void localMain()
{
    try
    {
        gameMain->Start();
    }
    catch(const std::exception& e)
    {
        std::cout << "Unhandled exception: " << e.what() << std::endl;
        attemptGracefulShutdown();
        exit(1);
    }
    catch(...)
    {
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

    // TODORT
    // This row is required later as it disables the standard output terminal, we do now want that.
    // However, as people are currently using cout for debugging we'll need this for the moment.
    // FreeConsole();

    __try
    {
        startup();
        localMain();
        shutdown();
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        attemptGracefulShutdown();
        exit(1);
    }
    printf("Graceful shutdown.");

    return 0;
}

void g_unexpected() { attemptGracefulShutdown(); }
void g_terminate() { attemptGracefulShutdown(); }

void startup()
{
    try
    {
        gameMain = new Doremi::GameMain();
    }
    catch(...)
    {
        printf("Failed with shutdown.");
    }
}

void shutdown()
{
    try
    {
        delete gameMain;
        gameMain = nullptr;
    }
    catch(...)
    {
        printf("Failed with shutdown.");
    }
}

void attemptGracefulShutdown()
{
    try
    {
        gameMain->Stop();
    }
    catch(...)
    {
        printf("Failed with graceful shutdown.");
    }
}

#ifdef _WIN32
BOOL CtrlHandler(DWORD fdwCtrlType)
{
    switch(fdwCtrlType)
    {
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
        case CTRL_CLOSE_EVENT: // CTRL-CLOSE: confirm that the user wants to exit
        case CTRL_C_EVENT: // Handle the CTRL-C signal
            printf("Shutting down\n");
            attemptGracefulShutdown();
            Sleep(10000); // Sleep until 10 seconds, if process has been shutof during this time, this will exit correctly
            return TRUE;
        default:
            return FALSE;
    }
}
#endif