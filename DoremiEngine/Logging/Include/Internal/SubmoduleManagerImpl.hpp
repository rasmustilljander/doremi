#include <SubmoduleManager.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        class SubmoduleManagerImpl : public SubmoduleManager
        {
        public:
            /**
            TODORT docs
            */
            SubmoduleManagerImpl();

            /**
            TODORT docs
            */
            virtual ~SubmoduleManagerImpl();

            /**
            TODORT docs
            */
            Logger& GetLogger() override;

            /**
            TODORT docs
            */
            ConsoleManager& GetConsoleManager() override;

        private:
            Logger* m_logger;
            ConsoleManager* m_consoleManager;
        };
    }
}