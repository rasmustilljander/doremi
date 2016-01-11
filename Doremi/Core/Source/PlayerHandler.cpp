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
#include <InputHandlerClient.hpp>
#include <DoremiEngine/Input/Include/InputModule.hpp>

#include <iostream>


namespace Doremi
{
    namespace Core
    {
        PlayerHandler::PlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext)
        {

        }

        PlayerHandler::~PlayerHandler() 
        {

        }

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

        InputHandler* PlayerHandler::GetDefaultInputHandler()
        {
            InputHandler* OutPointer = nullptr;

            if (m_playerMap.size())
            {
                OutPointer = m_playerMap.begin()->second->m_inputHandler;
            }
            
            return OutPointer;
        }

        InputHandler* PlayerHandler::GetInputHandlerForPlayer(uint32_t p_playerID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);

            InputHandler* outPointer = nullptr;

            if (iter != m_playerMap.end())
            {
                outPointer = iter->second->m_inputHandler;
            }

            return outPointer;
        }

        EntityID PlayerHandler::GetDefaultPlayerEntityID()
        {
            if (m_playerMap.size() == 0)
            {
                std::runtime_error("GetDefaultPlayerEntityID called without any players aviable.");
            }
            return m_playerMap.begin()->second->m_playerEntityID;
        }

        bool PlayerHandler::GetEntityIDForPlayer(uint32_t p_playerID, EntityID& outID)
        {
            std::map<uint32_t, Player*>::iterator iter = m_playerMap.find(p_playerID);


            if (iter != m_playerMap.end())
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

            Player* NewPlayer = new Player(p_inputHandler);

            // TODOCM hard coded entityID for new players
            NewPlayer->m_playerEntityID = 0;

            m_playerMap[p_playerID] = NewPlayer;
        }

        void PlayerHandler::UpdatePlayerInputs()
        {
            m_sharedContext.GetInputModule().Update();

            std::map<uint32_t, Player*>::iterator iter;

            for(iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                ((InputHandlerClient*)iter->second->m_inputHandler)->Update();
            }
        }

        // TODOEA fix this code up with comments
        // TODO fix code with comments
        void PlayerHandler::UpdatePlayerPositions()
        {
            std::map<uint32_t, Player*>::iterator iter;
            for (iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                int t_checkIfWeHavePlayer = EntityHandler::GetInstance().GetLastEntityIndex();

                XMFLOAT3 t_entityVelocity = XMFLOAT3(0, 0, 0);

                InputHandler* inputHandler = iter->second->m_inputHandler;

                EntityID entityID = iter->second->m_playerEntityID;

                if (EntityHandler::GetInstance().HasComponents(entityID, (int)ComponentType::CharacterController | (int)ComponentType::Transform))
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
                    if (inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Forward))
                    {
                        movementVec += dirVec * iter->second->m_moveSpeed;
                    }

                    if (inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Backward))
                    {
                        movementVec -= dirVec * iter->second->m_moveSpeed;
                    }
                    if (inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Left))
                    {
                        movementVec -= rightVec * iter->second->m_moveSpeed;
                    }
                    if (inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Right))
                    {
                        movementVec += rightVec * iter->second->m_moveSpeed;
                    }

                    if (inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Jump))
                    {
                        movementVec += XMLoadFloat3(&XMFLOAT3(0, 1, 0)) * 1;
                    }
                    else
                    {
                        movementVec -= XMLoadFloat3(&XMFLOAT3(0, 1, 0)) * 0.1;
                    }
                    // Store finished movement vec
                    XMStoreFloat3(&movement, movementVec);
                    // Directly tell controller to move. TODOJB should be handled with components
                    if(movement.x != 0 || movement.y != 0 || movement.z != 0)
                    {
                        EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(entityID)->movement = movement;
                    }
                }
            }
        }

        void PlayerHandler::UpdatePlayerRotationsClient()
        {
            std::map<uint32_t, Player*>::iterator iter;
            for (iter = m_playerMap.begin(); iter != m_playerMap.end(); ++iter)
            {
                InputHandlerClient* inputHandler = (InputHandlerClient*)iter->second->m_inputHandler;

                EntityID entityID = iter->second->m_playerEntityID;

                if (EntityHandler::GetInstance().HasComponents(entityID, (int)ComponentType::CharacterController | (int)ComponentType::Transform))
                {
                    /// Handle mouse input
                    // Get mouse input
                    int t_mouseMovementX = inputHandler->GetMouseMovementX();
                    if (t_mouseMovementX)
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
                        if (angle > 2.0f * 3.1415f)
                        {
                            angle -= 2.0f * 3.1415f;
                        }
                        else if (angle < 0)
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
                    /// Calculate where we want the shot to appear
                    // Get position and orientation of player
                    XMFLOAT4 orientation = GetComponent<TransformComponent>(entityID)->rotation;
                    XMFLOAT3 playerPos = GetComponent<TransformComponent>(entityID)->position;
                    XMFLOAT3 bulletPos = playerPos;

                    // Get direction of player
                    XMMATRIX rotMat = XMMatrixRotationQuaternion(XMLoadFloat4(&orientation));
                    XMVECTOR dirVec = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0, 0, 1)), rotMat);

                    // Add some distance
                    float offsetDist = 2.5;
                    XMVECTOR bulletPosVec = XMLoadFloat3(&bulletPos);
                    bulletPosVec += dirVec * offsetDist;
                    XMStoreFloat3(&bulletPos, bulletPosVec);

                    // create the bullet
                    int bulletID = EntityHandler::GetInstance().CreateEntity(Blueprints::BulletEntity);
                    // Get the material id
                    int physMatID = GetComponent<PhysicsMaterialComponent>(bulletID)->p_materialID;
                    // Create rigid body for the bullet
                    RigidBodyComponent* rigidComp = GetComponent<RigidBodyComponent>(bulletID);
                    rigidComp->p_bodyID = m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(bulletID, bulletPos, orientation,
                                                                                                                     XMFLOAT3(0.5, 0.5, 0.5), physMatID);
                    // Set start velocity
                    float fireVelocity = 50;
                    XMVECTOR bulletVelVec = dirVec * fireVelocity;
                    XMFLOAT3 bulletVel;
                    XMStoreFloat3(&bulletVel, bulletVelVec);
                    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(rigidComp->p_bodyID, bulletVel);
                }
            }
        }

        void PlayerHandler::UpdatePlayerRotationsServer()
        {

        }
    }
}