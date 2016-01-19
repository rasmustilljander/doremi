#pragma once
// Interface class
#include <TriggerManager.hpp>

// Third party
#include <DirectXMath.h>
#include <PhysX/PxPhysicsAPI.h>

// Standard libraries
#include <unordered_map>

using namespace physx;
using namespace std;
using namespace DirectX;
namespace DoremiEngine
{
    namespace Physics
    {
        struct InternalPhysicsUtils;
        class TriggerManagerImpl : public TriggerManager
        {
        public:
            TriggerManagerImpl(InternalPhysicsUtils& p_utils);
            virtual ~TriggerManagerImpl();

            void AddTrigger(int p_id) override;

        private:
            InternalPhysicsUtils& m_utils;
        };
    }
}