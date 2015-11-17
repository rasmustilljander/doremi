#pragma once

// Project specific

// Third party

// Standard libraries
#include <string>

namespace DoremiEngine
{
    namespace Core
    {
        class EngineSubsystem
        {
            public:
            /**
                TODO
            */
            virtual void SetWorkingDirectory(std::string p_workingDirectory) = 0;

            /**
                TODO
            */
            virtual void Shutdown() = 0;
        };
    }
}