#pragma once
#include <Utility/Utilities/Include/Logging/LogTagInfo.hpp>
#include <Utility/Utilities/Include/Logging/LogTag.hpp>
#include <map>
#include <exception>
#include <string>

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
                        map[LogTag::AUDIO] = LogTagInfo("AUDIO");
                        map[LogTag::GAME] = LogTagInfo("GAME");
                        map[LogTag::COMPONENT] = LogTagInfo("COMPONENT");
                        map[LogTag::GUI] = LogTagInfo("GUI");
                        map[LogTag::INPUT] = LogTagInfo("INPUT");
                        map[LogTag::TIMER] = LogTagInfo("TIMER");
                        map[LogTag::ANIMATION] = LogTagInfo("ANIMATION");
                        map[LogTag::PARTICLE] = LogTagInfo("PARTICLE");
                        map[LogTag::MEMORY] = LogTagInfo("MEMORY");
                        map[LogTag::ENGINE_CORE] = LogTagInfo("ENGINE_CORE");

                        return map;
                    }
                    const std::map<LogTag, LogTagInfo>& info = createMap();
                }

                /**
                    TORODRT Refactor to use uppercase :C
                    TORODRT Move to cpp to remove includes
                */
                static LogTagInfo convert(LogTag p_logTag)
                {
                    if(Internal::info.count(p_logTag) == 1)
                    {
                        return Internal::info.at(p_logTag);
                    }
                    {
                        const std::string message = std::string("Given LogTag does not exist: ").append(std::to_string(static_cast<int>(p_logTag)));
                        throw std::runtime_error(message);
                    }
                };
            }
        }
    }
}