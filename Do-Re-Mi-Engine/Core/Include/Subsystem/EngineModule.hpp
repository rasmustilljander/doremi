#pragma once

// Project specific

// Third party

// Standard libraries
#include <string>

namespace DoremiEngine
{
    namespace Core
    {
        class EngineModule
        {
            public:
            /**
            TODO
            */
            virtual void Startup() = 0;

            /**
                TODO
            */
            virtual void SetWorkingDirectory(const std::string& p_workingDirectory) = 0;

            /**
                TODO
            */
            virtual void Shutdown() = 0;
        };
    }
}