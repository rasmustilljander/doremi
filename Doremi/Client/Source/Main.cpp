#define IDI_MAIN_ICON 101

#define PLATFORM_WINDOWS 1
#define PLATFORM_UNIX 2

#ifdef WIN32
#define PLATFORM PLATFORM_WINDOWS
#else // Add mac as well
#define PLATFORM PLATFORM_UNIX
#endif

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
#include <Utility/Utilities/Include/Test/VariableManager.hpp>
#include <Utility/Utilities/Include/String/StringHelper.hpp>

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

void DefaultArguments()
{
    VariableManager& variableManager = VariableManager::GetVariablesManager();
    variableManager.SetValue<int>("messagesPerUpdate", 0);
    variableManager.SetValue<float>("testLength", 0.0f);
    variableManager.SetValue<bool>("stopAfterInitialize", false);
    variableManager.SetValue<std::string>("message", "");
}

void ParseArguments()
{

    VariableManager& variableManager = VariableManager::GetVariablesManager();

    LPWSTR* szArglist;
    int nArgs = 0;
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

    if(nArgs == 1)
    {
        DefaultArguments();
        return;
    }
    std::vector<std::string> subArgs;
    for(size_t i = 1; i < nArgs; ++i)
    {
        using namespace Doremi::Utilities::String;
        StringSplit(WideStringToString(szArglist[i]), ':', subArgs);
        if(subArgs[0].compare("s") == 0)
        {
            variableManager.SetValue<std::string>(subArgs[1], subArgs[2]);
        }
        else if(subArgs[0].compare("b") == 0)
        {
            variableManager.SetValue<bool>(subArgs[1], std::stoi(subArgs[2]));
        }
        else if(subArgs[0].compare("i") == 0)
        {
            variableManager.SetValue<int>(subArgs[1], std::stoi(subArgs[2]));
        }
        else if(subArgs[0].compare("f") == 0)
        {
            variableManager.SetValue<float>(subArgs[1], std::stof(subArgs[2]));
        }
        subArgs.clear();
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
    FreeConsole();
    __try
    {
        ParseArguments();
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
    catch(std::exception e)
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