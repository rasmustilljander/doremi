// Project
#include <DynamicLoader.hpp>

// Third party
#include <Windows.h>

// Standard lib

namespace DynamicLoader
{
    void BuildLastError();

    namespace
    {
        char* g_lastError = nullptr;
    }

    void* LoadSharedLibrary(const char* filename)
    {
        BuildLastError();
        return (void*)LoadLibraryA(filename);
    }

    void FreeSharedLibrary(void* library) { FreeLibrary((HMODULE)library); }

    void* LoadProcess(void* library, char* name) { return GetProcAddress((HMODULE)library, name); }

    void BuildLastError()
    {
        // delete g_lastError; //TODORT not sure if this is a memory leak or not.
        DWORD error = ::GetLastError();
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 0, error, 0, (LPSTR)&g_lastError, 0, 0);
    }

    const char* GetLastError() { return g_lastError; }
}