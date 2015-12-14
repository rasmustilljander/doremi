#include <VirtualConsole.hpp>
#include <Windows.h>
#include <string>
namespace Utility
{
    namespace DebugLog
    {
        VirtualConsole::VirtualConsole(const std::string& name, const size_t& color)
        {
            m_good = 0;
            SECURITY_ATTRIBUTES sa;
            sa.nLength = sizeof(SECURITY_ATTRIBUTES);
            sa.bInheritHandle = 1;
            sa.lpSecurityDescriptor = 0;

            if(!CreatePipe(&this->m_farEnd, &m_nearEnd, &sa, 0))
            {
                throw std::runtime_error("Creating pipe failed.");
                // assert(this->near_end == INVALID_HANDLE_VALUE); // TODORT remove asserts
                return;
            }
            SetHandleInformation(m_nearEnd, HANDLE_FLAG_INHERIT, 0);
            PROCESS_INFORMATION pi;
            STARTUPINFO si;
            ZeroMemory(&pi, sizeof(pi));
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(STARTUPINFO);
            si.hStdInput = this->m_farEnd;
            si.dwFlags |= STARTF_USESTDHANDLES;
            TCHAR program[] = TEXT("C:\\build\\build\\x86\\Debug\\ConsoleApplication.exe");
            TCHAR arguments[100];
#ifndef UNICODE
            sprintf(arguments, "%d", color);
#else
            swprintf(arguments, L"0 %d", color);
#endif
            if(!CreateProcess(program, arguments, 0, 0, 1, CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, 0, 0, &si, &pi))
            {
                throw std::runtime_error("Creating VirtualConsole failed.");
            }
            m_process = pi.hProcess;
            CloseHandle(pi.hThread);
            m_good = true;
        }

        VirtualConsole::~VirtualConsole()
        {
            if(m_nearEnd != INVALID_HANDLE_VALUE)
            {
                if(m_nearEnd != INVALID_HANDLE_VALUE)
                {
                    TerminateProcess(m_process, 0);
                    CloseHandle(m_process);
                }
                CloseHandle(m_nearEnd);
                CloseHandle(m_farEnd);
            }
        }

        void VirtualConsole::LT(std::string p_func, int p_line, LogTag p_tag, LogLevel p_vLevel, const char* p_format, ...)
        {
            va_list args;
            va_start(args, p_format);
            //   if (CheckLevel(p_vLevel) && CheckTag(p_tag))
            {
                WriteToConsole(p_func, p_line, p_tag, p_vLevel, p_format, args);
                //         WriteToFile(p_func, p_line, p_tag, p_vLevel, p_format, args);
            }
            va_end(args);
        }
        void VirtualConsole::WriteToConsole(const std::string& p_func, const size_t& p_line, const LogTag& p_tag, const LogLevel& p_vLevel,
                                            const char* p_format, va_list& p_args, const bool& writeFileLine)
        {
            va_start(p_args, p_format);

            char buffer[1024];
            vsnprintf(buffer, 1023, p_format, p_args);
            DWORD l;
            WriteFile(m_nearEnd, buffer, 1023, &l, 0);
            return;
            std::string output;
            output = "p_format \n";
            vprintf(output.c_str(), p_args);
        }
    }
}