#include <Doremi/Core/Include/PlayerHandlerClient.hpp>

#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>

#include <Doremi/Core/Include/NetworkEventReceiver.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>

#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlayerCreationEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/GunFireToggleEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlayerRespawnEvent.hpp>

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
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>
#include <iostream>

// Logging
#include <DoremiEngine/Logging/Include/LoggingModule.hpp>
#include <DoremiEngine/Logging/Include/SubmoduleManager.hpp>
#include <DoremiEngine/Logging/Include/Logger/Logger.hpp>

namespace Doremi
{
    namespace Core
    {
        PlayerHandlerClient::PlayerHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : PlayerHandler(p_sharedContext), m_logger(nullptr)
        {
            EventHandler::GetInstance()->Subscribe(EventType::GunFireToggle, this);
            EventHandler::GetInstance()->Subscribe(EventType::PlayerRespawn, this);
            m_logger = &m_sharedContext.GetLoggingModule().GetSubModuleManager().GetLogger();
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

        void PlayerHandlerClient::Update(double p_dt)
        {
            TIME_FUNCTION_START
            UpdatePlayerPositions();
            UpdatePlayerRotationsClient();
            UpdateFiring();
            TIME_FUNCTION_STOP
        }

        void PlayerHandlerClient::UpdatePlayerInputs()
        {
            TIME_FUNCTION_START
            m_sharedContext.GetInputModule().Update();

            for(auto& input : m_playerMap)
            {
                using namespace Doremi::Utilities::Logging;
                InputHandlerClient* inputHandler = static_cast<InputHandlerClient*>(input.second->m_inputHandler);
                inputHandler->Update();
                m_logger->LogText(LogTag::INPUT, LogLevel::INFO, "X, %d\nY, %d\nM, %d", inputHandler->GetMouseMovementX(),
                                  inputHandler->GetMouseMovementY(), inputHandler->GetInputBitMask());
            }
            TIME_FUNCTION_STOP
        }

        void PlayerHandlerClient::CreateNewPlayer(uint32_t p_playerID, InputHandler* p_inputHandler)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            if(iter != m_playerMap.end())
            {
                std::runtime_error("Creating player twice with same ID.");
            }

            // TODOCM hard coded entityID for new players
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(5.0f, 30.0f, 0.0f);
            EntityID t_EntityID =
                EntityHandler::GetInstance().CreateEntity(Blueprints::PlayerEntity, position, XMFLOAT4(0, 0, 0, 1), XMFLOAT3(0.25, 0.25, 0.25));

            NetworkEventReceiver* newNetworkEventReceiver = new NetworkEventReceiver();
            FrequencyBufferHandler* newFrequencyHandler = new FrequencyBufferHandler();

            Player* NewPlayer = new PlayerClient(t_EntityID, p_inputHandler, newFrequencyHandler, newNetworkEventReceiver);

            m_playerMap[p_playerID] = NewPlayer;

            // int materialID =
            // EntityHandler::GetInstance().GetComponentFromStorage<Core::PhysicsMaterialComponent>(NewPlayer->m_playerEntityID)->p_materialID;
            // DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(5.0f, 30.0f, 0.0f);
            // DirectX::XMFLOAT4 orientation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
            // m_sharedContext.GetPhysicsModule().GetCharacterControlManager().AddController(NewPlayer->m_playerEntityID, materialID, position,
            // XMFLOAT2(1.0f, 1.0f));

            /// Add a new potential field actor to the player
            // Check if we have a actor, different from server and client...
            if(EntityHandler::GetInstance().HasComponents(NewPlayer->m_playerEntityID, (int)ComponentType::PotentialField))
            {
                PotentialFieldComponent* pfComponent = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(NewPlayer->m_playerEntityID);
                pfComponent->ChargedActor =
                    m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(position, 30, 60, false, DoremiEngine::AI::AIActorType::Player);
            }
            /// Create the gun
            // Check if we have the gun
            if(EntityHandler::GetInstance().HasComponents(NewPlayer->m_playerEntityID, (int)ComponentType::PressureParticleSystem))
            {
                // ParticlePressureComponent* particleComp =
                //    EntityHandler::GetInstance().GetComponentFromStorage<ParticlePressureComponent>(NewPlayer->m_playerEntityID);
                // particleComp->data.m_active = false;
                // particleComp->data.m_density = 2.0f;
                // particleComp->data.m_dimensions = XMFLOAT2(0.0f, 0.0f);
                // particleComp->data.m_direction =
                // EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(NewPlayer->m_playerEntityID)->rotation;
                // particleComp->data.m_emissionAreaDimensions = XMFLOAT2(3.14 / 4, 3.14 / 5);
                // particleComp->data.m_emissionRate = 0.05;
                // particleComp->data.m_launchPressure = 100;
                // particleComp->data.m_numParticlesX = 5;
                // particleComp->data.m_numParticlesY = 1;
                // particleComp->data.m_size = 1;
                // particleComp->data.m_position =
                // EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(NewPlayer->m_playerEntityID)->position;
                // m_sharedContext.GetPhysicsModule().GetFluidManager().CreateParticleEmitter(NewPlayer->m_playerEntityID, particleComp->data);
            }

            // Create event
            PlayerCreationEvent* playerCreateEvent = new PlayerCreationEvent(NewPlayer->m_playerEntityID);

            // Broadcast event
            EventHandler::GetInstance()->BroadcastEvent(playerCreateEvent);
        }

        NetworkEventReceiver* PlayerHandlerClient::GetNetworkEventReceiverForPlayer(uint32_t p_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            NetworkEventReceiver* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = (static_cast<PlayerClient*>(iter->second))->m_networkEventReceiver;
            }

            return outPointer;
        }

        void PlayerHandlerClient::ReadEventsForJoin(NetworkStreamer& p_streamer, const uint32_t& p_bufferSize, uint32_t& op_bytesRead)
        {
            // Get eventhandler for later use
            EventHandler* t_eventHandler = EventHandler::GetInstance();

            // Read the first event to be read
            uint32_t t_messageStartEvent = p_streamer.ReadUnsignedInt32();
            op_bytesRead += sizeof(uint32_t);

            // Read the number of events to be read
            uint32_t t_numOfEvents = p_streamer.ReadUnsignedInt32();
            op_bytesRead = +sizeof(uint32_t);

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

        void PlayerHandlerClient::OnEvent(Event* p_event)
        {
            if(p_event->eventType == EventType::GunFireToggle)
            {
                GunFireToggleEvent* t_gunFireToggleEvent = static_cast<GunFireToggleEvent*>(p_event);

                // If it's not ourselves we fire
                EntityID t_entityID = 0;
                GetDefaultPlayerEntityID(t_entityID);
                if(t_gunFireToggleEvent->entityID != t_entityID)
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
        }
    }
}
