#pragma once
// Project specific
#include <Manager/MovementManagerClient.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/CharacterEffectComponent.hpp>
#include <EntityComponent/Components/GravityComponent.hpp>
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
        MovementManagerClient::MovementManagerClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "MovementManager")
        {
        }

        MovementManagerClient::~MovementManagerClient() {}

        void MovementManagerClient::Update(double p_dt)
        {
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Movement | (int)ComponentType::CharacterController;

            EntityID p_playerEntityID = 0;
            bool playerExist = PlayerHandler::GetInstance()->GetDefaultPlayerEntityID(p_playerEntityID);

            for(size_t i = 0; i < length; i++)
            {
                // Some debug bools just to test various effects. We'll have these read some other way later
                bool iceEffect = false;
                bool fireEffect = false;

                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    DoremiEngine::Physics::CharacterControlManager& charControlManager = m_sharedContext.GetPhysicsModule().GetCharacterControlManager();
                    /// 1 Get comp
                    MovementComponent* movementComp = EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(i);


                    ///  2 Clamp speed
                    // Clamp down XZ movement to maximum movement speed (we don't mess with y due to jump/gravity)
                    XMVECTOR movementXZVec = XMLoadFloat2(&XMFLOAT2(movementComp->movement.x, movementComp->movement.z));
                    movementXZVec = XMVector2ClampLength(movementXZVec, 0, 0.8f);
                    // Store it back
                    XMFLOAT2 movementXZ;
                    XMStoreFloat2(&movementXZ, movementXZVec);
                    movementComp->movement.x = movementXZ.x;
                    movementComp->movement.z = movementXZ.y;

                    /// 3 Move controller
                    // If player exist and we're player, confirm move with server
                    if(playerExist && p_playerEntityID == i)
                    {
                        XMFLOAT3 position = charControlManager.GetPosition(i);
                        XMFLOAT4 orientation = charControlManager.GetOrientation(i);

                        PositionCorrectionHandler::GetInstance()->QueuePlayerPositionForCheck(position, orientation, movementComp->movement,
                                                                                              InterpolationHandler::GetInstance()->GetRealSnapshotSequence());
                    }
                    // Perform move
                    bool hitGround = charControlManager.MoveController(i, movementComp->movement, p_dt);
                    if(hitGround)
                    {
                        EntityHandler::GetInstance().GetComponentFromStorage<GravityComponent>(i)->travelSpeed = 0;
                    }

                    /// 4 Fix speed for next iteration
                    // Check if we have special things to do because we're affected by character effects
                    if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::CharacterEffect))
                    {
                        CharacterEffectComponent* effectComp = EntityHandler::GetInstance().GetComponentFromStorage<CharacterEffectComponent>(i);
                        int currentEffects = (int)effectComp->effect;
                        // If we're sliding around, only reduce speed, don't entierly reset it
                        if(currentEffects & (int)CharacterEffect::IceSliding)
                        {
                            float iceSlowdownFactor = 0.99f * (1 - p_dt);
                            movementComp->movement.x *= iceSlowdownFactor;
                            movementComp->movement.z *= iceSlowdownFactor;
                        }

                        // If we're not running on fire (or ice) we can stop
                        else if(currentEffects & (int)CharacterEffect::CantStop)
                        {
                            movementComp->movement = XMFLOAT3(0, 0, 0);
                        }

                        // Always reset y movement. Again, we don't mess with y
                        movementComp->movement.y = 0;
                    }
                    else
                    {
                        // We weren't effected in any strange way. Just reset movement
                        movementComp->movement = XMFLOAT3(0, 0, 0);
                    }
                }
                else
                {
                    // Didn't have component. Move on to next entity
                }
            }
        }

        void MovementManagerClient::OnEvent(Event* p_event) {}
    }
}
