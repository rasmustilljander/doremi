#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <string>

using std::string;

namespace Utility
{
    namespace DebugLog
    {
        ///
        /// \breif Format message
        /// \param dst String to store formatted message
        /// \param format Format of message
        /// \param ap Variable argument list
        ///
        void toString(string& dst, const char* format, va_list ap) throw()
        {
            int length;
            va_list apStrLen;
            va_copy(apStrLen, ap);
            length = vsnprintf(NULL, 0, format, apStrLen);
            va_end(apStrLen);
            if(length > 0)
            {
                dst.resize(length);
                vsnprintf((char*)dst.data(), dst.size() + 1, format, ap);
            }
            else
            {
                dst = "Format error! format: ";
                dst.append(format);
            }
        }

        ///
        /// \breif Format message
        /// \param dst String to store formatted message
        /// \param format Format of message
        /// \param ... Variable argument list
        ///
        void toString(string& dst, const char* format, ...) throw()
        {
            va_list ap;
            va_start(ap, format);
            toString(dst, format, ap);
            va_end(ap);
        }
    }
}