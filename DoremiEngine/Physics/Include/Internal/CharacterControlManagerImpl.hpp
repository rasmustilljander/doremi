#pragma once
#include <CharacterControlManager.hpp>
#include <PhysX/PxPhysicsAPI.h>

#include <unordered_map>
using namespace std;
using namespace physx;
namespace DoremiEngine
{
    namespace Physics
    {
        struct InternalPhysicsUtils;
        class CharacterControlManagerImpl : public CharacterControlManager
        {
            public:
            CharacterControlManagerImpl(InternalPhysicsUtils& p_utils);
            virtual ~CharacterControlManagerImpl();

            int AddController(int p_id, XMFLOAT3 p_position, XMFLOAT2 p_dimensions) override;
            int MoveController(int p_id, XMFLOAT3 p_discplacement, float p_dt) override;

            XMFLOAT3 GetPosition(int p_id) override;
            XMFLOAT4 GetOrientation(int p_id) override;

            private:
            InternalPhysicsUtils& m_utils;

            unordered_map<int, PxController*> m_controllers;

            PxControllerManager* m_manager;
        };
    }
}