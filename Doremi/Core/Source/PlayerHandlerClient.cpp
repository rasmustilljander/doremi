#include <Doremi/Core/Include/PlayerHandlerClient.hpp>

#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>

#include <Doremi/Core/Include/NetworkEventReceiver.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>

#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlayerCreationEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/GunFireToggleEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlayerRespawnEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/ChangeMenuState.hpp>

#include <DoremiEngine/Input/Include/InputModule.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlaySoundEvent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/AudioComponent.hpp>

// AI
#include <DoremiEngine/AI/Include/AIModule.hpp>
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>

// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>

// Components
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
#include <EntityComponent/Components/PressureParticleComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/HealthComponent.hpp>

#include <Doremi/Core/Include/InputHandlerClient.hpp>

#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>
#include <Doremi/Core/Include/EventInterpeter.hpp>

// Timing
#include <Timing/FunctionTimer.hpp>

#include <iostream>

// Logging
#include <DoremiEngine/Logging/Include/LoggingModule.hpp>
#include <DoremiEngine/Logging/Include/SubmoduleManager.hpp>
#include <DoremiEngine/Logging/Include/Logger/Logger.hpp>

// Configuration
#include <DoremiEngine/Configuration/Include/ConfigurationModule.hpp>

namespace Doremi
{
    namespace Core
    {
        PlayerHandlerClient::PlayerHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : PlayerHandler(p_sharedContext), m_logger(nullptr), m_lastJoinEventRead(0), m_maxNumEvents(0), m_jaw(0), m_pitch(0)
        {
            EventHandler::GetInstance()->Subscribe(EventType::GunFireToggle, this);
            EventHandler::GetInstance()->Subscribe(EventType::PlayerRespawn, this);
            EventHandler::GetInstance()->Subscribe(EventType::EntityCreated, this);
            EventHandler::GetInstance()->Subscribe(EventType::SetHealth, this);
            EventHandler::GetInstance()->Subscribe(EventType::SetTransform, this);
            EventHandler::GetInstance()->Subscribe(EventType::DamageTaken, this);

            m_logger = &m_sharedContext.GetLoggingModule().GetSubModuleManager().GetLogger();

            InputHandlerClient* m_inputHandler = new InputHandlerClient(p_sharedContext);
            NetworkEventReceiver* newNetworkEventReceiver = new NetworkEventReceiver();
            FrequencyBufferHandler* newFrequencyHandler = new FrequencyBufferHandler();

            // Create player object thing
            m_player = new PlayerClient(0, m_inputHandler, newFrequencyHandler, newNetworkEventReceiver);
            m_player->m_turnSpeed = m_sharedContext.GetConfigurationModule().GetAllConfigurationValues().TurnSpeed;
            m_maxPitch = m_sharedContext.GetConfigurationModule().GetAllConfigurationValues().MaxPitch;
            m_minPitch = m_sharedContext.GetConfigurationModule().GetAllConfigurationValues().MinPitch;
        }

        PlayerHandlerClient::~PlayerHandlerClient()
        {
            // Do not delete m_logger, internally handled by loggingmodule
        }

