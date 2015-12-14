#pragma once
#include <cstdarg>
#include <string>

namespace Utility
{
    namespace DebugLog
    {

        using ::std::string;

        ///
        /// \breif Format message
        /// \param dst String to store formatted message
        /// \param format Format of message
        /// \param ap Variable argument list
        ///
        void toString(string& dst, const char* format, va_list ap) throw();

        ///
        /// \breif Format message
        /// \param dst String to store formatted message
        /// \param format Format of message
        /// \param ... Variable argument list
        ///
        void toString(string& dst, const char* format, ...) throw();
    }
}
