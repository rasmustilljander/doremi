#pragma once
#include <cstdarg>
#include <string>

namespace DoremiEngine
{
    namespace Logging
    {
        using std::string;

        /**
        Code found on stackoverflow.
        Converts valist to string.
        \param dst String to store formatted message
        \param format Format of message
        \param ap Variable argument list
        */
        void toString(string& dst, const char* format, va_list ap) throw();

        /**
        Code found on stackoverflow.
        Converts valist to string.
        \param dst String to store formatted message
        \param format Format of message
        \param ap Variable argument list
        */
        void toString(string& dst, const char* format, ...) throw();
    }
}
