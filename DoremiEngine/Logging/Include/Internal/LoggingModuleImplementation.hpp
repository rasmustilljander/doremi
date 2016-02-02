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
                TODO docs
            */
            LoggingModuleImplementation();

            /**
                TODO docs
            */
            virtual ~LoggingModuleImplementation();

            /**
                  TODO docs
            */
            void Startup() override;

            /**
              TODO docs
            */
            void Shutdown() override;
        };
    }
}