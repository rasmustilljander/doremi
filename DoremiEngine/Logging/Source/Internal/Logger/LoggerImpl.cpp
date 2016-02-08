#include <Internal/Logger/LoggerImpl.hpp>
#include <Utility/Utilities/Include/Logging/LogLevel.hpp>
#include <Utility/Utilities/Include/Logging/LogTag.hpp>
#include <Utility/Utilities/Include/Logging/LogTextData.hpp>
#include <Utility/Utilities/Include/Logging/LogLevelConverter.hpp>

#include <Internal/VA_LISTHelper.hpp>
#include <iostream>

namespace DoremiEngine
{
    namespace Logging
    {
        using namespace Doremi::Utilities::Logging;
        LoggerImpl::LoggerImpl()
        {
            /*
            m_levelInfo[LogLevel::FATAL_ERROR] = TagLevelInfo("FATAL_ERROR");
            m_levelInfo[LogLevel::NON_FATAL_ERROR] = TagLevelInfo("NON_FATAL_ERROR");
            m_levelInfo[LogLevel::WARNING] = TagLevelInfo("WARNING");
            m_levelInfo[LogLevel::MASS_DATA_PRINT] = TagLevelInfo("MASS_DATA_PRINT");
            m_levelInfo[LogLevel::NOLEVEL] = TagLevelInfo("NOLEVEL");
            */
        }

        LoggerImpl::~LoggerImpl() {}

        void LoggerImpl::LT(const std::string& p_function, const size_t& p_line, const LogTag& p_tag, const LogLevel& p_vLevel, const char* p_format, ...)
        {
            // TODORT expand
            va_list args;
            va_start(args, p_format);
            LogTextData* threadData = new LogTextData(); // TODORT delete allocation
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
