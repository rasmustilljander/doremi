#pragma once
#include <Windows.h>
#include <string>
#include <map>

namespace Utility
{
    namespace DebugLog
    {
        enum class LogLevel // TODORT move to hpp
        {
            FATAL_ERROR,
            NON_FATAL_ERROR,
            WARNING,
            SUCCESS,
            DEBUG_PRINT,
            INIT_PRINT,
            START_PRINT,
            PINK_PRINT,
            PACKET_PRINT,
            MASS_DATA_PRINT,
            NOLEVEL,
            HELP_PRINT,
            IDENTIFY_PRINT,
        };

        enum class ConsoleColor // TODORT move to hpp
        {
            BLACK = 0,
            DARK_BLUE = 1,
            DARK_GREEN = 2,
            DARK_AQUA = 3,
            DARK_CYAN = 3,
            DARK_RED = 4,
            DARK_PURPLE = 5,
            DARK_PINK = 5,
            DARK_MANGENTA = 5,
            DARK_YELLOW = 6,
            DARK_WHITE = 7,
            GRAY = 8,
            BLUE = 9,
            GREEN = 10,
            AQUA = 11,
            CYAN = 11,
            RED = 12,
            PURPLE = 13,
            PINK = 13,
            MANGENTA = 13,
            YELLOW = 14,
            WHITE = 15
        };

        enum class LogTag // TODORT move to hpp
        {
            RENDER,
            NETWORK,
            CLIENT,
            SERVER,
            GENERAL,
            NOTAG,
            PHYSICS,
            TOOLS,
            SOUND,
            GAME,
            COMPONENT,
            GUI,
            INPUT,
            RESOURCE,
            SCRIPT,
            ANIMATION,
            WATER,
            PARTICLE
        };

        struct TagLevelInfo // TODORT move to hpp
        {
            TagLevelInfo(std::string p_name, bool p_enabled) : Name(p_name), Enabled(p_enabled) {}
            TagLevelInfo() {}
            std::string Name;
            bool Enabled;
        };

        class VirtualConsole
        {
            public:
            VirtualConsole(const std::string& p_applicationName, const size_t& p_color);
            virtual ~VirtualConsole();

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
            void WriteToConsole(const std::string& p_func, const size_t& p_line, const LogTag& p_tag, const LogLevel& p_vLevel, const std::string& p_VAListAsString);

            std::map<LogTag, TagLevelInfo> m_tagInfo;
            std::map<LogLevel, TagLevelInfo> m_levelInfo;

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
