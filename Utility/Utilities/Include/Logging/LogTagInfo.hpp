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
                explicit LogTagInfo(const std::string& p_name) : name(p_name) {}
                LogTagInfo() {}
                std::string name;
            };
        }
    }
}