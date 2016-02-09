#pragma once
#include <fstream>

namespace Doremi
{
    namespace Utilities
    {
        namespace Chrono
        {
            class Timer;
        }
        namespace Logging
        {
            enum class LogTag;
            struct LogTextData;
        }
    }
}

class SpecificLogFile
{
public:
    SpecificLogFile();
    virtual ~SpecificLogFile();

    void Initialize(const Doremi::Utilities::Logging::LogTag& p_logTag);

    void Write(const Doremi::Utilities::Logging::LogTextData& p_data);

private:
    void BuildLogFile(const std::string& p_fileName);
    void OpenFileStream(const std::string& p_fileName);
    std::ofstream* m_fileStream;
    Doremi::Utilities::Chrono::Timer* m_timer;
    double m_flushTimerLimit;
    double m_elapsedTime;
};