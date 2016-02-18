#include <Doremi/Core/Include/PlayerHandlerServer.hpp>

#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>

#include <Doremi/Core/Include/NetworkEventSender.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>
#include <Doremi/Core/Include/NetworkPriorityHandler.hpp>
#include <Doremi/Core/Include/InputHandlerServer.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/PlayerSpawnerHandler.hpp>

#include <DoremiEngine/Input/Include/InputModule.hpp>
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>

// Events
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/EventHandler/Events/RemoveEntityEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/EntityCreatedEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlayerCreationEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlayerRespawnEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/GunFireToggleEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/TriggerEvent.hpp>

// Timing
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>

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

#include <iostream>

namespace Doremi
{
    namespace Core
    {
        PlayerHandlerServer::PlayerHandlerServer(const DoremiEngine::Core::SharedContext& p_sharedContext) : PlayerHandler(p_sharedContext)
        {
            EventHandler* t_EventHandler = EventHandler::GetInstance();
            t_EventHandler->Subscribe(EventType::RemoveEntity, this);
            t_EventHandler->Subscribe(EventType::EntityCreated, this);
            t_EventHandler->Subscribe(EventType::PlayerRespawn, this);
            t_EventHandler->Subscribe(EventType::GunFireToggle, this);
        }

        PlayerHandlerServer::~PlayerHandlerServer() {}

        void PlayerHandlerServer::StartPlayerHandlerServer(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("PlayerHandler StartPlayerHandler called multiple times.");
            }
            m_singleton = new PlayerHandlerServer(p_sharedContext);
        }

        void PlayerHandlerServer::Update(double p_dt)
        {
            TIME_FUNCTION_START
            UpdatePlayerInputs();
            UpdatePlayerPositions();
            UpdatePlayerRotationsServer();
            UpdateFiring();
            UpdateNetworkObjectPriority(p_dt);

            // Hotfix
            std::map<uint32_t, Player*>::iterator iter;

            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                (static_cast<PlayerServer*>(iter->second))->m_frequencyBufferHandler->Update();
            }


