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
            LogTag logTag;
            LogLevel logLevel;
            std::string p_message;
            // TODORT Ideally I would like the conversion to happen on the threads, however,
            // I did not manage to fully use the VA_LIST correctly thus some values were copied between threads.
            //    std::string format;
            //    va_list args;
        };

        VirtualConsoleImpl::VirtualConsoleImpl(ctpl::thread_pool& p_threadPool, const std::string& p_pipeName, const bool& p_writeToConsole,
                                               const bool& p_writeToFile, const ConsoleColor& p_textColor, const ConsoleColor& p_backgroundColor)
            : m_threadPool(p_threadPool),
              m_pipeName(p_pipeName),
              m_writeToConsole(p_writeToConsole),
              m_writeToFile(p_writeToFile),
              m_textColor(p_textColor),
              m_backgroundColor(p_backgroundColor)
        {
            m_tagInfo[LogTag::GRAPHIC] = TagLevelInfo("GRAPHIC");
            m_tagInfo[LogTag::NETWORK] = TagLevelInfo("NETWORK");
            m_tagInfo[LogTag::CLIENT] = TagLevelInfo("CLIENT");
            m_tagInfo[LogTag::SERVER] = TagLevelInfo("SERVER");
            m_tagInfo[LogTag::GENERAL] = TagLevelInfo("GENERAL");
            m_tagInfo[LogTag::NOTAG] = TagLevelInfo("NOTAG");
            m_tagInfo[LogTag::PHYSICS] = TagLevelInfo("PHYSICS");
            m_tagInfo[LogTag::GAME] = TagLevelInfo("GAME");
            m_tagInfo[LogTag::COMPONENT] = TagLevelInfo("COMPONENT");
            m_tagInfo[LogTag::GUI] = TagLevelInfo("GUI");
            m_tagInfo[LogTag::INPUT] = TagLevelInfo("INPUT");
            m_tagInfo[LogTag::RESOURCE] = TagLevelInfo("RESOURCE");
            m_tagInfo[LogTag::ANIMATION] = TagLevelInfo("ANIMATION");
            m_tagInfo[LogTag::PARTICLE] = TagLevelInfo("PARTICLE");

            m_levelInfo[LogLevel::FATAL_ERROR] = TagLevelInfo("FATAL_ERROR");
            m_levelInfo[LogLevel::NON_FATAL_ERROR] = TagLevelInfo("NON_FATAL_ERROR");
            m_levelInfo[LogLevel::WARNING] = TagLevelInfo("WARNING");
            m_levelInfo[LogLevel::MASS_DATA_PRINT] = TagLevelInfo("MASS_DATA_PRINT");
            m_levelInfo[LogLevel::NOLEVEL] = TagLevelInfo("NOLEVEL");
        }

        void VirtualConsoleImpl::Initialize()
        {
            BuildConsoleApplicationPath();
            SetupSharedMemory();
            CreateConsoleProcess();
        }

        void VirtualConsoleImpl::BuildConsoleApplicationPath()
        {
            HMODULE hModule = GetModuleHandleW(NULL);
            WCHAR applicationPath[MAX_PATH]; // TODORT Remove WCHAR
            GetModuleFileNameW(hModule, applicationPath, MAX_PATH); // Get the path to this executable.

            std::wstring path = std::wstring(applicationPath); // Put the path into a string
            path = std::wstring(path.begin(), path.end() - 10); // TODORT, This only works for server.exe and client.exe as it's 10 characters.
            path.append(L"ConsoleApplication.exe"); // Add the name of the desired application instead
            m_consoleApplicationPath = path;
        }

        void VirtualConsoleImpl::SetupSharedMemory()
        {
            // TODORT The memory should be owned by another process
            // TODORT Verify comment; Nowdays the shared memory is a named pipe.
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
        }

        void VirtualConsoleImpl::CreateConsoleProcess()
        {
            PROCESS_INFORMATION pi;
            STARTUPINFO si;
            ZeroMemory(&pi, sizeof(pi));
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(STARTUPINFO);
            si.hStdInput = this->m_farEnd;
            si.dwFlags |= STARTF_USESTDHANDLES;

            WCHAR arguments[100]; // TODORT Remove WCHAR
            DWORD color = m_textColor.stateValue + m_backgroundColor.stateValue;
#ifndef UNICODE
// sprintf(arguments, "%d", color);
#error Non-unicode not supported
#else
            std::wstring wPipeName(m_pipeName.begin(), m_pipeName.end());
            // TODORT add argument for client or server
            swprintf(arguments, L"0, %s %d %d %d", wPipeName.c_str(), color, m_writeToConsole, m_writeToFile);
#endif
            if(!CreateProcess(m_consoleApplicationPath.c_str(), arguments, 0, 0, 1, CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, 0, 0, &si, &pi))
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
            threadData->logTag = p_tag;
            threadData->logLevel = p_vLevel;
            toString(threadData->p_message, p_format, args);
            m_threadPool.push(AsynchronousLogText, this, threadData);
            va_end(args);
        }

        void VirtualConsoleImpl::WriteToSharedMemory(const LoggingData& p_loggingData)
        {
            // TODORT Remember to check if oneliner can increase performance
            DWORD l;
            std::string out;
            out.append("[" + m_levelInfo[p_loggingData.logLevel].name + "]" + " ");
            out.append("[" + m_tagInfo[p_loggingData.logTag].name + "]" + " ");
            out.append("[" + p_loggingData.function + ":" + std::to_string(p_loggingData.line) + "]" + " ");
            out.append(p_loggingData.p_message + "\n");
            WriteFile(m_nearEnd, out.c_str(), out.size(), &l, 0);
        }
    }
}