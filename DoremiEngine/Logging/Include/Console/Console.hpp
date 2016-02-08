#pragma once
#include <Utility/Utilities/Include/Logging/LogLevel.hpp>

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
            virtual void WriteToConsole(const Doremi::Utilities::Logging::LogLevel& p_vLevel, const char* p_format, ...) = 0;
        };
    }
}
