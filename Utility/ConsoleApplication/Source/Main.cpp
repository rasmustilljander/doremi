#include <windows.h>

#define BUFSIZE 4096
int main(int argc, char** argv)
{
    CHAR chBuf[BUFSIZE];
    DWORD dwRead;
    HANDLE hStdin;
    BOOL fSuccess;

    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if(hStdin == INVALID_HANDLE_VALUE) ExitProcess(1);

    HANDLE out = CreateFile(TEXT("CONOUT$"), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(out == INVALID_HANDLE_VALUE) return 1;
    if(argc > 1)
    {
        WORD color = atoi(argv[1]);
        SetConsoleTextAttribute(out, color);
    }

    while(1)
    {
        fSuccess = ReadFile(hStdin, chBuf, BUFSIZE, &dwRead, NULL);
        if(!fSuccess || !dwRead) break;
        WriteFile(out, chBuf, dwRead, &dwRead, 0);
    }
}