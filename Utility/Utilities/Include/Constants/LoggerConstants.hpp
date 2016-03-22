#pragma once
#include <string>
#include <chrono>

namespace Doremi
{
    namespace Utilities
    {
        namespace Constants
        {
            using namespace std::literals;
            const std::string IPC_DEFAULT_FILEMAP_NAME = std::string("doremi_filemap");
            const size_t IPC_FILEMAP_SIZE = 1024 * 10;
            const std::string IPC_FILEMAP_MUTEX_NAME = std::string("doremi_filemap_mutex");
            const double IPC_FILEMAP_TIMEOUT = 120;

            const size_t LONGEST_FUNCTION_NAME = 256;
            const size_t LONGEST_MESSAGE_NAME = 256;
            const size_t LONGEST_LINE_NAME = 256;
            const std::string LOGGING_PROCESS_NAME = std::string("LoggerProcess.exe");
            const double LOGFILE_FLUSH_INTERVAL = 3.0;
            const std::chrono::milliseconds LOGGING_PRODUCE_TIME_WAIT = 5ms;
            const std::chrono::milliseconds LOGGING_CONSUME_TIME_WAIT = 10ms;
        }
    }
}