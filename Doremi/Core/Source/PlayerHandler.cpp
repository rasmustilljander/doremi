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
#include <InputHandlerClient.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlayerCreationEvent.hpp>
#include <Doremi/Core/Include/InputHandlerServer.hpp>
#include <Doremi/Core/Include/NetworkEventSender.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>
#include <Doremi/Core/Include/InputHandler.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>
#include <Doremi/Core/Include/NetworkPriorityHandler.hpp>

#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/EventHandler/Events/RemoveEntityEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/EntityCreatedEvent.hpp>

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
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>

#include <iostream>


namespace Doremi
{
    namespace Core
    {
        PlayerHandler::PlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            EventHandler* t_EventHandler = EventHandler::GetInstance();
            t_EventHandler->Subscribe(EventType::RemoveEntity, this);
            t_EventHandler->Subscribe(EventType::EntityCreated, this);
        }

        PlayerHandler::~PlayerHandler() {}

        PlayerHandler* PlayerHandler::m_singleton = nullptr;

        void PlayerHandler::StartPlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("PlayerHandler StartPlayerHandler called multiple times.");
            }
            m_singleton = new PlayerHandler(p_sharedContext);
        }

        PlayerHandler* PlayerHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("PlayerHandler not initialized, GetInstance called.");
            }

            return m_singleton;
        }

        std::map<uint32_t, Player*>& PlayerHandler::GetPlayerMap() { return m_playerMap; }

        InputHandler* PlayerHandler::GetDefaultInputHandler()
        {
            InputHandler* OutPointer = nullptr;

            if(m_playerMap.size())
            {
                OutPointer = m_playerMap.begin()->second->m_inputHandler;
            }

            return OutPointer;
        }

        InputHandler* PlayerHandler::GetInputHandlerForPlayer(uint32_t p_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            InputHandler* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = iter->second->m_inputHandler;
            }

            return outPointer;
        }

        NetworkEventSender* PlayerHandler::GetNetworkEventSenderForPlayer(uint32_t p_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            NetworkEventSender* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = iter->second->m_networkEventSender;
            }

            return outPointer;
        }

        FrequencyBufferHandler* PlayerHandler::GetDefaultFrequencyBufferHandler()
        {
            FrequencyBufferHandler* OutPointer = nullptr;

            if(m_playerMap.size())
            {
                OutPointer = m_playerMap.begin()->second->m_frequencyBufferHandler;
            }

            return OutPointer;
        }

        FrequencyBufferHandler* PlayerHandler::GetFrequencyBufferHandlerForPlayer(uint32_t p_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            FrequencyBufferHandler* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = iter->second->m_frequencyBufferHandler;
            }

            return outPointer;
        }

        NetworkPriorityHandler* PlayerHandler::GetNetworkPriorityHandlerForplayer(uint32_t p_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            NetworkPriorityHandler* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = iter->second->m_networkPriorityHandler;
            }

            return outPointer;
        }

        bool PlayerHandler::GetDefaultPlayerEntityID(EntityID& o_outID)
        {
            if(m_playerMap.size() == 0)
            {
                std::runtime_error("GetDefaultPlayerEntityID called without any players aviable.");
                return false;
            }
            o_outID = m_playerMap.begin()->second->m_playerEntityID;

            return true;
        }

        bool PlayerHandler::GetEntityIDForPlayer(uint32_t p_playerID, EntityID& outID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);


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

        void PlayerHandler::CreateNewPlayer(uint32_t p_playerID, InputHandler* p_inputHandler)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);


            if(iter != m_playerMap.end())
            {
                std::runtime_error("Creating player twice with same ID.");
            }

            // TODOCM hard coded entityID for new players
            EntityID t_EntityID = EntityHandler::GetInstance().CreateEntity(Blueprints::PlayerEntity);

            NetworkEventSender* newNetworkEventSender = new NetworkEventSender();
            FrequencyBufferHandler* newFrequencyHandler = new FrequencyBufferHandler();
            NetworkPriorityHandler* newNetworkPriorityHandler = new NetworkPriorityHandler(m_sharedContext);

            Player* NewPlayer = new Player(t_EntityID, p_inputHandler, newNetworkEventSender, newFrequencyHandler, newNetworkPriorityHandler);

            m_playerMap[p_playerID] = NewPlayer;


            int materialID = EntityHandler::GetInstance().GetComponentFromStorage<Core::PhysicsMaterialComponent>(NewPlayer->m_playerEntityID)->p_materialID;
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(5.0f, 30.0f, 0.0f);
            DirectX::XMFLOAT4 orientation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

            /// Create the gun
            // Check if we have the gun
            if(EntityHandler::GetInstance().HasComponents(NewPlayer->m_playerEntityID, (int)ComponentType::PressureParticleSystem))
            {
                ParticlePressureComponent* particleComp =
                    EntityHandler::GetInstance().GetComponentFromStorage<ParticlePressureComponent>(NewPlayer->m_playerEntityID);
                particleComp->data.m_active = false;
                particleComp->data.m_density = 2.0f;
                particleComp->data.m_dimensions = XMFLOAT2(0.0f, 0.0f);
                particleComp->data.m_direction = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(NewPlayer->m_playerEntityID)->rotation;
                particleComp->data.m_emissionAreaDimensions = XMFLOAT2(3.14 / 4, 3.14 / 5);
                particleComp->data.m_emissionRate = 0.05;
                particleComp->data.m_launchPressure = 100;
                particleComp->data.m_numParticlesX = 5;
                particleComp->data.m_numParticlesY = 1;
                particleComp->data.m_size = 1;
                particleComp->data.m_position = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(NewPlayer->m_playerEntityID)->position;
                m_sharedContext.GetPhysicsModule().GetFluidManager().CreateParticleEmitter(NewPlayer->m_playerEntityID, particleComp->data);
            }


            // Create event
            PlayerCreationEvent* playerCreateEvent = new PlayerCreationEvent(NewPlayer->m_playerEntityID);

            // Broadcast event
            EventHandler::GetInstance()->BroadcastEvent(playerCreateEvent);
        }

        void PlayerHandler::UpdateClient()
        {
            TIME_FUNCTION_START
            
            UpdatePlayerPositions();
            UpdatePlayerRotationsClient();
            TIME_FUNCTION_STOP
        }

        void PlayerHandler::UpdateServer(double p_dt)
        {
            TIME_FUNCTION_START
            UpdatePlayerInputsServer();
            UpdatePlayerPositions();
            UpdatePlayerRotationsServer();
            UpdateFiring();
            UpdateNetworkObjectPriority(p_dt);

            // Hotfix
            std::map<uint32_t, Player*>::iterator iter;

            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                iter->second->m_frequencyBufferHandler->Update();
            }


            TIME_FUNCTION_STOP
        }

        void PlayerHandler::UpdatePlayerInputsClient()
        {
            TIME_FUNCTION_START
            m_sharedContext.GetInputModule().Update();

            std::map<uint32_t, Player*>::iterator iter;

            for (iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                ((InputHandlerClient*)iter->second->m_inputHandler)->Update();
            }
            TIME_FUNCTION_STOP
        }

        void PlayerHandler::UpdatePlayerInputsServer()
        {
            TIME_FUNCTION_START
            std::map<uint32_t, Player*>::iterator iter;

            for (iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                ((InputHandlerServer*)iter->second->m_inputHandler)->Update(iter->second->m_playerEntityID);
            }
            TIME_FUNCTION_STOP
        }

        // TODOEA fix this code up with comments
        // TODO fix code with comments
        void PlayerHandler::UpdatePlayerPositions()
        {
            TIME_FUNCTION_START
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                int t_checkIfWeHavePlayer = EntityHandler::GetInstance().GetLastEntityIndex();

                XMFLOAT3 t_entityVelocity = XMFLOAT3(0, 0, 0);

                InputHandler* inputHandler = iter->second->m_inputHandler;

                EntityID entityID = iter->second->m_playerEntityID;

                if(EntityHandler::GetInstance().HasComponents(entityID, (int)ComponentType::CharacterController | (int)ComponentType::Transform))
                {
                    // Get transform component
                    TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(entityID);
                    // Get direction
                    XMFLOAT4 orientation = transComp->rotation;
                    // Start with standard direction
                    XMFLOAT3 dir = XMFLOAT3(0, 0, 1);
                    XMVECTOR dirVec = XMLoadFloat3(&dir);
                    // Create rotation matrix with orientation quaternion
                    XMVECTOR orientationVec = XMLoadFloat4(&orientation);
                    XMMATRIX rotMat = XMMatrixRotationQuaternion(orientationVec);
                    // Rotate "start vector" with rotation matrix, giving us the target vector
                    dirVec = XMVector3Transform(dirVec, rotMat);
                    // Create right vector the same way
                    XMFLOAT3 right = XMFLOAT3(1, 0, 0);
                    XMVECTOR rightVec = XMLoadFloat3(&right);
                    rightVec = XMVector3Transform(rightVec, rotMat);
                    // WARNING! Might be necessary to consider up-vector (project into XZ-plane or something?)

                    /// Handle keyboard input
                    // Start by creating a movement vector
                    XMFLOAT3 movement = XMFLOAT3(0, 0, 0);
                    XMVECTOR movementVec = XMLoadFloat3(&movement);
                    if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Forward))
                    {
                        movementVec += dirVec * iter->second->m_moveSpeed;
                    }

                    if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Backward))
                    {
                        movementVec -= dirVec * iter->second->m_moveSpeed;
                    }
                    if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Left))
                    {
                        movementVec -= rightVec * iter->second->m_moveSpeed;
                    }
                    if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Right))
                    {
                        movementVec += rightVec * iter->second->m_moveSpeed;
                    }

                    if(inputHandler->CheckForOnePress((int)UserCommandPlaying::Jump))
                    {
                        EntityHandler::GetInstance().GetComponentFromStorage<JumpComponent>(entityID)->StartJump();
                    }

                    // Store finished movement vec
                    XMStoreFloat3(&movement, movementVec);
                    if(movement.x != 0 || movement.y != 0 || movement.z != 0)
                    {
                        MovementComponent* moveComp = EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(entityID);
                        moveComp->movement.x += movement.x;
                        moveComp->movement.y += movement.y;
                        moveComp->movement.z += movement.z;
                    }
                }
            }
            TIME_FUNCTION_STOP
        }

        void PlayerHandler::UpdatePlayerRotationsClient()
        {
            TIME_FUNCTION_START
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                InputHandlerClient* inputHandler = (InputHandlerClient*)iter->second->m_inputHandler;

                EntityID entityID = iter->second->m_playerEntityID;

                if(EntityHandler::GetInstance().HasComponents(entityID, (int)ComponentType::CharacterController | (int)ComponentType::Transform))
                {
                    /// Handle mouse input
                    // Get mouse input
                    int t_mouseMovementX = inputHandler->GetMouseMovementX();
                    if(t_mouseMovementX)
                    {
                        TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(entityID);
                        // Get direction
                        XMFLOAT4 orientation = transComp->rotation;

                        // Create rotation matrix with orientation quaternion
                        XMVECTOR orientationVec = XMLoadFloat4(&orientation);

                        // Get current angle
                        float angle;
                        XMVECTOR oldDir; // This really is only needed for the parameter below...
                        XMQuaternionToAxisAngle(&oldDir, &angle, orientationVec);
                        // Change the angle
                        angle += t_mouseMovementX * iter->second->m_turnSpeed;
                        // Single quaternions don't really like angles over 2*pi, we do this
                        if(angle > 2.0f * 3.1415f)
                        {
                            angle -= 2.0f * 3.1415f;
                        }
                        else if(angle < 0)
                        {
                            angle += 2.0f * 3.1415f;
                        }
                        // Create a new quaternion with the new angle
                        orientationVec = XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(0, 1, 0)), angle);
                        // Store results
                        XMStoreFloat4(&transComp->rotation, orientationVec);
                    }
                }

                if(EntityHandler::GetInstance().HasComponents(entityID, (int)ComponentType::PressureParticleSystem))
                {
                    if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::LeftClick))
                    {
                        m_gunController.FireGun(entityID, iter->second, m_sharedContext);
                    }
                    else
                    {
                        // Set the particle emitter to not be active if we are not firing
                        GetComponent<ParticlePressureComponent>(entityID)->data.m_active = false;
                        m_sharedContext.GetPhysicsModule().GetFluidManager().SetParticleEmitterData(entityID,
                                                                                                    GetComponent<ParticlePressureComponent>(entityID)->data);
                    }
                }

                // Fire weapon TODOJB move this someplace that makes sense. Also fix input. Scroll wheel is silly...
                if(inputHandler->CheckForOnePress((int)UserCommandPlaying::ScrollWpnUp))
                {

                    ///// Calculate where we want the shot to appear
                    //// Get position and orientation of player
                    // XMFLOAT4 orientation = GetComponent<TransformComponent>(entityID)->rotation;
                    // XMFLOAT3 playerPos = GetComponent<TransformComponent>(entityID)->position;
                    // XMFLOAT3 bulletPos = playerPos;

                    //// Get direction of player
                    // XMMATRIX rotMat = XMMatrixRotationQuaternion(XMLoadFloat4(&orientation));
                    // XMVECTOR dirVec = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0, 0, 1)), rotMat);

                    //// Add some distance
                    // float offsetDist = 2.5;
                    // XMVECTOR bulletPosVec = XMLoadFloat3(&bulletPos);
                    // bulletPosVec += dirVec * offsetDist;
                    // XMStoreFloat3(&bulletPos, bulletPosVec);

                    //// create the bullet
                    // int bulletID = EntityHandler::GetInstance().CreateEntity(Blueprints::BulletEntity);
                    //// Get the material id
                    // int physMatID = GetComponent<PhysicsMaterialComponent>(bulletID)->p_materialID;
                    //// Create rigid body for the bullet
                    // RigidBodyComponent* rigidComp = GetComponent<RigidBodyComponent>(bulletID);
                    // rigidComp->p_bodyID = m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(bulletID, bulletPos,
                    // orientation,
                    //                                                                                                 XMFLOAT3(0.5, 0.5, 0.5),
                    //                                                                                                 physMatID);
                    //// Set start velocity
                    // float fireVelocity = 50;
                    // XMVECTOR bulletVelVec = dirVec * fireVelocity;
                    // XMFLOAT3 bulletVel;
                    // XMStoreFloat3(&bulletVel, bulletVelVec);
                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(rigidComp->p_bodyID, bulletVel);
                }
            }
            TIME_FUNCTION_STOP
        }

        void PlayerHandler::UpdatePlayerRotationsServer()
        {
            TIME_FUNCTION_START
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                InputHandlerServer* inputHandler = (InputHandlerServer*)iter->second->m_inputHandler;

                EntityID entityID = iter->second->m_playerEntityID;

                if(EntityHandler::GetInstance().HasComponents(entityID, (int)ComponentType::CharacterController | (int)ComponentType::Transform))
                {
                    TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(entityID);
                    transComp->rotation = inputHandler->GetOrientationFromInput();
                }
            }
            TIME_FUNCTION_STOP
        }

        void PlayerHandler::UpdateFiring()
        {
            // THOS DOES NOTHING AT THIS MOMENT
            // TODOJB fix gun on server
            TIME_FUNCTION_START
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                InputHandlerServer* inputHandler = (InputHandlerServer*)iter->second->m_inputHandler;

                EntityID entityID = iter->second->m_playerEntityID;
                // Check if player fires the gun. TODOJB strange to have it in this method? Refactor into overall UpdatePlayerServer method?
                if(EntityHandler::GetInstance().HasComponents(entityID, (int)ComponentType::PressureParticleSystem))
                {
                    if(inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::LeftClick))
                    {
                        m_gunController.FireGun(entityID, iter->second, m_sharedContext);
                    }
                    else
                    {
                        // Set the particle emitter to not be active if we are not firing
                        GetComponent<ParticlePressureComponent>(entityID)->data.m_active = false;
                        m_sharedContext.GetPhysicsModule().GetFluidManager().SetParticleEmitterData(entityID,
                                                                                                    GetComponent<ParticlePressureComponent>(entityID)->data);
                    }
                }
            }
            TIME_FUNCTION_STOP
        }

        void PlayerHandler::UpdateNetworkObjectPriority(double p_dt)
        {
            // For each player we update their networkobj priorities
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                iter->second->m_networkPriorityHandler->Update(iter->second->m_playerEntityID, p_dt);
            }
        }

        void PlayerHandler::QueueAddObjectToPlayers(uint32_t p_blueprint, DirectX::XMFLOAT3 p_position)
        {
            // Fix because there is a difference between "myPlayer" and "otherplayers" for the client
            if(p_blueprint == (uint32_t)Blueprints::PlayerEntity)
            {
                p_blueprint = (uint32_t)Blueprints::NetworkPlayerEntity;
            }

            // For each player we queue
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                iter->second->m_networkEventSender->QueueAddObject(p_blueprint, p_position);
            }
        }

        void PlayerHandler::QueueRemoveObjectToPlayers(uint32_t p_entityID)
        {
            // For each player we queue
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                iter->second->m_networkEventSender->QueueRemoveObject(p_entityID);
            }
        }

        void PlayerHandler::UpdateAddRemoveObjects()
        {
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                iter->second->m_networkEventSender->AddRemoveQueuedObjects();
            }
        }

        uint32_t PlayerHandler::GetNumOfPlayers() { return m_playerMap.size(); }

        void PlayerHandler::RemoveAllPlayers()
        {
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                delete iter->second;
            }
            m_playerMap.clear();
        }

        void PlayerHandler::AddNetObjectToPlayers(const EntityID& p_entityID)
        {
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                iter->second->m_networkPriorityHandler->UpdateNetworkObject(p_entityID);
            }
        }

        void PlayerHandler::OnEvent(Event* p_event)
        {
            switch(p_event->eventType)
            {
                case Doremi::Core::EventType::EntityCreated:
                {
                    EntityCreatedEvent* t_entityCreatedEvent = static_cast<EntityCreatedEvent*>(p_event);

                    // If the object is a Netobject we need to add those components to each seperate player, a solution by the cause that we want
                    // dynamic amount of players
                    // and individual components for each of them
                    if(EntityHandler::GetInstance().HasComponents(t_entityCreatedEvent->entityID, (int)ComponentType::NetworkObject))
                    {
                        AddNetObjectToPlayers(t_entityCreatedEvent->entityID);
                    }

                    QueueAddObjectToPlayers((uint32_t)t_entityCreatedEvent->bluepirnt, GetComponent<TransformComponent>(t_entityCreatedEvent->entityID)->position);

                    break;
                }
                case Doremi::Core::EventType::RemoveEntity:
                {

                    RemoveEntityEvent* p_removeEvent = static_cast<RemoveEntityEvent*>(p_event);

                    PlayerHandler::GetInstance()->QueueRemoveObjectToPlayers(p_removeEvent->entityID);
                    // m_sinceGameStartAddRemoves.push_back(AddRemoveStruct(false, p_removeEvent->entityID));

                    break;
                }
                default:
                    break;
            }
        }
    }
}