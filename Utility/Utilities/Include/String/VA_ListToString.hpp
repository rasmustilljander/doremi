#pragma once
#include <cstdarg>
#include <string>

namespace Doremi
{
    namespace Utilities
    {
        namespace String
        {

            /**
            Code found on stackoverflow.
            Converts valist to string.
            \param dst String to store formatted message
            \param format Format of message
            \param ap Variable argument list
            */
            void toString(std::string& dst, const char* format, std::va_list ap) throw();

            /**
            Code found on stackoverflow.
            Converts valist to string.
            \param dst String to store formatted message
            \param format Format of message
            \param ap Variable argument list
            */
            void toString(std::string& dst, const char* format, ...) throw();
        }
    }
}