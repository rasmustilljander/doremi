#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include <Utility/DebugLog/Include/LogLevel.hpp>
#include <Utility/DebugLog/Include/LogTag.hpp>
#include <Utility/DebugLog/Include/ConsoleColor.hpp>

namespace ctpl
{
    class thread_pool;
}

namespace Utility
{
    namespace DebugLog
    {
        struct TagLevelInfo // TODORT move to hpp
        {
            TagLevelInfo(std::string p_name, bool p_enabled) : Name(p_name), Enabled(p_enabled) {}
            TagLevelInfo() {}
            std::string Name;
            bool Enabled;
        };
        struct LoggingData;

        class VirtualConsole
        {
            public:
            VirtualConsole();
            virtual ~VirtualConsole();
            void Initialize(const std::string& p_pipeName = "standard", bool p_writeToConsole = true, bool p_writeToFile = true,
                            const ConsoleColor& p_textColor = ConsoleColorEnum::WHITE, const ConsoleColor& p_backgroundColor = ConsoleColorEnum::BLACK);

            /**
                The actual method called when calling LogText
            */
            void LT(std::string p_func, int p_line, LogTag p_tag, LogLevel p_vLevel, const char* p_format, ...);

            /**
                Dummy method for the intellisense to recognize the method
            */
            void LogText(LogTag p_tag, LogLevel p_vLevel, const char* p_format, ...){};

            private:
            //    void AsynchronousLogText(const std::string& p_func, const size_t& p_line, const LogTag& p_tag, const LogLevel& p_vLevel, const char*
            //    p_format,
            //       va_list p_args, const bool& writeFileLine = false);
            bool CheckTag(LogTag p_tag) { return m_tagInfo[p_tag].Enabled; }
            bool CheckLevel(LogLevel p_level) { return m_levelInfo[p_level].Enabled; }
            public:
            void WriteToConsole(const LoggingData& p_loggingData);

            std::map<LogTag, TagLevelInfo> m_tagInfo;
            std::map<LogLevel, TagLevelInfo> m_levelInfo;

            HANDLE m_nearEnd;
            HANDLE m_farEnd;
            HANDLE m_process;
            ctpl::thread_pool& m_threadPool;
        };
    }
}
/*
A bit of a hack
*/
#define LogText(...) LT(__FUNCTION__, __LINE__, ##__VA_ARGS__)
