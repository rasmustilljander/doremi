#pragma once

namespace DoremiEngine
{
    namespace Core
    {
        /**
            TODO docs
        */
        namespace EngineModuleEnum
        {
            /* For bitwise operations enum is actually better than enum class */
            enum EngineModuleEnumReal
            {
                AUDIO = 1,
                GRAPHIC = 2,
                NETWORK = 4,
                PHYSICS = 8,
                SCRIPT = 16,
                INPUT = 32,
                ALL = AUDIO | GRAPHIC | NETWORK | PHYSICS | SCRIPT | INPUT
            };
        };
    }
}