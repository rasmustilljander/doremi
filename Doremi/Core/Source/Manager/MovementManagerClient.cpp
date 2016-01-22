#pragma once
// Project specific
#include <Manager/MovementManagerClient.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <PlayerHandler.hpp>
#include <PositionCorrectionHandler.hpp>
#include <InterpolationHandler.hpp>

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
        MovementManagerClient::MovementManagerClient(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "MovementManager") {}

        MovementManagerClient::~MovementManagerClient() {}

        void MovementManagerClient::Update(double p_dt)
        {
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Movement | (int)ComponentType::CharacterController;

            EntityID p_playerEntityID = 0;
            bool playerExist = PlayerHandler::GetInstance()->GetDefaultPlayerEntityID(p_playerEntityID);

            for (size_t i = 0; i < length; i++)
            {
                if (EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    DoremiEngine::Physics::CharacterControlManager &charControlManager = m_sharedContext.GetPhysicsModule().GetCharacterControlManager();
                    MovementComponent* movementComp = EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(i);

                    // If player exist and we're player
                    if (playerExist && p_playerEntityID == i)
                    {
                        XMFLOAT3 position = charControlManager.GetPosition(i);
                        XMFLOAT4 orientation = charControlManager.GetOrientation(i);

                        PositionCorrectionHandler::GetInstance()->QueuePlayerPositionForCheck(position, orientation, movementComp->movement, InterpolationHandler::GetInstance()->GetRealSnapshotSequence());
                    }

                    charControlManager.MoveController(i, movementComp->movement, p_dt);
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

        void MovementManagerClient::OnEvent(Event* p_event) {}
    }
}
