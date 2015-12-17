#pragma once
#include <VirtualConsole.hpp>
#include <LogLevel.hpp>
#include <LogTag.hpp>
#include <ConsoleColor.hpp>
#include <Internal/ThreadPool.hpp>

#include <Windows.h>
#include <string>
#include <map>

namespace Utility
{
    namespace DebugLog
    {
        // TODORT this is not used
        struct TagLevelInfo // TODORT move to hpp
        {
            TagLevelInfo(std::string p_name, bool p_enabled) : Name(p_name), Enabled(p_enabled) {}
            TagLevelInfo() {}
            std::string Name;
            bool Enabled;
        };
        struct LoggingData;

        class VirtualConsoleImpl : public VirtualConsole
        {
            public:
            VirtualConsoleImpl(ctpl::thread_pool& p_threadPool, const std::string& p_pipeName, const bool& p_writeToConsole,
                               const bool& p_writeToFile, const ConsoleColor& p_textColor, const ConsoleColor& p_backgroundColor);

            virtual ~VirtualConsoleImpl();

            /**
                Actually building the console
            */
            void Initialize();

            /**
                The actual method called when calling LogText
            */
            void LT(const std::string& p_function, const size_t& p_line, const LogTag& p_tag, const LogLevel& p_vLevel, const char* p_format, ...) override;

            /**
            Should not be called directly. // TODORT this -should- not be public.
            */
            void WriteToSharedMemory(const LoggingData& p_loggingData);

            private:
            bool CheckTag(LogTag p_tag) { return m_tagInfo[p_tag].Enabled; } // TODORT this is not used
            bool CheckLevel(LogLevel p_level) { return m_levelInfo[p_level].Enabled; } // TODORT this is not used

            private:
            void BuildConsoleApplicationPath();
            void SetupSharedMemory();
            void CreateConsoleProcess();

            // Settings parameters
            bool m_writeToConsole, m_writeToFile;
            ConsoleColor m_textColor, m_backgroundColor;

            // Logging
            std::map<LogTag, TagLevelInfo> m_tagInfo; // TODORT this is not used
            std::map<LogLevel, TagLevelInfo> m_levelInfo; // TODORT this is not used

            // Consoleprocess stuff
            HANDLE m_nearEnd;
            HANDLE m_farEnd;
            HANDLE m_process;

            // Shared memory
            std::string m_pipeName;
            std::wstring m_consoleApplicationPath;

            // Threading
            ctpl::thread_pool& m_threadPool;
        };
    }
}