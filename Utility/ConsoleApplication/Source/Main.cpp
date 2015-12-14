#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include <codecvt>
#include <iostream>

inline std::wstring s2ws(const std::string& str)
{
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
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
    if(hStdin == INVALID_HANDLE_VALUE) ExitProcess(1);

    HANDLE out = CreateFile(TEXT("CONOUT$"), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(out == INVALID_HANDLE_VALUE) return 1;
    if(argc > 2)
    {
        SetConsoleTitle(s2ws(std::string(argv[1])).c_str());
        WORD color = atoi(argv[2]);
        SetConsoleTextAttribute(out, color);
    }

    while(1)
    {
        fSuccess = ReadFile(hStdin, chBuf, BUFSIZE, &dwRead, NULL);
        if(!fSuccess || !dwRead) break;
        WriteFile(out, chBuf, dwRead, &dwRead, 0);
    }
}