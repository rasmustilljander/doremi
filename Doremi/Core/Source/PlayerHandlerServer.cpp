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
#include <Doremi/Core/Include/EventHandler/Events/SetHealthEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/SetTransformEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/AnimationTransitionEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/DamageTakenEvent.hpp>

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
#include <EntityComponent/Components/HealthComponent.hpp>

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
            t_EventHandler->Subscribe(EventType::AnimationTransition, this);
            t_EventHandler->Subscribe(EventType::DamageTaken, this);
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
        std::map<uint32_t, PlayerServer*>& PlayerHandlerServer::GetPlayerMap() { return m_playerMap; }

        bool PlayerHandlerServer::GetEntityIDForPlayer(uint32_t p_playerID, EntityID& outID)
        {
            std::map<uint32_t, PlayerServer*>::iterator iter = m_playerMap.find(p_playerID);

            if(iter != m_playerMap.end())
            {
                outID = iter->second->m_playerEntityID;
                return true;
            }
            else
            {
                return false;
            }
        }

        InputHandler* PlayerHandlerServer::GetInputHandlerForPlayer(uint32_t p_playerID)
        {
            std::map<uint32_t, PlayerServer*>::iterator iter = m_playerMap.find(p_playerID);

            InputHandler* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = iter->second->m_inputHandler;
            }

            return outPointer;
        }

        FrequencyBufferHandler* PlayerHandlerServer::GetFrequencyBufferHandlerForPlayer(uint32_t p_playerID)
        {
            std::map<uint32_t, PlayerServer*>::iterator iter = m_playerMap.find(p_playerID);

            FrequencyBufferHandler* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = iter->second->m_frequencyBufferHandler;
            }

            return outPointer;
        }

        bool PlayerHandlerServer::IsPlayer(EntityID p_entityID)
        {
            for(std::map<uint32_t, PlayerServer*>::iterator iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                if(iter->second->m_playerEntityID == p_entityID)
                {
                    return true;
                }
            }
            return false;
        }

        void PlayerHandlerServer::Update(double p_dt)
        {
            TIME_FUNCTION_START

            // Update all players
            for(auto& t_player : m_playerMap)
            {
                UpdatePlayerInputs(t_player.second);
                UpdatePlayerPositions(t_player.second);
                UpdatePlayerRotations(t_player.second);
                UpdateFiring(t_player.second);
                UpdateNetworkObjectPriority(t_player.second, p_dt);
                t_player.second->m_frequencyBufferHandler->Update();
            }


            TIME_FUNCTION_STOP
        }

        void PlayerHandlerServer::UpdatePlayerInputs(Player* t_player)
        {
            TIME_FUNCTION_START

                ((InputHandlerServer*)t_player->m_inputHandler)
                    ->Update(t_player->m_playerEntityID);

            TIME_FUNCTION_STOP
        }

        void PlayerHandlerServer::UpdatePlayerRotations(Player* t_player)
        {
            TIME_FUNCTION_START

            InputHandlerServer* inputHandler = (InputHandlerServer*)t_player->m_inputHandler;

            EntityID entityID = t_player->m_playerEntityID;

            if(EntityHandler::GetInstance().HasComponents(entityID, (int)ComponentType::CharacterController | (int)ComponentType::Transform))
            {
                TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(entityID);
                transComp->rotation = inputHandler->GetOrientationFromInput();
            }

            TIME_FUNCTION_STOP
        }

        void PlayerHandlerServer::CreateNewPlayer(uint32_t p_playerID, InputHandler* p_inputHandler)
        {
            // Error check if we create duplicate, shouoldn't be able to happen
            if(m_playerMap.count(p_playerID) == 0)
            {
                std::runtime_error("Creating player twice with same ID.");
            }

            PlayerServer* NewPlayer;
            XMFLOAT3 newPosition;
            EntityHandler& entityHandler = EntityHandler::GetInstance();

            // If we got the id saved as a disconnect, we use the saved values
            std::map<uint32_t, InactivePlayerServer*>::iterator iterInact = m_inactivePlayers.find(p_playerID);
            if(iterInact != m_inactivePlayers.end())
            {
                newPosition = iterInact->second->m_savedPosition;

                // Create the actual playerentity
                EntityID t_EntityID = entityHandler.CreateEntity(Blueprints::PlayerEntity, iterInact->second->m_savedPosition,
                                                                 iterInact->second->m_savedOrientation, XMFLOAT3(0.25, 0.25, 0.25));

                // Set health
                EventHandler::GetInstance()->BroadcastEvent(new SetHealthEvent(t_EntityID, iterInact->second->m_savedHealth));

                // Create a "new" player
                NewPlayer = iterInact->second->m_savedPlayer;

                // Add player to map and remove from inactive
                m_playerMap[p_playerID] = NewPlayer;
                m_inactivePlayers.erase(iterInact);
            }
            else
            {
                // Get the spawner ID and spawn the new player there
                // TODO could change this to checkpoint when we get checkpoints in the game
                uint32_t t_spawnerEntityID = PlayerSpawnerHandler::GetInstance()->GetCurrentSpawnerEntityID();
                DoremiEngine::Physics::RigidBodyManager& t_rigidBodyManager = m_sharedContext.GetPhysicsModule().GetRigidBodyManager();

                // Get position and orientation of the trigger..
                DirectX::XMFLOAT3 t_triggerPosition = t_rigidBodyManager.GetBodyPosition(t_spawnerEntityID);
                DirectX::XMFLOAT4 t_triggerOrientation = t_rigidBodyManager.GetBodyOrientation(t_spawnerEntityID);
                newPosition = t_triggerPosition;

                // Create new stuff needed for a Player
                NetworkEventSender* newNetworkEventSender = new NetworkEventSender();
                FrequencyBufferHandler* newFrequencyHandler = new FrequencyBufferHandler();
                NetworkPriorityHandler* newNetworkPriorityHandler = new NetworkPriorityHandler(m_sharedContext);

                // Create the actual playerentity
                const EntityID t_EntityID = entityHandler.CreateEntity(Blueprints::PlayerEntity, t_triggerPosition, t_triggerOrientation);

                // Create a new player
                NewPlayer = new PlayerServer(t_EntityID, p_inputHandler, newFrequencyHandler, newNetworkPriorityHandler, newNetworkEventSender);

                // Add player to map
                m_playerMap[p_playerID] = NewPlayer;
            }

            /// Add a new potential field actor to the player
            // Check if we have a actor, different from server and client...
            if(entityHandler.HasComponents(NewPlayer->m_playerEntityID, (int)ComponentType::PotentialField))
            {
                PotentialFieldComponent* pfComponent = entityHandler.GetComponentFromStorage<PotentialFieldComponent>(NewPlayer->m_playerEntityID);
                pfComponent->ChargedActor =
                    m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(newPosition, 30, 60, false, DoremiEngine::AI::AIActorType::Player);
            }

            // Set start and endEvent for client
            SetupRejoin(p_playerID);

            // Create event & broadcast event
            PlayerCreationEvent* playerCreateEvent = new PlayerCreationEvent(NewPlayer->m_playerEntityID);
            EventHandler::GetInstance()->BroadcastEvent(playerCreateEvent);
        }

        void PlayerHandlerServer::RemoveAndSavePlayer(uint32_t p_playerID)
        {
            // If playerID exist
            if(m_playerMap.count(p_playerID))
            {
                // Create new inactive player
                InactivePlayerServer* t_newInactivePlayer = new InactivePlayerServer();

                // Get transcomp and player object
                const TransformComponent* const t_transComp = GetComponent<TransformComponent>(m_playerMap.at(p_playerID)->m_playerEntityID);
                PlayerServer* t_playerServ = static_cast<PlayerServer*>(m_playerMap.at(p_playerID));

                // Save position, orientation, health
                t_newInactivePlayer->m_savedPosition = t_transComp->position;
                t_newInactivePlayer->m_savedOrientation = t_transComp->rotation;
                t_newInactivePlayer->m_savedHealth = GetComponent<HealthComponent>(m_playerMap.at(p_playerID)->m_playerEntityID)->currentHealth;
                t_newInactivePlayer->m_savedPlayer = t_playerServ;

                // Set input to 0, and clear buffer
                // TODOCM If we're based on 0 sequence or something we'll need to fix this, seems not to be a problem with input?
                static_cast<InputHandlerServer*>(t_playerServ->m_inputHandler)->ClearInput();

                // Remove the entity
                EntityHandler::GetInstance().RemoveEntity(t_playerServ->m_playerEntityID);

                // Erase from active players, and add to inactive players
                m_playerMap.erase(p_playerID);
                m_inactivePlayers[p_playerID] = t_newInactivePlayer;
            }
        }


        bool PlayerHandlerServer::InactivePlayerIDExists(const uint32_t& p_playerID)
        {
            std::map<uint32_t, InactivePlayerServer*>::iterator iter = m_inactivePlayers.find(p_playerID);
            if(iter != m_inactivePlayers.end())
            {
                cout << "Found inactive playerID..." << endl;
                return true;
            }

            return false;
        }

        bool PlayerHandlerServer::ActivePlayerIDExists(const uint32_t& p_playerID)
        {
            std::map<uint32_t, PlayerServer*>::iterator iter = m_playerMap.find(p_playerID);
            if(iter != m_playerMap.end())
            {
                cout << "Found active playerID..." << endl;
                return true;
            }

            return false;
        }

        uint32_t PlayerHandlerServer::GetMaxEventForPlayer(uint32_t p_playerID)
        {
            std::map<uint32_t, PlayerServer*>::iterator iter = m_playerMap.find(p_playerID);

            NetworkEventSender* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                return static_cast<PlayerServer*>(iter->second)->m_EndEvent;
            }

            return 0;
        }

        NetworkEventSender* PlayerHandlerServer::GetNetworkEventSenderForPlayer(uint32_t p_playerID)
        {
            std::map<uint32_t, PlayerServer*>::iterator iter = m_playerMap.find(p_playerID);

            NetworkEventSender* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = (static_cast<PlayerServer*>(iter->second))->m_networkEventSender;
            }

            return outPointer;
        }

        NetworkPriorityHandler* PlayerHandlerServer::GetNetworkPriorityHandlerForplayer(uint32_t p_playerID)
        {
            std::map<uint32_t, PlayerServer*>::iterator iter = m_playerMap.find(p_playerID);

            NetworkPriorityHandler* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = (static_cast<PlayerServer*>(iter->second))->m_networkPriorityHandler;
            }

            return outPointer;
        }

        void PlayerHandlerServer::UpdateNetworkObjectPriority(Player* t_player, double p_dt)
        {
            (static_cast<PlayerServer*>(t_player))->m_networkPriorityHandler->Update(t_player->m_playerEntityID, p_dt);
        }

        void PlayerHandlerServer::AddNetObjectToPlayers(const EntityID& p_entityID)
        {
            for(auto& t_player : m_playerMap)
            {
                t_player.second->m_networkPriorityHandler->UpdateNetworkObject(p_entityID);
            }
        }

        void PlayerHandlerServer::SetupRejoin(uint32_t t_playerID)
        {
            if(m_playerMap.count(t_playerID) == 0)
            {
                // Something really wrong
                cout << "Error when setup rejoin, playerID missing." << endl;
                std::runtime_error("Error when setup rejoin, playerID missing.");
            }
            PlayerServer* const t_player = static_cast<PlayerServer*>(m_playerMap.at(t_playerID));

            // reset start and set new end
            t_player->m_StartEvent = 0;
            t_player->m_EndEvent = m_lateJoinEventQueue.size();

            // Get network event sender
            NetworkEventSender* t_netEventSender = t_player->m_networkEventSender;

            // Now when we created the event box, we can add health events and setposition
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();
            const size_t t_numEntities = t_entityHandler.GetLastEntityIndex();
            uint32_t t_mask = static_cast<uint32_t>(ComponentType::Health);
            for(size_t entityID = 0; entityID < t_numEntities; entityID++)
            {
                if(t_entityHandler.HasComponents(entityID, t_mask))
                {
                    t_netEventSender->QueueEventToFrame(new SetHealthEvent(entityID, GetComponent<HealthComponent>(entityID)->currentHealth));
                }
            }

            // Set positions, we might not actuall
            t_mask = static_cast<uint32_t>(ComponentType::Transform) | static_cast<uint32_t>(ComponentType::NetworkObject);
            for(size_t entityID = 0; entityID < t_numEntities; entityID++)
            {
                if(t_entityHandler.HasComponents(entityID, t_mask))
                {
                    TransformComponent* t_transComp = GetComponent<TransformComponent>(entityID);
                    t_netEventSender->QueueEventToFrame(new SetTransformEvent(entityID, t_transComp->position, t_transComp->rotation));
                }
            }
        }

        bool PlayerHandlerServer::UpdateRejoinQueueForPlayer(uint32_t p_EventSlot, uint32_t t_playerID)
        {
            if(m_playerMap.count(t_playerID) == 0)
            {
                // Something really wrong
                cout << "Error when setup rejoin, playerID missing." << endl;
                std::runtime_error("Error when setup rejoin, playerID missing.");
            }
            PlayerServer* t_player = static_cast<PlayerServer*>(m_playerMap.at(t_playerID));

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
            if(m_playerMap.count(t_playerID) == 0)
            {
                // Something really wrong
                cout << "Error when sending events on rejoin, playerID missing." << endl;
                std::runtime_error("Error when sending events on rejoin, playerID missing.");
            }
            PlayerServer* const t_player = static_cast<PlayerServer*>(m_playerMap.at(t_playerID));

            // Write start event
            const uint32_t t_eventStart = t_player->m_StartEvent;
            const size_t t_eventEnd = t_player->m_EndEvent;
            p_streamer.WriteUnsignedInt32(t_eventStart);
            op_bytesWritten += sizeof(uint32_t);

            // Save position to wirte number of events
            uint32_t t_numOfEvents = 0;
            const uint32_t t_posToWriteNumEvents = op_bytesWritten;
            op_bytesWritten += sizeof(uint32_t);
            p_streamer.SetReadWritePosition(op_bytesWritten);

            uint32_t t_bitsWritten = 0;
            uint32_t t_bitIncrement = 0;
            uint32_t t_bitsLeftToWrite = (p_bufferSize - op_bytesWritten) * 8;

            // While we got events left to send and we have space left to wirte
            for(size_t i = t_eventStart; i < t_eventEnd && t_bitsWritten < t_bitsLeftToWrite; ++i)
            {
                t_bitIncrement = 0;
                p_streamer.WriteUnsignedInt8(static_cast<uint8_t>(m_lateJoinEventQueue[i]->eventType));
                t_bitIncrement += sizeof(uint8_t) * 8;

                m_lateJoinEventQueue[i]->Write(&p_streamer, t_bitIncrement);
                t_bitsWritten += t_bitIncrement;
                ++t_numOfEvents;
            }

            // if we overdid it, take a step back
            if(t_bitsWritten > t_bitsLeftToWrite)
            {
                t_bitsWritten -= t_bitIncrement;
                --t_numOfEvents;
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
            for(auto& t_player : m_playerMap)
            {
                Blueprints blueprint = p_entityCreatedEvent->bluepirnt;

                // If we create a player entity, we want to create a network player on other clients
                if(blueprint == Blueprints::PlayerEntity && !p_entityCreatedEvent->entityID == t_player.second->m_playerEntityID)
                {
                    blueprint = Blueprints::NetworkPlayerEntity;
                }

                (static_cast<PlayerServer*>(t_player.second))
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
            for(auto& t_player : m_playerMap)
            {
                t_player.second->m_networkEventSender->QueueEventToFrame(new RemoveEntityEvent(*p_removeEvent));
            }

            // Save it for later joins
            m_lateJoinEventQueue.push_back(new RemoveEntityEvent(*p_removeEvent));
        }

        void PlayerHandlerServer::QueuePlayerRespawnEventToPlayers(PlayerRespawnEvent* p_playerRespawnEvent)
        {
            // Go through all players
            for(auto& t_player : m_playerMap)
            {
                t_player.second->m_networkEventSender->QueueEventToFrame(new PlayerRespawnEvent(*p_playerRespawnEvent));
            }
        }

        void PlayerHandlerServer::QueueGunFireToggleEventToPlayers(GunFireToggleEvent* t_gunFireToggleEvent)
        {
            // Go through all players
            for(auto& t_player : m_playerMap)
            {
                if(t_player.second->m_playerEntityID == t_gunFireToggleEvent->entityID)
                {
                    continue;
                }
                (static_cast<PlayerServer*>(t_player.second))->m_networkEventSender->QueueEventToFrame(new GunFireToggleEvent(*t_gunFireToggleEvent));
            }
        }

        void PlayerHandlerServer::QueueSetHealthEventToPlayers(SetHealthEvent* t_setHealthEvent)
        {
            // Go through all players
            for(auto& t_player : m_playerMap)
            {
                (static_cast<PlayerServer*>(t_player.second))->m_networkEventSender->QueueEventToFrame(new SetHealthEvent(*t_setHealthEvent));
            }
        }

        void PlayerHandlerServer::QueueSetTransformEventToPlayers(SetTransformEvent* t_setTransformEvent)
        {
            // Go through all players
            for(auto& t_player : m_playerMap)
            {
                t_player.second->m_networkEventSender->QueueEventToFrame(new SetTransformEvent(*t_setTransformEvent));
            }
        }

        void PlayerHandlerServer::QueueAnimationTransitionToPlayers(AnimationTransitionEvent* t_animationTransitionEvent)
        {
            // Go through all players
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                if(iter->second->m_playerEntityID == t_animationTransitionEvent->entityID)
                {
                    continue;
                }
                (static_cast<PlayerServer*>(iter->second))->m_networkEventSender->QueueEventToFrame(new AnimationTransitionEvent(*t_animationTransitionEvent));
            }
        }

        void PlayerHandlerServer::QueueDamageEventToPlayers(DamageTakenEvent* t_takeDamageEvent)
        {
            // Go through all players
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                (static_cast<PlayerServer*>(iter->second))->m_networkEventSender->QueueEventToFrame(new DamageTakenEvent(*t_takeDamageEvent));
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
                case Doremi::Core::EventType::SetHealth:
                {
                    SetHealthEvent* t_setHealthEvent = static_cast<SetHealthEvent*>(p_event);

                    QueueSetHealthEventToPlayers(t_setHealthEvent);

                    break;
                }
                case Doremi::Core::EventType::SetTransform:
                {
                    SetTransformEvent* t_setTransformEvent = static_cast<SetTransformEvent*>(p_event);

                    QueueSetTransformEventToPlayers(t_setTransformEvent);

                    break;
                }
                case Doremi::Core::EventType::AnimationTransition:
                {
                    AnimationTransitionEvent* t_animationTransitionEvent = static_cast<AnimationTransitionEvent*>(p_event);

                    QueueAnimationTransitionToPlayers(t_animationTransitionEvent);
                    break;
                }
                case Doremi::Core::EventType::DamageTaken:
                {
                    DamageTakenEvent* t_damageTakenEvent = static_cast<DamageTakenEvent*>(p_event);

                    QueueDamageEventToPlayers(t_damageTakenEvent);
                }
                default:
                    break;
            }
        }
    }
}