#include <VirtualConsole.hpp>
#include <Windows.h>
#include <string>
#include <thread>
#include <Internal\VA_LISTHelper.hpp>
namespace Utility
{
    namespace DebugLog
    {
        VirtualConsole::VirtualConsole(const std::string& name, const size_t& color)
        {
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
                throw std::runtime_error("Creating virtualConsole failed.");
            }
            m_process = pi.hProcess;
            CloseHandle(pi.hThread);
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

        void AsynchronousLogText(VirtualConsole& p_console, const std::string& p_func, const size_t& p_line, const LogTag& p_tag,
                                 const LogLevel& p_vLevel, const std::string& p_VAListAsString)
        {
            p_console.WriteToConsole(p_func, p_line, p_tag, p_vLevel, p_VAListAsString);
        }


        void VirtualConsole::LT(std::string p_func, int p_line, LogTag p_tag, LogLevel p_vLevel, const char* p_format, ...)
        {
            va_list args;
            va_start(args, p_format);
            std::string vaListAsString;
            toString(vaListAsString, p_format, args);
            // TODORT use a struct to pass info instead

            std::thread([=]()
                        {
                            AsynchronousLogText(*this, p_func, p_line, p_tag, p_vLevel, vaListAsString);
                        })
                .detach();
        }

        void VirtualConsole::WriteToConsole(const std::string& p_func, const size_t& p_line, const LogTag& p_tag, const LogLevel& p_vLevel,
                                            const std::string& p_VAListAsString)
        {
            DWORD l;
            std::string out = std::string("[" + p_func + ":" + std::to_string(p_line) + "]" + "\t" + p_VAListAsString);
            WriteFile(m_nearEnd, out.c_str(), out.size(), &l, 0);
            return;
        }
    }
}