            TIME_FUNCTION_STOP
        }

        void PlayerHandlerServer::UpdatePlayerInputs()
        {
            TIME_FUNCTION_START
            std::map<uint32_t, Player*>::iterator iter;

            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                ((InputHandlerServer*)iter->second->m_inputHandler)->Update(iter->second->m_playerEntityID);
            }
            TIME_FUNCTION_STOP
        }

        void PlayerHandlerServer::CreateNewPlayer(uint32_t p_playerID, InputHandler* p_inputHandler)
        {
            // Check if we already have a ID
            // TODOCM maybe we ignore here and if we find one, we know there actually was one before, and can use that position etc..
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            if(iter != m_playerMap.end())
            {
                std::runtime_error("Creating player twice with same ID.");
            }

            // Get the spawner ID and spawn the new player there
            // TODO could change this to checkpoint when we get checkpoints in the game
            uint32_t t_spawnerEntityID = PlayerSpawnerHandler::GetInstance()->GetCurrentSpawnerEntityID();
            DoremiEngine::Physics::RigidBodyManager& t_rigidBodyManager = m_sharedContext.GetPhysicsModule().GetRigidBodyManager();

            // Get position and orientation of the trigger..
            DirectX::XMFLOAT3 t_triggerPosition = t_rigidBodyManager.GetBodyPosition(t_spawnerEntityID);
            DirectX::XMFLOAT4 t_triggerOrientation = t_rigidBodyManager.GetBodyOrientation(t_spawnerEntityID);

            // Create new stuff needed for a Player
            NetworkEventSender* newNetworkEventSender = new NetworkEventSender();
            FrequencyBufferHandler* newFrequencyHandler = new FrequencyBufferHandler();
            NetworkPriorityHandler* newNetworkPriorityHandler = new NetworkPriorityHandler(m_sharedContext);

            // Create the actual playerentity
            EntityID t_EntityID =
                EntityHandler::GetInstance().CreateEntity(Blueprints::PlayerEntity, t_triggerPosition, t_triggerOrientation, XMFLOAT3(0.25, 0.25, 0.25));

            // Create a new player
            PlayerServer* NewPlayer = new PlayerServer(t_EntityID, p_inputHandler, newFrequencyHandler, newNetworkPriorityHandler, newNetworkEventSender);

            // Add player to map
            m_playerMap[p_playerID] = NewPlayer;

            // Set start and endEvent for client
            SetupRejoin(p_playerID);

            /// Add a new potential field actor to the player
            // Check if we have a actor, different from server and client...
            if(EntityHandler::GetInstance().HasComponents(NewPlayer->m_playerEntityID, (int)ComponentType::PotentialField))
            {
                PotentialFieldComponent* pfComponent = EntityHandler::GetInstance().GetComponentFromStorage<PotentialFieldComponent>(NewPlayer->m_playerEntityID);
                pfComponent->ChargedActor = m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(t_triggerPosition, 30, 60, false,
                                                                                                                      DoremiEngine::AI::AIActorType::Player);
            }

            // Create event & broadcast event
            PlayerCreationEvent* playerCreateEvent = new PlayerCreationEvent(NewPlayer->m_playerEntityID);
            EventHandler::GetInstance()->BroadcastEvent(playerCreateEvent);
        }

        NetworkEventSender* PlayerHandlerServer::GetNetworkEventSenderForPlayer(uint32_t p_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            NetworkEventSender* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = (static_cast<PlayerServer*>(iter->second))->m_networkEventSender;
            }

            return outPointer;
        }

        NetworkPriorityHandler* PlayerHandlerServer::GetNetworkPriorityHandlerForplayer(uint32_t p_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            NetworkPriorityHandler* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = (static_cast<PlayerServer*>(iter->second))->m_networkPriorityHandler;
            }

            return outPointer;
        }

        void PlayerHandlerServer::UpdateNetworkObjectPriority(double p_dt)
        {
            // For each player we update their networkobj priorities
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                (static_cast<PlayerServer*>(iter->second))->m_networkPriorityHandler->Update(iter->second->m_playerEntityID, p_dt);
            }
        }

        void PlayerHandlerServer::AddNetObjectToPlayers(const EntityID& p_entityID)
        {
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                (static_cast<PlayerServer*>(iter->second))->m_networkPriorityHandler->UpdateNetworkObject(p_entityID);
            }
        }

        void PlayerHandlerServer::SetupRejoin(uint32_t t_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(t_playerID);
            if(iter == m_playerMap.end())
            {
                // Something really wrong
                cout << "Error when setup rejoin, playerID missing." << endl;
                std::runtime_error("Error when setup rejoin, playerID missing.");
            }
            PlayerServer* t_player = static_cast<PlayerServer*>(iter->second);

            t_player->m_StartEvent = 0; // Well... if we rejoin we want to reset
            t_player->m_EndEvent = m_lateJoinEventQueue.size(); // Size of current queue
        }

        bool PlayerHandlerServer::UpdateRejoinQueueForPlayer(uint32_t p_EventSlot, uint32_t t_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(t_playerID);
            if(iter == m_playerMap.end())
            {
                // Something really wrong
                cout << "Error when setup rejoin, playerID missing." << endl;
                std::runtime_error("Error when setup rejoin, playerID missing.");
            }
            PlayerServer* t_player = static_cast<PlayerServer*>(iter->second);

            // If we acc something larger or same (case of 0), then our start
            if(p_EventSlot >= t_player->m_StartEvent)
            {
                // Set new start
                t_player->m_StartEvent = p_EventSlot;

                // If we reached the end, all events sent
                if(t_player->m_StartEvent == t_player->m_EndEvent)
                {
                    return true;
                }
                else if(t_player->m_StartEvent > t_player->m_EndEvent)
                {
                    cout << "Error acc on updateRejoinQueue... consider disconnecting client" << endl;
                }
            }
            return false;
        }

        void PlayerHandlerServer::WriteQueuedEventsFromLateJoin(NetworkStreamer& p_streamer, const uint32_t& p_bufferSize, uint32_t& op_bytesWritten, uint32_t t_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(t_playerID);
            if(iter == m_playerMap.end())
            {
                // Something really wrong
                cout << "Error when sending events on rejoin, playerID missing." << endl;
                std::runtime_error("Error when sending events on rejoin, playerID missing.");
            }
            PlayerServer* t_player = static_cast<PlayerServer*>(iter->second);

            // Write start event
            uint32_t t_eventStart = t_player->m_StartEvent;
            size_t t_eventEnd = t_player->m_EndEvent;
            p_streamer.WriteUnsignedInt32(t_eventStart);
            op_bytesWritten += sizeof(uint32_t);

            // Save position to wirte number of events
            uint32_t t_numOfEvents = 0;
            uint32_t t_posToWriteNumEvents = op_bytesWritten;
            op_bytesWritten += sizeof(uint32_t);
            p_streamer.SetReadWritePosition(op_bytesWritten);

            uint32_t t_bitsWritten = 0;
            uint32_t t_bitIncrement = 0;
            uint32_t t_bitsLeftToWrite = (p_bufferSize - op_bytesWritten) * 8;


            // While we got events left to send and we have space left to wirte
            for(size_t i = t_eventStart; i < t_eventEnd && t_bitsWritten < t_bitsLeftToWrite; i++)
            {
                p_streamer.WriteUnsignedInt32(static_cast<uint32_t>(m_lateJoinEventQueue[i]->eventType));
                t_bitIncrement += sizeof(uint32_t) * 8;

                m_lateJoinEventQueue[i]->Write(&p_streamer, t_bitIncrement);
                t_bitsWritten += t_bitIncrement;
                t_numOfEvents++;
            }

            // if we overdid it, take a step back
            if(t_bitsWritten > t_bitsLeftToWrite)
            {
                t_bitsWritten -= t_bitIncrement;
                t_numOfEvents--;
            }
            op_bytesWritten += ceil(t_bitsWritten / 8.0f);

            // Write num of events sent
            p_streamer.SetReadWritePosition(t_posToWriteNumEvents);
            p_streamer.WriteUnsignedInt32(t_numOfEvents);

            // Set position back
            p_streamer.SetReadWritePosition(op_bytesWritten);
        }

        void PlayerHandlerServer::QueueEntityCreatedEventToPlayers(EntityCreatedEvent* p_entityCreatedEvent)
        {
            // If object have network component, we add it to the all players
            if(EntityHandler::GetInstance().HasComponents(p_entityCreatedEvent->entityID, static_cast<int>(ComponentType::NetworkObject)))
            {
                AddNetObjectToPlayers(p_entityCreatedEvent->entityID);
            }

            // Go through all players
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                Blueprints blueprint = p_entityCreatedEvent->bluepirnt;

                // If we create a player entity, we want to create a network player on other clients
                if(blueprint == Blueprints::PlayerEntity && !p_entityCreatedEvent->entityID == iter->second->m_playerEntityID)
                {
                    blueprint = Blueprints::NetworkPlayerEntity;
                }

                (static_cast<PlayerServer*>(iter->second))
                    ->m_networkEventSender->QueueEventToFrame(new EntityCreatedEvent(p_entityCreatedEvent->entityID, blueprint,
                                                                                     p_entityCreatedEvent->position, p_entityCreatedEvent->orientation));
            }

            // For less if checks later on we change all playerEntities created to networkPlayers
            // The real player will be sent in the "normal" event queue to the clients, and therefor skip this one
            // TODOXX If there is any other OnEvent that explicity checks for PlayerEntity to be created.. this might fuck up
            if(p_entityCreatedEvent->bluepirnt == Blueprints::PlayerEntity)
            {
                p_entityCreatedEvent->bluepirnt = Blueprints::NetworkPlayerEntity;
            }

            // Save it for later joins
            m_lateJoinEventQueue.push_back(new EntityCreatedEvent(*p_entityCreatedEvent));
        }

        void PlayerHandlerServer::QueueRemoveEntityEventToPlayers(RemoveEntityEvent* p_removeEvent)
        {
            // Go through all players
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                (static_cast<PlayerServer*>(iter->second))->m_networkEventSender->QueueEventToFrame(new RemoveEntityEvent(*p_removeEvent));
            }

            // Save it for later joins
            m_lateJoinEventQueue.push_back(new RemoveEntityEvent(*p_removeEvent));
        }

        void PlayerHandlerServer::QueuePlayerRespawnEventToPlayers(PlayerRespawnEvent* p_playerRespawnEvent)
        {
            // Go through all players
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                (static_cast<PlayerServer*>(iter->second))->m_networkEventSender->QueueEventToFrame(new PlayerRespawnEvent(*p_playerRespawnEvent));
            }
        }

        void PlayerHandlerServer::QueueGunFireToggleEventToPlayers(GunFireToggleEvent* t_gunFireToggleEvent)
        {
            // Go through all players
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                if(iter->second->m_playerEntityID == t_gunFireToggleEvent->entityID)
                {
                    continue;
                }
                (static_cast<PlayerServer*>(iter->second))->m_networkEventSender->QueueEventToFrame(new GunFireToggleEvent(*t_gunFireToggleEvent));
            }
        }

        void PlayerHandlerServer::OnEvent(Event* p_event)
        {
            switch(p_event->eventType)
            {
                case Doremi::Core::EventType::EntityCreated:
                {
                    EntityCreatedEvent* t_entityCreatedEvent = static_cast<EntityCreatedEvent*>(p_event);

                    // Send the event to all players
                    QueueEntityCreatedEventToPlayers(t_entityCreatedEvent);

                    break;
                }
                case Doremi::Core::EventType::RemoveEntity:
                {
                    RemoveEntityEvent* t_removeEvent = static_cast<RemoveEntityEvent*>(p_event);

                    // Queue the event to all players
                    QueueRemoveEntityEventToPlayers(t_removeEvent);

                    break;
                }
                case Doremi::Core::EventType::PlayerRespawn:
                {
                    PlayerRespawnEvent* t_playerRespawnEvent = static_cast<PlayerRespawnEvent*>(p_event);

                    // Queue the event to all players
                    QueuePlayerRespawnEventToPlayers(t_playerRespawnEvent);

                    break;
                }
                case Doremi::Core::EventType::GunFireToggle:
                {
                    GunFireToggleEvent* t_gunFireToggleEvent = static_cast<GunFireToggleEvent*>(p_event);

                    QueueGunFireToggleEventToPlayers(t_gunFireToggleEvent);

                    break;
                }
                default:
                    break;
            }
        }
    }
}