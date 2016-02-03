#pragma once
#include <unordered_map>
#include <LoggingModule.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        class LoggingModuleImplementation : public LoggingModule
        {

        public:
            /**
                Constructor, should only be called by core.
            */
            LoggingModuleImplementation();

            /**
                Destructor, should only be called by core.
            */
            virtual ~LoggingModuleImplementation();

            /**
                Starts the module, all and any initialize code is runned here.
            */
            void Startup() override;

            /**
              Terminates this module gracefully and releases all memory which this modules is currently responsible for.
            */
            void Shutdown() override;

            /**
              Fetches the submodulemanager which manages different parts within the logging module.
            */
            SubmoduleManager& GetSubModuleManager() override;

        private:
            SubmoduleManager* m_submoduleManager;
        };
    }
}