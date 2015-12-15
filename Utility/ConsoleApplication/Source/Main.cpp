#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <cstdio>

#include <iostream>
#include <fstream>
using namespace std;

inline wstring s2ws(const std::string& str)
{
    typedef codecvt_utf8<wchar_t> convert_typeX;
    wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.from_bytes(str);
}

#define BUFSIZE 4096
int main(int argc, char** argv)
{
    char chBuf[BUFSIZE];
    unsigned long dwRead = 1;
    BOOL fSuccess = true;
    HANDLE hStdin;
    bool writeToConsole;
    bool writeToFile;
    string consoleName;
    HANDLE out;
    HANDLE fileHandle;
    WORD color;

    if(argc > 4)
    {
        consoleName = string(argv[1]);
        color = atoi(argv[2]);
        writeToConsole = atoi(argv[3]);
        writeToFile = atoi(argv[4]);

        // For some reason this represents the pipe, gg
        hStdin = GetStdHandle(STD_INPUT_HANDLE);
        if(hStdin == INVALID_HANDLE_VALUE)
        {
            ExitProcess(1);
        }
    }
    else
    {
        return 1;
    }

    if(writeToConsole)
    {
        out = CreateFile(TEXT("CONOUT$"), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

        if(out == INVALID_HANDLE_VALUE)
        {
            return 1;
        }
        SetConsoleTitle(s2ws(consoleName).c_str());
        SetConsoleTextAttribute(out, color);
    }
    else
    {
        FreeConsole();
    }

    if(writeToFile)
    {
        // Create log folder
        CreateDirectory(L"logs", NULL);

        // Build logfilename
        struct tm now;
        time_t time_create = time(NULL);
        localtime_s(&now, &time_create);
        consoleName = string("logs/" + std::to_string(now.tm_hour) + "." + std::to_string(now.tm_min) + "-" + consoleName);
        consoleName.append(".txt");

        // Create the file
        fileHandle = CreateFile(s2ws(consoleName).c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);
        if(fileHandle == INVALID_HANDLE_VALUE)
        {
            return 1;
        }
    }

    while(true)
    {
        if(!fSuccess || !dwRead)
        {
            break;
        }
        fSuccess = ReadFile(hStdin, chBuf, BUFSIZE, &dwRead, NULL);

        if(writeToConsole)
        {
            WriteFile(out, chBuf, dwRead, &dwRead, 0);
        }

        if(writeToFile)
        {
            WriteFile(fileHandle, chBuf, dwRead, &dwRead, 0);
        }
    }
    return 1;
}