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
            VirtualConsoleImpl(const std::string& p_pipeName, ctpl::thread_pool& p_threadPool);
            virtual ~VirtualConsoleImpl();

            /**
                The initialize method, can be called with no arguments
            */
            void Initialize(bool p_writeToConsole, bool p_writeToFile, const ConsoleColor& p_textColor, const ConsoleColor& p_backgroundColor) override;

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
            std::map<LogTag, TagLevelInfo> m_tagInfo; // TODORT this is not used
            std::map<LogLevel, TagLevelInfo> m_levelInfo; // TODORT this is not used
            HANDLE m_nearEnd;
            HANDLE m_farEnd;
            HANDLE m_process;
            std::string m_pipeName;
            ctpl::thread_pool& m_threadPool;
        };
    }
}
