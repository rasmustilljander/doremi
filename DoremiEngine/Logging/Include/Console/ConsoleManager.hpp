#pragma once

#include <DoremiEngine/Logging/Include/Console/Console.hpp>
#include <DoremiEngine/Logging/Include/Logger/LogLevel.hpp>
#include <DoremiEngine/Logging/Include/Logger/LogTag.hpp>
#include <DoremiEngine/Logging/Include/Console/ConsoleColor.hpp>
#include <string>

namespace DoremiEngine
{
    namespace Logging
    {
        class ConsoleManager
        {
        public:
            /**
                Create a new console with the given parameters, returns an already existing console if one exists with the given name.
            */
            virtual Console& CreateNewConsole(const std::string& p_consoleName = "standard", const LogTag& p_logtag = LogTag::NOTAG,
                                              const ConsoleColor& p_textColor = ConsoleColorEnum::WHITE,
                                              const ConsoleColor& p_backgroundColor = ConsoleColorEnum::BLACK) = 0;

            /**
                Returns an already existing console, throws exception if no console exists.
            */
            virtual Console& GetConsole(const std::string& p_consoleName = "standard") = 0;
        };
    }
}