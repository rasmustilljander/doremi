#include <SubmoduleManager.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        class SubmoduleManagerImpl : public SubmoduleManager
        {
        public:
            /**
            Constructor, should only be called by LoggingModule.
            */
            SubmoduleManagerImpl();

            /**
            Destructor, should only be called by LoggingModule.
            */
            virtual ~SubmoduleManagerImpl();

            /**
            Fetches the main application logger.
            */
            Logger& GetLogger() override;

            /**
            Fetches the consolemanager which manages theoretical external consoles.
            */
            ConsoleManager& GetConsoleManager() override;

        private:
            Logger* m_logger;
            ConsoleManager* m_consoleManager;
        };
    }
}