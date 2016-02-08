#include <String/VA_ListToString.hpp>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <string>

using namespace std;

namespace Doremi
{
    namespace Utilities
    {
        namespace String
        {
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

            void toString(string& dst, const char* format, ...) throw()
            {
                va_list ap;
                va_start(ap, format);
                toString(dst, format, ap);
                va_end(ap);
            }
        }
    }
}