#pragma once
// Project specific
#include <PlayerHandler.hpp>
#include <Doremi/Core/Include/InputHandler.hpp>

#include <EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>
#include <EntityComponent/Components/JumpComponent.hpp>
#include <EntityComponent/Components/GravityComponent.hpp>
#include <EntityComponent/Components/PressureParticleComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
#include <EntityComponent/Components/AudioComponent.hpp>
#include <InputHandlerClient.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>

#include <Doremi/Core/Include/EventHandler/Events/PlayerCreationEvent.hpp>
#include <Doremi/Core/Include/InputHandlerServer.hpp>
#include <Doremi/Core/Include/NetworkEventSender.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>
#include <Doremi/Core/Include/InputHandler.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>
#include <Doremi/Core/Include/NetworkPriorityHandler.hpp>

#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/GunFireToggleEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlaySoundEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/StopSoundEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/AnimationTransitionEvent.hpp>

/// Engine
// AI
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
//#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>

// Timing
#include <Timing/FunctionTimer.hpp>

#include <iostream>


namespace Doremi
{
    namespace Core
    {
        PlayerHandler::PlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

        PlayerHandler::~PlayerHandler() {}

        PlayerHandler* PlayerHandler::m_singleton = nullptr;

        PlayerHandler* PlayerHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("PlayerHandler not initialized, GetInstance called.");
            }

            return m_singleton;
        }

        void PlayerHandler::UpdatePlayerPositions(Player* p_player)
        {
            FUNCTION_TIMER
            int t_checkIfWeHavePlayer = EntityHandler::GetInstance().GetLastEntityIndex();

            XMFLOAT3 t_entityVelocity = XMFLOAT3(0, 0, 0);

            InputHandler* inputHandler = p_player->m_inputHandler;

            EntityID entityID = p_player->m_playerEntityID;

            if (EntityHandler::GetInstance().HasComponents(entityID, (int)ComponentType::CharacterController | (int)ComponentType::Transform))
            {
                // Get transform component
                TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(entityID);
                // Get direction
                XMFLOAT4 orientation = transComp->rotation;
                // Start with standard right
                XMVECTOR right = XMLoadFloat3(&XMFLOAT3(1, 0, 0));

                // Rotate right vector with quaternione giving us the characters right vec
                XMVECTOR orientationVec = XMLoadFloat4(&orientation);
                right = XMVector3Rotate(right, orientationVec);

                // Create forward by crossing right with standard up
                XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
                XMVECTOR forward = XMVector3Cross(right, up);

                // Normalize!
                forward = XMVector3Normalize(forward);

                /// Handle keyboard input
                // Start by creating a movement vector
                XMFLOAT3 movement = XMFLOAT3(0, 0, 0);
                XMVECTOR movementVec = XMLoadFloat3(&movement);
                if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Forward))
                {
                    movementVec += forward * p_player->m_moveSpeed;
                }

                if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Backward))
                {
                    movementVec -= forward * p_player->m_moveSpeed;
                }
                if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Left))
                {
                    movementVec -= right * p_player->m_moveSpeed;
                }
                if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Right))
                {
                    movementVec += right * p_player->m_moveSpeed;
                }

                if(inputHandler->CheckForOnePress((int)UserCommandPlaying::Jump))
                {

                    if(EntityHandler::GetInstance().GetComponentFromStorage<GravityComponent>(entityID)->travelSpeed <= 1)
                    {
                        JumpComponent* t_jumpComponent = EntityHandler::GetInstance().GetComponentFromStorage<JumpComponent>(entityID);
                        if(!t_jumpComponent->active)
                        {
                            t_jumpComponent->StartJump();
                            EntityHandler::GetInstance().GetComponentFromStorage<GravityComponent>(entityID)->travelSpeed = 0;
                            // Send jump event.
                            // AudioComponent* audioComp = EntityHandler::GetInstance().GetComponentFromStorage<AudioComponent>(entityID);
                            PlaySoundEvent* playSoundEvent = new PlaySoundEvent(p_player->m_playerEntityID, (int32_t)AudioCompEnum::Jump);
                            EventHandler::GetInstance()->BroadcastEvent(playSoundEvent);
                            AnimationTransitionEvent* t_animationTransitionEvent = new AnimationTransitionEvent(p_player->m_playerEntityID, Animation::JUMP);
                            EventHandler::GetInstance()->BroadcastEvent(t_animationTransitionEvent);
                        }
                        // EntityHandler::GetInstance().GetComponentFromStorage<JumpComponent>(entityID)->StartJump();
                    }
                }

                // Store finished movement vec
                XMStoreFloat3(&movement, movementVec);
                if (movement.x != 0 || movement.y != 0 || movement.z != 0)
                {
                    MovementComponent* moveComp = EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(entityID);
                    moveComp->movement.x += movement.x;
                    moveComp->movement.y += movement.y;
                    moveComp->movement.z += movement.z;
                }
            }
        }

        void PlayerHandler::UpdateFiring(Player* p_player)
        {
            FUNCTION_TIMER

            // THOS DOES NOTHING AT THIS MOMENT
            // TODOJB fix gun on server
            InputHandler* inputHandler = p_player->m_inputHandler;

            EntityID entityID = p_player->m_playerEntityID;
            // Check if player fires the gun. TODOJB strange to have it in this method? Refactor into overall UpdatePlayerServer method?
            if(EntityHandler::GetInstance().HasComponents(entityID, (int)ComponentType::PressureParticleSystem))
            {
                // Update position of emitter
                m_gunController.Update(p_player, m_sharedContext);

                if(inputHandler->CheckForOnePress((int)UserCommandPlaying::LeftClick))
                {
                    m_gunController.StartFireGun(entityID, m_sharedContext);
                    GunFireToggleEvent* gunFireEvent = new GunFireToggleEvent(p_player->m_playerEntityID, true);
                    EventHandler::GetInstance()->BroadcastEvent(gunFireEvent);
                    AnimationTransitionEvent* t_animationTransitionEvent = new AnimationTransitionEvent(p_player->m_playerEntityID, Animation::ATTACK);
                    EventHandler::GetInstance()->BroadcastEvent(t_animationTransitionEvent);
                }
                else if(inputHandler->CheckForRelease((int)UserCommandPlaying::LeftClick))
                {
                    m_gunController.StopFireGun(entityID, m_sharedContext);
                    GunFireToggleEvent* gunFireEvent = new GunFireToggleEvent(p_player->m_playerEntityID, false);
                    EventHandler::GetInstance()->BroadcastEvent(gunFireEvent);
                    AnimationTransitionEvent* t_animationTransitionEvent = new AnimationTransitionEvent(p_player->m_playerEntityID, Animation::STOPATTACK);
                    EventHandler::GetInstance()->BroadcastEvent(t_animationTransitionEvent);
                }
            }
        }
    }
}