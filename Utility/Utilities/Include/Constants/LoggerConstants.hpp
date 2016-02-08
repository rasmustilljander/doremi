#pragma once
#include <string>

namespace Doremi
{
    namespace Utilities
    {
        namespace Constants
        {
            const size_t FILEMAP_SIZE = 1024;
            const std::string FILEMAP_NAME = std::string("doremi_filemap");
            const size_t FILEMAP_TIMEOUT = 10;
            const size_t LONGEST_FUNCTION_NAME = 256;
            const size_t LONGEST_MESSAGE_NAME = 256;
            const size_t LONGEST_LINE_NAME = 256;
            const std::string FILEMAP_MUTEX_NAME = std::string("doremi_filemap_mutex");
            const std::string LOGGING_PROCESS_NAME = std::string("LoggerProcess");
        }
    }
}