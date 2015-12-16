#pragma once
#include <Interface/SubModule/PotentialFieldSubModule.hpp>

namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldSubModuleImpl : public PotentialFieldSubModule
        {
            public:
            PotentialFieldSubModuleImpl();
            virtual ~PotentialFieldSubModuleImpl();
            int GetNextStep();
        };
    }
}
