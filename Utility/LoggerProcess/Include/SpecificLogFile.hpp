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
    /**
    Constructor
    */
    SpecificLogFile();

    /**
    Destructor
    */
    virtual ~SpecificLogFile();

    /**
    Initializes the logfile with a specific tag, each tag is one file.
    */
    void Initialize(const Doremi::Utilities::Logging::LogTag& p_logTag);

    /**
    Writes to buffer for file, calls flush with regular intervalls.
    */
    void Write(const Doremi::Utilities::Logging::LogTextData& p_data);

    /**
    Forcily flushes the data in the buffer to a file on the disk.
    */
    void Flush();

private:
    void BuildLogFile(const std::string& p_fileName);
    void OpenFileStream(const std::string& p_fileName);
    std::ofstream* m_fileStream;
    Doremi::Utilities::Chrono::Timer* m_timer;
    double m_flushTimerLimit;
    double m_elapsedTime;
};