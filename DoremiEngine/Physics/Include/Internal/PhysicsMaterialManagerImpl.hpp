#pragma once
#include <PhysicsMaterialManager.hpp>
#include <unordered_map>
#include <PhysX/PxPhysicsAPI.h>
using namespace std;
using namespace physx;
namespace DoremiEngine
{
    namespace Physics
    {
        struct InternalPhysicsUtils;
        class PhysicsMaterialManagerImpl : public PhysicsMaterialManager
        {
            public:
            PhysicsMaterialManagerImpl(InternalPhysicsUtils& p_utils);
            virtual ~PhysicsMaterialManagerImpl();

            // Stuff for interface

            int CreateMaterial(float p_staticFriction, float p_dynamicFriction, float p_restitution) override;

            // Stuff for internals
            PxMaterial* GetMaterial(int id);

            private:
            InternalPhysicsUtils& m_utils;
            unordered_map<int, PxMaterial*> m_materials;
            int m_nextMaterial;
        };
    }
}