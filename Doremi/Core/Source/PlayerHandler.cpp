#pragma once
// Project specific
#include <PlayerHandler.hpp>
#include <Doremi/Core/Include/InputHandler.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
//#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>
#include <EntityComponent/Components/JumpComponent.hpp>
#include <EntityComponent/Components/GravityComponent.hpp>
#include <InputHandlerClient.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlayerCreationEvent.hpp>
#include <Doremi/Core/Include/InputHandlerServer.hpp>
#include <Doremi/Core/Include/AddRemoveSyncHandler.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>

// Timing
#include <Utility/Timer/Include/Measure/MeasureTimer.hpp>

#include <iostream>


namespace Doremi
{
    namespace Core
    {
        PlayerHandler::PlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

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

        AddRemoveSyncHandler* PlayerHandler::GetAddRemoveSyncHandlerForPlayer(uint32_t p_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            AddRemoveSyncHandler* outPointer = nullptr;

            if(iter != m_playerMap.end())
            {
                outPointer = iter->second->m_addRemoveSyncHandler;
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

            AddRemoveSyncHandler* newAddRemoveSyncHandler = new AddRemoveSyncHandler();
            FrequencyBufferHandler* newFrequencyHandler = new FrequencyBufferHandler();

            Player* NewPlayer = new Player(p_inputHandler, newAddRemoveSyncHandler, newFrequencyHandler);

            // TODOCM hard coded entityID for new players
            NewPlayer->m_playerEntityID = EntityHandler::GetInstance().CreateEntity(Blueprints::PlayerEntity);

            m_playerMap[p_playerID] = NewPlayer;


            int materialID = EntityHandler::GetInstance().GetComponentFromStorage<Core::PhysicsMaterialComponent>(NewPlayer->m_playerEntityID)->p_materialID;
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(5, 30, 0);
            DirectX::XMFLOAT4 orientation = DirectX::XMFLOAT4(0, 0, 0, 1);
            m_sharedContext.GetPhysicsModule().GetCharacterControlManager().AddController(NewPlayer->m_playerEntityID, materialID, position, XMFLOAT2(1, 1));

            // Create event
            PlayerCreationEvent* playerCreateEvent = new PlayerCreationEvent(NewPlayer->m_playerEntityID);

            // Broadcast event
            EventHandler::GetInstance()->BroadcastEvent(playerCreateEvent);
        }

        void PlayerHandler::UpdateClient()
        {
            TIME_FUNCTION_START
            // UpdatePlayerInputs();
            UpdatePlayerPositions();
            UpdatePlayerRotationsClient();
            TIME_FUNCTION_STOP
        }

        void PlayerHandler::UpdateServer()
        {
            TIME_FUNCTION_START
            UpdatePlayerPositions();
            UpdatePlayerRotationsServer();
            UpdateFiring();


            std::map<uint32_t, Player*>::iterator iter;

            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                iter->second->m_frequencyBufferHandler->GetFrequencyForFrame();
            }
            TIME_FUNCTION_STOP
        }

        void PlayerHandler::UpdatePlayerInputs()
        {
            TIME_FUNCTION_START
            m_sharedContext.GetInputModule().Update();

            std::map<uint32_t, Player*>::iterator iter;

            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                ((InputHandlerClient*)iter->second->m_inputHandler)->Update();
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
                        EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(entityID)->movement = movement;
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
            TIME_FUNCTION_START
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                InputHandlerServer* inputHandler = (InputHandlerServer*)iter->second->m_inputHandler;

                EntityID entityID = iter->second->m_playerEntityID;
                // Check if player fires the gun. TODOJB strange to have it in this method? Refactor into overall UpdatePlayerServer method?
                if(inputHandler->CheckForOnePress((int)UserCommandPlaying::ScrollWpnDown))
                {
                    m_gunController.FireGun(entityID, m_sharedContext);
                }
            }
            TIME_FUNCTION_STOP
        }

        void PlayerHandler::QueueAddObjectToPlayers(uint32_t p_blueprint, DirectX::XMFLOAT3 p_position)
        {
            if(p_blueprint == (uint32_t)Blueprints::PlayerEntity)
            {
                p_blueprint = (uint32_t)Blueprints::NetworkPlayerEntity;
            }

            // For each player we queue
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                iter->second->m_addRemoveSyncHandler->QueueAddObject(p_blueprint, p_position);
            }
        }

        void PlayerHandler::QueueRemoveObjectToPlayers(uint32_t p_entityID)
        {
            // For each player we queue
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                iter->second->m_addRemoveSyncHandler->QueueRemoveObject(p_entityID);
            }
        }

        void PlayerHandler::UpdateAddRemoveObjects()
        {
            std::map<uint32_t, Player*>::iterator iter;
            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                iter->second->m_addRemoveSyncHandler->AddRemoveQueuedObjects();
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
    }
}