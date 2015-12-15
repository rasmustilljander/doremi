#include <Internal/VirtualConsoleImpl.hpp>
#include <Windows.h>
#include <string>
#include <thread>
#include <Internal/VA_LISTHelper.hpp>
#include <iostream>
#include <Utility/DebugLog/Include/Internal/ThreadPool.hpp>
#include <string>

namespace Utility
{
    namespace DebugLog
    {

        struct LoggingData // TODORT, maybe move to .cpp?
        {
            std::string function;
            size_t line;
            LogTag tag;
            LogLevel vLevel;
            std::string p_message;
            // TODORT Ideally I would like the conversion to happen on the threads, however,
            // I did not manage to fully use the VA_LIST correctly thus some values were copied between threads.
            //    std::string format;
            //    va_list args;
        };

        VirtualConsoleImpl::VirtualConsoleImpl(const std::string& p_pipeName, ctpl::thread_pool& p_threadPool)
            : m_pipeName(p_pipeName), m_threadPool(p_threadPool)
        {
        }

        void VirtualConsoleImpl::Initialize(bool p_writeToConsole, bool p_writeToFile, const ConsoleColor& p_textColor, const ConsoleColor& p_backgroundColor)
        {
            if(!p_writeToConsole && !p_writeToFile)
            {
                throw std::runtime_error("Not both outputs can be offline."); // TODORT better message ?
            }

            SECURITY_ATTRIBUTES sa;
            sa.nLength = sizeof(SECURITY_ATTRIBUTES);
            sa.bInheritHandle = 1;
            sa.lpSecurityDescriptor = 0;

            if(!CreatePipe(&m_farEnd, &m_nearEnd, &sa, 0))
            {
                throw std::runtime_error("Creating shared memory failed.");
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

            HMODULE hModule = GetModuleHandleW(NULL);
            WCHAR path[MAX_PATH];
            GetModuleFileNameW(hModule, path, MAX_PATH);
            std::wstring tmpPath = std::wstring(path);
            tmpPath = std::wstring(tmpPath.begin(), tmpPath.end() - 10); // TODORT, better solution
            tmpPath.append(L"ConsoleApplication.exe");

            WCHAR arguments[100];
#ifndef UNICODE
            sprintf(arguments, "%d", color);
#else
            std::wstring wPipeName(m_pipeName.begin(), m_pipeName.end()); // I fucking hate windows
            DWORD color = p_textColor.stateValue + p_backgroundColor.stateValue; // I fucking hate windows
            swprintf(arguments, L"0, %s %d %d %d", wPipeName.c_str(), color, p_writeToConsole, p_writeToFile); // I fucking hate windows
#endif
            if(!CreateProcess(tmpPath.c_str(), arguments, 0, 0, 1, CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, 0, 0, &si, &pi))
            {
                throw std::runtime_error("Creating virtualConsole failed.");
            }
            m_process = pi.hProcess;
            CloseHandle(pi.hThread);
        }

        VirtualConsoleImpl::~VirtualConsoleImpl()
        {
            if(m_nearEnd != INVALID_HANDLE_VALUE)
            {
                if(m_nearEnd != INVALID_HANDLE_VALUE)
                {
                    TerminateProcess(m_process, 0); // TODORT Graceful shutdown may be ok?
                    CloseHandle(m_process);
                }
                CloseHandle(m_nearEnd);
                CloseHandle(m_farEnd);
            }
        }

        void AsynchronousLogText(int id, VirtualConsoleImpl* const p_console, LoggingData* p_data)
        {
            p_console->WriteToSharedMemory(*p_data);
            delete p_data;
        }

        void VirtualConsoleImpl::LT(const std::string& p_function, const size_t& p_line, const LogTag& p_tag, const LogLevel& p_vLevel, const char* p_format, ...)
        {
            va_list args;
            va_start(args, p_format);
            LoggingData* threadData = new LoggingData();
            threadData->function = p_function;
            threadData->line = p_line;
            threadData->tag = p_tag;
            threadData->vLevel = p_vLevel;
            toString(threadData->p_message, p_format, args);
            m_threadPool.push(AsynchronousLogText, this, threadData);
            va_end(args);
        }

        void VirtualConsoleImpl::WriteToSharedMemory(const LoggingData& p_loggingData)
        {
            DWORD l;
            // TODORT better format
            std::string out = std::string("[" + p_loggingData.function + ":" + std::to_string(p_loggingData.line) + "]" + "\t" + p_loggingData.p_message + "\n");
            WriteFile(m_nearEnd, out.c_str(), out.size(), &l, 0);
        }
    }
}