#pragma once
// Project specific
#include <Manager/MovementManagerServer.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>
#include <EntityComponent\Components\GravityComponent.hpp>
#include <EntityComponent/Components/JumpComponent.hpp>
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
        MovementManagerServer::MovementManagerServer(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "MovementManagerServer")
        {
        }

        MovementManagerServer::~MovementManagerServer() {}

        void MovementManagerServer::Update(double p_dt)
        {
            // Some debug bools just to test various effects. We'll have these read some other way later
            bool iceEffect = false;
            bool fireEffect = false;

            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Movement | (int)ComponentType::CharacterController;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    /// 1 Get comp
                    MovementComponent* movementComp = EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(i);

                    /// 2 Clamp speed
                    // Clamp down XZ movement to maximum movement speed (we don't mess with y due to jump/gravity)
                    XMVECTOR movementXZVec = XMLoadFloat2(&XMFLOAT2(movementComp->movement.x, movementComp->movement.z));
                    movementXZVec = XMVector2Normalize(movementXZVec) * movementComp->speed * p_dt;
                    // movementXZVec = XMVector2ClampLength(movementXZVec, 0, 0.8f);
                    // Store it back
                    XMFLOAT2 movementXZ;
                    XMStoreFloat2(&movementXZ, movementXZVec);
                    movementComp->movement.x = movementXZ.x;
                    movementComp->movement.z = movementXZ.y;

                    /// 3 Move controller
                    // Perform move
                    bool hitGround = m_sharedContext.GetPhysicsModule().GetCharacterControlManager().MoveController(i, movementComp->movement, p_dt);
                    if(hitGround)
                    {
                        EntityHandler::GetInstance().GetComponentFromStorage<GravityComponent>(i)->travelSpeed = 0;
                        if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Jump)) // temporary fix
                        {
                            EntityHandler::GetInstance().GetComponentFromStorage<JumpComponent>(i)->active = false;
                        }
                    }

                    /// 4 Fix speed for next iteration
                    // If we're sliding around, only reduce speed, don't entierly reset it
                    if(iceEffect)
                    {
                        float iceSlowdownFactor = 0.99f * (1 - p_dt);
                        movementComp->movement.x *= iceSlowdownFactor;
                        movementComp->movement.z *= iceSlowdownFactor;
                    }

                    // If we're not running on fire (or ice) we can stop
                    else if(!fireEffect)
                    {
                        movementComp->movement = XMFLOAT3(0, 0, 0);
                    }

                    // Always reset y movement. Again, we don't mess with y
                    movementComp->movement.y = 0;


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

        void MovementManagerServer::OnEvent(Event* p_event) {}
    }
}