        void PlayerHandlerClient::StartPlayerHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("PlayerHandler StartPlayerHandler called multiple times.");
            }
            m_singleton = new PlayerHandlerClient(p_sharedContext);
        }


        EntityID PlayerHandlerClient::GetPlayerEntityID() { return m_player->m_playerEntityID; }


        InputHandlerClient* PlayerHandlerClient::GetInputHandler() { return static_cast<InputHandlerClient*>(m_player->m_inputHandler); }

        FrequencyBufferHandler* PlayerHandlerClient::GetFrequencyBufferHandler() { return m_player->m_frequencyBufferHandler; }


        void PlayerHandlerClient::Update(double p_dt)
        {
            FUNCTION_TIMER

            // If player is created we can update it
            if(m_player->IsCreated)
            {
                UpdatePlayerPositions(m_player);
                UpdatePlayerRotations(m_player);
                UpdateFiring(m_player);
                UpdatePlayerConfigurationValues(m_player);
            }

            // Check to exit TODO move place of code
            if(m_player->m_inputHandler->CheckForOnePress(static_cast<uint32_t>(UserCommandPlaying::ExitGame)))
            {
                ChangeMenuState* t_newEvent = new ChangeMenuState();
                t_newEvent->state = DoremiGameStates::MAINMENU;


                EventHandler::GetInstance()->BroadcastEvent(t_newEvent);
            }
        }

        void PlayerHandlerClient::UpdatePlayerRotations(Player* p_player)
        {
            FUNCTION_TIMER

            InputHandlerClient* inputHandler = (InputHandlerClient*)p_player->m_inputHandler;

            EntityID entityID = p_player->m_playerEntityID;

            if(EntityHandler::GetInstance().HasComponents(entityID, (int)ComponentType::CharacterController | (int)ComponentType::Transform))
            {
                /// Handle mouse input
                m_jaw += inputHandler->GetMouseMovementX() * p_player->m_turnSpeed;
                m_pitch += inputHandler->GetMouseMovementY() * p_player->m_turnSpeed;

                // Check if we are looking outside of bounds
                if(m_pitch > m_maxPitch)
                {
                    m_pitch = m_maxPitch;
                }
                else if(m_pitch < m_minPitch)
                {
                    m_pitch = m_minPitch;
                }

                // Create and save rotation
                XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(m_pitch, m_jaw, 0);
                XMFLOAT4 newOrientation;
                XMStoreFloat4(&newOrientation, XMQuaternionRotationMatrix(rotationMat));
                TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(entityID);
                transComp->rotation = newOrientation;
            }
        }

        void PlayerHandlerClient::UpdatePlayerInputs()
        {
            FUNCTION_TIMER
            m_sharedContext.GetInputModule().Update();

            // Update the inputhandler
            InputHandlerClient* inputHandler = static_cast<InputHandlerClient*>(m_player->m_inputHandler);
            inputHandler->Update();

            // If player is created, I think we want to logg input
            if(m_player->IsCreated)
            {
                using namespace Doremi::Utilities::Logging;
                m_logger->LogText(LogTag::INPUT, LogLevel::MASS_DATA_PRINT, "X, %d\nY, %d\nM, %d", inputHandler->GetMouseMovementX(),
                                  inputHandler->GetMouseMovementY(), inputHandler->GetInputBitMask());
            }
        }

        void PlayerHandlerClient::SetNewPlayerEntityID(const EntityID& p_entityID)
        {
            if(m_player->IsCreated)
            {
                std::runtime_error("Player created multiple times");
            }

            // Set id and set it created
            m_player->IsCreated = true;
            m_player->m_playerEntityID = p_entityID;

            // Create event
            PlayerCreationEvent* playerCreateEvent = new PlayerCreationEvent(p_entityID);

            // Broadcast event
            // TODOXX If we create this event on server and send this might be removed
            EventHandler::GetInstance()->BroadcastEvent(playerCreateEvent);
        }

        NetworkEventReceiver* PlayerHandlerClient::GetNetworkEventReceiver() { return m_player->m_networkEventReceiver; }

        void PlayerHandlerClient::ReadEventsForJoin(NetworkStreamer& p_streamer, const uint32_t& p_bufferSize, uint32_t& op_bytesRead)
        {
            // Get eventhandler for later use
            EventHandler* t_eventHandler = EventHandler::GetInstance();

            // Read the first event to be read
            uint32_t t_messageStartEvent = p_streamer.ReadUnsignedInt32();
            op_bytesRead += sizeof(uint32_t);

            // Read the number of events to be read
            uint32_t t_numOfEvents = p_streamer.ReadUnsignedInt32();
            op_bytesRead += sizeof(uint32_t);

            // Here is a thing, because we know all event exists beforehand, and because they are bunched to be sent according to acks
            // We can make the assumption that if we've already read the first one here, we've read all in the message
            uint32_t t_bitsRead = 0;
            if(t_messageStartEvent < m_lastJoinEventRead)
            {
                for(size_t i = 0; i < t_numOfEvents; i++)
                {
                    // Read but ignore all events
                    Event* t_newEvent = InterpetEvent(p_streamer, t_bitsRead);


                    // TODO better way of creating, destroying events?(not just this place)
                    // delete them right away
                    delete t_newEvent;
                }
            }
            else
            {
                for(size_t i = 0; i < t_numOfEvents; i++)
                {
                    // Read but ignore all events
                    Event* t_newEvent = InterpetEvent(p_streamer, t_bitsRead);

                    t_eventHandler->BroadcastEvent(t_newEvent);
                }
                m_lastJoinEventRead += t_numOfEvents;
            }

            // Exclude duplicates
            op_bytesRead += ceil(t_bitsRead / 8.0f);

            // Set position back
            p_streamer.SetReadWritePosition(op_bytesRead);
        }

        bool PlayerHandlerClient::PlayerExists() { return m_player->IsCreated; }

        void PlayerHandlerClient::RemovePlayer()
        {
            m_player->IsCreated = false;
            m_player->m_networkEventReceiver->Reset();
            m_player->m_frequencyBufferHandler->Reset();
            m_lastJoinEventRead = 0;
            m_maxNumEvents = 0;
        }

        void PlayerHandlerClient::SetMaximumNumberOfJoinEvents(uint32_t p_maxNumOfJoinEvents) { m_maxNumEvents = p_maxNumOfJoinEvents; }

        void PlayerHandlerClient::OnEvent(Event* p_event)
        {
            if(p_event->eventType == EventType::GunFireToggle)
            {
                GunFireToggleEvent* t_gunFireToggleEvent = static_cast<GunFireToggleEvent*>(p_event);

                // If it's not ourselves we fire
                if(m_player->IsCreated && t_gunFireToggleEvent->entityID != m_player->m_playerEntityID)
                {
                    if(t_gunFireToggleEvent->isFiring)
                    {
                        m_gunController.StartFireGun(t_gunFireToggleEvent->entityID, m_sharedContext);
                    }
                    else
                    {
                        m_gunController.StopFireGun(t_gunFireToggleEvent->entityID, m_sharedContext);
                    }
                }
            }
            else if(p_event->eventType == EventType::PlayerRespawn)
            {
                PlayerRespawnEvent* t_playerSpawnerEvent = static_cast<PlayerRespawnEvent*>(p_event);

                // Reset health for show
                HealthComponent* t_healthComp = GetComponent<HealthComponent>(t_playerSpawnerEvent->entityID);
                t_healthComp->currentHealth = t_healthComp->maxHealth;

                // Play respawn sound
                EventHandler::GetInstance()->BroadcastEvent(new PlaySoundEvent(t_playerSpawnerEvent->entityID, static_cast<int32_t>(AudioCompEnum::Death)));
            }
            else if(p_event->eventType == EventType::DamageTaken)
            {
                DamageTakenEvent* t_dmgTakenEvent = static_cast<DamageTakenEvent*>(p_event);

                HealthComponent* t_healthComp = GetComponent<HealthComponent>(t_dmgTakenEvent->entityId);
                t_healthComp->currentHealth -= t_dmgTakenEvent->damage;
            }
            else if(p_event->eventType == Doremi::Core::EventType::SetHealth)
            {
                SetHealthEvent* t_setHealthEvent = static_cast<SetHealthEvent*>(p_event);

                GetComponent<HealthComponent>(t_setHealthEvent->entityID)->currentHealth = t_setHealthEvent->health;
            }
            else if(p_event->eventType == Doremi::Core::EventType::SetTransform)
            {
                cout << "Received set transform" << endl;
                SetTransformEvent* t_setTransformEvent = static_cast<SetTransformEvent*>(p_event);

                EntityHandler& t_entityHandler = EntityHandler::GetInstance();

                // If char controller or rigid body, set position to physics
                if(t_entityHandler.HasComponents(t_setTransformEvent->entityID, static_cast<uint32_t>(ComponentType::RigidBody)))
                {
                    DoremiEngine::Physics::RigidBodyManager& t_rigidBodyManager = m_sharedContext.GetPhysicsModule().GetRigidBodyManager();

                    t_rigidBodyManager.SetBodyPosition(t_setTransformEvent->entityID, t_setTransformEvent->position, t_setTransformEvent->orientation);
                }
                else if(t_entityHandler.HasComponents(t_setTransformEvent->entityID, static_cast<uint32_t>(ComponentType::CharacterController)))
                {
                    cout << "Setting char controller transform" << endl;
                    DoremiEngine::Physics::CharacterControlManager& t_characterControlManager = m_sharedContext.GetPhysicsModule().GetCharacterControlManager();

                    t_characterControlManager.SetPosition(t_setTransformEvent->entityID, t_setTransformEvent->position);
                }

                // If player
                if(PlayerExists() && GetPlayerEntityID() == t_setTransformEvent->entityID)
                {
                    XMFLOAT3 t_standard = XMFLOAT3(0, 0, 1);
                    XMVECTOR t_standardVec = XMLoadFloat3(&t_standard);
                    XMVECTOR t_newOrienVec = XMLoadFloat4(&t_setTransformEvent->orientation);
                    XMVECTOR t_newVector = XMVector3Rotate(t_standardVec, t_newOrienVec);

                    XMVECTOR t_anglesVec = XMVector3AngleBetweenVectors(t_standardVec, t_newVector);
                    XMFLOAT3 t_angles;
                    XMStoreFloat3(&t_angles, t_anglesVec);
                    m_jaw = t_angles.x;
                    m_pitch = 0;
                }

                // Set transform to components
                TransformComponent* transComp = GetComponent<TransformComponent>(t_setTransformEvent->entityID);
                transComp->position = t_setTransformEvent->position;
                transComp->rotation = t_setTransformEvent->orientation;

                // Set copy data to other transform components
                memcpy(GetComponent<TransformComponentNext>(t_setTransformEvent->entityID), transComp, sizeof(TransformComponent));
                memcpy(GetComponent<TransformComponentPrevious>(t_setTransformEvent->entityID), transComp, sizeof(TransformComponent));
                *GetComponent<TransformComponentSnapshotNext>(t_setTransformEvent->entityID) =
                    TransformComponentSnapshotNext(*GetComponent<TransformComponentNext>(t_setTransformEvent->entityID));
                *GetComponent<TransformComponentSnapshotPrevious>(t_setTransformEvent->entityID) =
                    TransformComponentSnapshotPrevious(*GetComponent<TransformComponentNext>(t_setTransformEvent->entityID));
            }
        }

        void PlayerHandlerClient::UpdatePlayerConfigurationValues(Player* p_player)
        {
            InputHandler* inputHandler = p_player->m_inputHandler;
            if(inputHandler->CheckForOnePress((int)UserCommandPlaying::ReduceMouseSensitivity))
            {
                if(p_player->m_turnSpeed > 0.001f)
                {
                    p_player->m_turnSpeed -= 0.001f;
                }
            }
            if(inputHandler->CheckForOnePress((int)UserCommandPlaying::IncreaseMouseSensitivity))
            {
                p_player->m_turnSpeed += 0.001f;
            }
        }
    }
}
