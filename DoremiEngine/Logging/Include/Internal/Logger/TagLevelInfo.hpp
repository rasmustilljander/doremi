#pragma once
#include <string>

namespace DoremiEngine
{
    namespace Logging
    {
        struct TagLevelInfo // TODORT move to hpp
        {
            TagLevelInfo(const std::string& p_name) : name(p_name) {}
            TagLevelInfo() {}
            std::string name;
        };
    }
}