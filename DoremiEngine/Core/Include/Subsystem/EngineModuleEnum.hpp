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
            enum EngineModuleEnumReal // TODO Define integer size
            {
                AUDIO = 1,
                GRAPHIC = 2,
                NETWORK = 4,
                PHYSICS = 8,
                SCRIPT = 16,
                INPUT = 32,
                AI = 64,
                ALL = AUDIO | GRAPHIC | NETWORK | PHYSICS | SCRIPT | INPUT | AI
            };
        };
    }
}