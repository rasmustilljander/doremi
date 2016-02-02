#pragma once
#include <DoremiEngine/Logging/Include/Logger/LogLevel.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        class Console
        {
        public:
            /**
                Sends data to console
            */
            virtual void WriteToConsole(const LogLevel& p_vLevel, const char* p_format, ...) = 0;
        };
    }
}
