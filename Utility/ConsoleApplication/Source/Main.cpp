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

inline string getTwoDigit(const int& value)
{
    if(value < 10)
    {
        return string("0" + value);
    }
    return to_string(value);
}

inline string buildLogFileName(const string& p_consoleName)
{
    struct tm now;
    time_t time_create = time(NULL);
    localtime_s(&now, &time_create);

    string fileName = string("logs/");
    fileName.append(getTwoDigit(now.tm_year + 1900) + "-");
    fileName.append(getTwoDigit(now.tm_mon + 1) + "-");
    fileName.append(getTwoDigit(now.tm_mday) + "_");
    fileName.append(getTwoDigit(now.tm_hour) + ".");
    fileName.append(getTwoDigit(now.tm_min));
    fileName.append("__" + p_consoleName);
    fileName.append(".txt");
    return fileName;
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
        // TODORT Handle client and server logs
        consoleName = string(argv[1]);
        color = atoi(argv[2]);
        writeToConsole = atoi(argv[3]);
        writeToFile = atoi(argv[4]);

        // For some reason this represents the pipe, gg
        hStdin = GetStdHandle(STD_INPUT_HANDLE);
        if(hStdin == INVALID_HANDLE_VALUE)
        {
#ifdef _DEBUG
            system("pause");
#endif
            ExitProcess(1); // TODORT more info
        }
    }
    else
    {
#ifdef _DEBUG
        system("pause");
#endif
        ExitProcess(1); // TODORT more info
    }

    if(writeToConsole)
    {
        out = CreateFile(TEXT("CONOUT$"), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

        if(out == INVALID_HANDLE_VALUE)
        {
            writeToConsole = false;
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
        const string& fileName = buildLogFileName(consoleName);

        // Create the file
        fileHandle = CreateFile(s2ws(fileName).c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);
        // TODO Add check to create another logfile if one already exists, because two clients may be runned at the same computer.

        if(fileHandle == INVALID_HANDLE_VALUE)
        {
            writeToFile = false;
        }
    }

    if(!writeToFile || !writeToConsole)
    {
#ifdef _DEBUG
        system("pause");
#endif
        ExitProcess(1); // TODORT more info
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
    return 0;
}