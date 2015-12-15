#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include <Utility/DebugLog/Include/LogLevel.hpp>
#include <Utility/DebugLog/Include/LogTag.hpp>
#include <Utility/DebugLog/Include/ConsoleColor.hpp>

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

        class VirtualConsole
        {
            public:
            VirtualConsole();
            virtual ~VirtualConsole();

            /**
                The initialize method, can be called with no arguments
                // TODORT I'll need an manager for creating consoles, atm it's possible to create multiple identical
            */
            void Initialize(const std::string& p_pipeName = "standard", bool p_writeToConsole = true, bool p_writeToFile = true,
                            const ConsoleColor& p_textColor = ConsoleColorEnum::WHITE, const ConsoleColor& p_backgroundColor = ConsoleColorEnum::BLACK);

            /**
                Dummy method for the intellisense to recognize the method
            */
            void LogText(LogTag p_tag, LogLevel p_vLevel, const char* p_format, ...){};

            /**
                The actual method called when calling LogText
            */
            void LT(const std::string& p_function, const size_t& p_line, const LogTag& p_tag, const LogLevel& p_vLevel, const char* p_format, ...);

            /**
            Should not be called directly. // TODORT this -should- not be public.
            */
            void WriteToConsole(const LoggingData& p_loggingData);

            private:
            bool CheckTag(LogTag p_tag) { return m_tagInfo[p_tag].Enabled; } // TODORT this is not used
            bool CheckLevel(LogLevel p_level) { return m_levelInfo[p_level].Enabled; } // TODORT this is not used

            private:
            std::map<LogTag, TagLevelInfo> m_tagInfo; // TODORT this is not used
            std::map<LogLevel, TagLevelInfo> m_levelInfo; // TODORT this is not used
            HANDLE m_nearEnd;
            HANDLE m_farEnd;
            HANDLE m_process;
        };
    }
}
/*
A bit of a hack
*/
#define LogText(...) LT(__FUNCTION__, __LINE__, ##__VA_ARGS__)
