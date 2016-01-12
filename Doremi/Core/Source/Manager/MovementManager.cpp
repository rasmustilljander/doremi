#pragma once
// Project specific
#include <Manager/MovementManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>
// Engine
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>

// Debug stuff TODOJB remove
#include <iostream>
using namespace std;

namespace Doremi
{
    namespace Core
    {
        MovementManager::MovementManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext) {}

        MovementManager::~MovementManager() {}

        void MovementManager::Update(double p_dt)
        {
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Movement | (int)ComponentType::CharacterController;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    MovementComponent* movementComp = EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(i);
                    m_sharedContext.GetPhysicsModule().GetCharacterControlManager().MoveController(i, movementComp->movement, p_dt);
                    movementComp->movement = XMFLOAT3(0, 0, 0);
                    // RigidBodyComponent* rigidBody = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(i);
                    // XMFLOAT3 currentVelocity = m_sharedContext.GetPhysicsModule().GetRigidBodyManager().GetBodyVelocity(rigidBody->p_bodyID);
                    // XMVECTOR forward = XMLoadFloat3(&movement->direction);
                    // XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
                    // XMVECTOR right = XMVector3Cross(up, forward);
                    // right *= movement->rightAcceleration;
                    // forward *= movement->forwardAcceleration;
                    // XMVECTOR moveForce = right + forward;
                    // XMVECTOR currentVel = XMLoadFloat3(&currentVelocity);
                    //// moveForce -= (XMVector3Length(currentVel)/movement->maxSpeed) * moveForce ;
                    // XMFLOAT3 force;
                    // XMStoreFloat3(&force, moveForce);


                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(rigidBody->p_bodyID, force);
                }
            }
        }

        void MovementManager::OnEvent(Event* p_event) {}
    }
}
