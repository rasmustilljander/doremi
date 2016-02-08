#pragma once
#include <Utility/Utilities/Include/Logging/LogLevelInfo.hpp>
#include <Utility/Utilities/Include/Logging/LogTag.hpp>
#include <map>

namespace Doremi
{
    namespace Utilities
    {
        namespace Logging
        {
            namespace LogLevelConverter
            {
                namespace Internal
                {
                    static std::map<LogTag, LogLevelInfo> createMap()
                    {
                        std::map<LogTag, LogLevelInfo> map;
                        map[LogTag::GRAPHIC] = LogLevelInfo("GRAPHIC");
                        map[LogTag::NETWORK] = LogLevelInfo("NETWORK");
                        map[LogTag::CLIENT] = LogLevelInfo("CLIENT");
                        map[LogTag::SERVER] = LogLevelInfo("SERVER");
                        map[LogTag::GENERAL] = LogLevelInfo("GENERAL");
                        map[LogTag::NOTAG] = LogLevelInfo("NOTAG");
                        map[LogTag::PHYSICS] = LogLevelInfo("PHYSICS");
                        map[LogTag::GAME] = LogLevelInfo("GAME");
                        map[LogTag::COMPONENT] = LogLevelInfo("COMPONENT");
                        map[LogTag::GUI] = LogLevelInfo("GUI");
                        map[LogTag::INPUT] = LogLevelInfo("INPUT");
                        map[LogTag::RESOURCE] = LogLevelInfo("RESOURCE");
                        map[LogTag::ANIMATION] = LogLevelInfo("ANIMATION");
                        map[LogTag::PARTICLE] = LogLevelInfo("PARTICLE");

                        return map;
                    }
                    const std::map<LogTag, LogLevelInfo>& info = createMap();
                }

                static LogLevelInfo getStringValue(LogTag p_logTag) { return Internal::info.at(p_logTag); };
            }
        }
    }
}