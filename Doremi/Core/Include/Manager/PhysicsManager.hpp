#pragma once
// Project specific
#include <Manager/Manager.hpp>

namespace Doremi
{
    namespace Core
    {
        class PhysicsManager : public Manager
        {
        public:
            PhysicsManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~PhysicsManager();
            void Update(double p_dt) override;
        };
    }
}

