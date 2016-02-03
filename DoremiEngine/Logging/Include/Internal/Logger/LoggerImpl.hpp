#pragma once
#include <Logger/Logger.hpp>
#include <Internal/Logger/TagLevelInfo.hpp>
#include <map>

namespace DoremiEngine
{
    namespace Logging
    {

        class LoggerImpl : public Logger
        {
        public:
            /**
                Constructor
            */
            LoggerImpl();

            /**
           Destuctor
            */
            virtual ~LoggerImpl();

            /**
            The actual method called when calling LogText
            */
            void LT(const std::string& p_function, const size_t& p_line, const LogTag& p_tag, const LogLevel& p_vLevel, const char* p_format, ...) override;

        private:
            std::map<LogTag, TagLevelInfo> m_tagInfo;
            std::map<LogLevel, TagLevelInfo> m_levelInfo;
        };
    }
}
