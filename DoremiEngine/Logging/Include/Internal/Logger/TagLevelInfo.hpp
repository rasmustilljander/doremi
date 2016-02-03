#pragma once
#include <string>

namespace DoremiEngine
{
    namespace Logging
    {
        struct TagLevelInfo
        {
            TagLevelInfo(const std::string& p_name) : name(p_name) {}
            TagLevelInfo() {}
            std::string name;
        };
    }
}