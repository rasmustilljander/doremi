#include <VirtualConsole.hpp>
#include <Windows.h>
#include <string>
#include <thread>
#include <Internal/VA_LISTHelper.hpp>
#include <iostream>
#include <Utility/DebugLog/Include/Internal/ThreadPool.hpp> // It did like the effect of being forward declared

namespace
{
    static ctpl::thread_pool g_threadPool(8); // TODORT this needs an manager
}

namespace Utility
{
    namespace DebugLog
    {

        struct LoggingData
        {
            std::string function;
            size_t line;
            LogTag tag;
            LogLevel vLevel;
            std::string format;
            va_list args;
        };

        VirtualConsole::VirtualConsole() {}

        void VirtualConsole::Initialize(const std::string& p_pipeName, bool p_writeToConsole, bool p_writeToFile, const ConsoleColor& p_textColor,
                                        const ConsoleColor& p_backgroundColor)
        {
            SECURITY_ATTRIBUTES sa;
            sa.nLength = sizeof(SECURITY_ATTRIBUTES);
            sa.bInheritHandle = 1;
            sa.lpSecurityDescriptor = 0;

            if(!CreatePipe(&m_farEnd, &m_nearEnd, &sa, 0))
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
            DWORD color = p_textColor.stateValue | p_backgroundColor.stateValue;
            swprintf(arguments, L"0, %s %d", p_pipeName, color);
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

        void AsynchronousLogText(int id, VirtualConsole* const p_console, LoggingData* p_data)
        {
            int a = id;
            p_console->WriteToConsole(*p_data);
            //   va_end(p_data->args);
            delete p_data;
        }

        void VirtualConsole::LT(const std::string& p_function, const size_t& p_line, const LogTag& p_tag, const LogLevel& p_vLevel, const char* p_format, ...)
        {
            va_list args;
            va_start(args, p_format);
            LoggingData* threadData = new LoggingData();
            threadData->function = p_function;
            threadData->line = p_line;
            threadData->tag = p_tag;
            threadData->vLevel = p_vLevel;
            //      threadData->format = string(p_format);
            std::string vaListAsString;
            toString(vaListAsString, p_format, args);
            threadData->format = vaListAsString;
            g_threadPool.push(AsynchronousLogText, this, threadData);
            va_end(args);
        }

        void VirtualConsole::WriteToConsole(const LoggingData& p_loggingData)
        {
            DWORD l;

            std::string out = std::string("[" + p_loggingData.function + ":" + std::to_string(p_loggingData.line) + "]" + "\t" + p_loggingData.format + "\n");
            WriteFile(m_nearEnd, out.c_str(), out.size(), &l, 0);
            return;
        }
    }
}