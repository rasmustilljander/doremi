#pragma once
#include <Utility/Utilities/Include/Logging/LogTagInfo.hpp>
#include <Utility/Utilities/Include/Logging/LogTag.hpp>
#include <map>

namespace Doremi
{
    namespace Utilities
    {
        namespace Logging
        {
            namespace LogTagConverter
            {
                namespace Internal
                {
                    static std::map<LogTag, LogTagInfo> createMap()
                    {
                        std::map<LogTag, LogTagInfo> map;
                        map[LogTag::GRAPHIC] = LogTagInfo("GRAPHIC");
                        map[LogTag::NETWORK] = LogTagInfo("NETWORK");
                        map[LogTag::CLIENT] = LogTagInfo("CLIENT");
                        map[LogTag::SERVER] = LogTagInfo("SERVER");
                        map[LogTag::GENERAL] = LogTagInfo("GENERAL");
                        map[LogTag::NOTAG] = LogTagInfo("NOTAG");
                        map[LogTag::PHYSICS] = LogTagInfo("PHYSICS");
                        map[LogTag::GAME] = LogTagInfo("GAME");
                        map[LogTag::COMPONENT] = LogTagInfo("COMPONENT");
                        map[LogTag::GUI] = LogTagInfo("GUI");
                        map[LogTag::INPUT] = LogTagInfo("INPUT");
                        map[LogTag::RESOURCE] = LogTagInfo("RESOURCE");
                        map[LogTag::ANIMATION] = LogTagInfo("ANIMATION");
                        map[LogTag::PARTICLE] = LogTagInfo("PARTICLE");
                        map[LogTag::ENGINE_CORE] = LogTagInfo("ENGINE_CORE");

                        return map;
                    }
                    const std::map<LogTag, LogTagInfo>& info = createMap();
                }

                static LogTagInfo convert(LogTag p_logTag)
                {
                    if(Internal::info.count(p_logTag) == 1)
                    {
                        return Internal::info.at(p_logTag);
                    }
                    throw std::runtime_error("The given logtag does not exist.");
                };
            }
        }
    }
}