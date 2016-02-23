#include <SubmoduleManager.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        class LoggingModuleImplementation;
        class LoggerImpl;
        class ConsoleManagerImpl;

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
            Init the submodule manager with information regarding the rest of the logging module.
            */
            void Initialize(LoggingModuleImplementation& p_loggingModule);

            /**
            Fetches the main application logger.
            */
            Logger& GetLogger() const override;

            /**
            Fetches the consolemanager which manages theoretical external consoles.
            */
            ConsoleManager& GetConsoleManager() const override;

            LoggingModuleImplementation GetLoggingModule() const;

        private:
            LoggerImpl* m_logger;
            ConsoleManagerImpl* m_consoleManager;
            LoggingModuleImplementation* m_loggingModule;
        };
    }
}