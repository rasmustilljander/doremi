#pragma once
#include <string>

namespace Doremi
{
    namespace Utilities
    {
        namespace Logging
        {
            struct LogTagInfo
            {
                LogTagInfo(const std::string& p_name) : name(p_name) {}
                LogTagInfo() {}
                std::string name;
            };
        }
    }
}