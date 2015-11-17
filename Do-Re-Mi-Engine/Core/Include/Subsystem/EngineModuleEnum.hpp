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
                MEMORY = 4,
                NETWORK = 8,
                PHYSICS = 16,
                SCRIPT = 64,
                ALL = AUDIO | GRAPHIC | MEMORY | NETWORK | PHYSICS | SCRIPT
            };
        };
    }
}