#include <Internal/Logger/LoggerImpl.hpp>
#include <Internal/Logger/LogginData.hpp>
#include <Internal/VA_LISTHelper.hpp>

#include <iostream>

namespace DoremiEngine
{
    namespace Logging
    {
        LoggerImpl::LoggerImpl()
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

        LoggerImpl::~LoggerImpl() {}

        void LoggerImpl::LT(const std::string& p_function, const size_t& p_line, const LogTag& p_tag, const LogLevel& p_vLevel, const char* p_format, ...)
        {
            // TODORT expand
            va_list args;
            va_start(args, p_format);
            LoggingData* threadData = new LoggingData(); // TODORT delete allocation
            threadData->function = p_function;
            threadData->line = p_line;
            threadData->logTag = p_tag;
            threadData->logLevel = p_vLevel;
            toString(threadData->p_message, p_format, args);
            va_end(args);
            using namespace std;
            cout << threadData->p_message << endl;
        }
    }
}
