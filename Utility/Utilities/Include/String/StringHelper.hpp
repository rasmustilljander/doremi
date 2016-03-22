#pragma once

#include <string>
#include <codecvt>
#include <cstdint>
#include <vector>
#include <sstream>

namespace Doremi
{
    namespace Utilities
    {
        namespace String
        {
            /**
                TODO docs
                TODO remove
            */
            inline std::wstring s2ws(const std::string& str)
            {
                using namespace std;
                typedef codecvt_utf8<wchar_t> convert_typeX;
                wstring_convert<convert_typeX, wchar_t> converterX;
                return converterX.from_bytes(str);
            }

            /**
            TODO docs
            */
            inline std::wstring StringToWideString(const std::string& str)
            {
                using namespace std;
                typedef codecvt_utf8<wchar_t> convert_typeX;
                wstring_convert<convert_typeX, wchar_t> converterX;
                return converterX.from_bytes(str);
            }

            inline std::string WideStringToString(const std::wstring& wstr)
            {
                typedef std::codecvt_utf8<wchar_t> convert_typeX;
                std::wstring_convert<convert_typeX, wchar_t> converterX;

                return converterX.to_bytes(wstr);
            }

            /**
            Builds a string from a integer value and returns at least two digits.
            Ex. Input 0 Output 00
            Ex. Input 1 Output 01
            Ex. Input 9 Output 09
            Ex. Input 10 Output 10
            */
            inline std::string intToStringAtleastTwoDigits(const uint32_t& value)
            {
                using namespace std;
                if(value < 10)
                {
                    return string("0" + std::to_string(value));
                }

                return to_string(value);
            }

            inline void StringSplit(const std::string& s, char delim, std::vector<std::string>& elems)
            {
                using namespace std;
                stringstream ss(s);
                string item;
                while(getline(ss, item, delim))
                {
                    elems.push_back(item);
                }
            }
        }
    }
}