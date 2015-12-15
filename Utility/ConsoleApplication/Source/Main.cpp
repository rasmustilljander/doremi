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
    unsigned long dwRead;
    bool fSuccess;
    HANDLE hStdin;

    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if(hStdin == INVALID_HANDLE_VALUE)
    {
        ExitProcess(1);
    }

    HANDLE out = CreateFile(TEXT("CONOUT$"), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    if(out == INVALID_HANDLE_VALUE)
    {
        return 1;
    }
    string consoleName = string(argv[1]);
    if(argc > 2)
    {
        SetConsoleTitle(s2ws(consoleName).c_str());
        WORD color = atoi(argv[2]);
        SetConsoleTextAttribute(out, color);
    }

    // Create log folder
    CreateDirectory(L"logs", NULL);

    // Build logfile
    struct tm now;
    time_t time_create = time(NULL);
    localtime_s(&now, &time_create);
    consoleName = string(std::to_string(now.tm_hour) + "." + std::to_string(now.tm_min) + "-" + consoleName);
    consoleName.append(".txt");

    ofstream myfile;

    size_t counter = 0;
    myfile.open(string("logs/" + consoleName).c_str());
    while(true)
    {
        fSuccess = ReadFile(hStdin, chBuf, BUFSIZE, &dwRead, NULL);
        if(!fSuccess || !dwRead) break;
        WriteFile(out, chBuf, dwRead, &dwRead, 0);
        myfile << chBuf;
        counter++;
        if(counter == 10)
        {
            myfile.flush();
            counter = 0;
        }
    }
}