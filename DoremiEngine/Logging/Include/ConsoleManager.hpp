#pragma once

#include <DoremiEngine\Logging\Include\VirtualConsole.hpp>
#include <DoremiEngine\Logging\Include\LogLevel.hpp>
#include <DoremiEngine\Logging\Include\LogTag.hpp>
#include <DoremiEngine\Logging\Include\ConsoleColor.hpp>

#include <unordered_map>
#include <string>
namespace DoremiEngine
{
    namespace Logging
    {
        class ConsoleManager
        {
        public:
            /**
            TODORT docs
            */
            static void Startup();

            /**
            TODORT docs
            */
            static void Shutdown();

            /**
            TODORT docs
            */
            static ConsoleManager& GetInstance();

            /**
            TODORT docs
            */
            ConsoleManager(ConsoleManager const&) = delete;

            /**
            TODORT docs
            */
            void operator=(ConsoleManager const&) = delete;

            /**
                TODORT docs
            */
            VirtualConsole& CreateNewConsole(const std::string& p_consoleName = "standard", bool p_writeToConsole = true, bool p_writeToFile = true,
                                             const ConsoleColor& p_textColor = ConsoleColorEnum::WHITE,
                                             const ConsoleColor& p_backgroundColor = ConsoleColorEnum::BLACK);

            /**
                Returns an already existing console, throws exception if no console exists.
            */
            VirtualConsole& GetConsole(const std::string& p_consoleName = "standard");

        private:
            // Static
            static ConsoleManager* m_instance;

            // Nonstatic
            ConsoleManager();
            virtual ~ConsoleManager();

            std::unordered_map<std::string, VirtualConsole*> m_console;
        };
    }
}