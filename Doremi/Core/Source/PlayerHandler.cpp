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
#include <iostream>
namespace Doremi
{
    namespace Core
    {
        PlayerHandler::PlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext), m_moveSpeed(0.2), m_autoRetardation(50), m_turnSpeed(0.01)
        {
        }
        PlayerHandler::~PlayerHandler() {}
        void PlayerHandler::StartPlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            m_singleton = new PlayerHandler(p_sharedContext);
        }
        void PlayerHandler::Initialize(int p_playerEntityID)
        {
            m_playerEntityID = p_playerEntityID;
            // m_inputHandler = InputHandler::GetInstance();
        }
        PlayerHandler* PlayerHandler::m_singleton = nullptr;
        PlayerHandler* PlayerHandler::GetInstance() { return m_singleton; }
        void PlayerHandler::UpdatePosition()
        {
            int t_checkIfWeHavePlayer = EntityHandler::GetInstance().GetLastEntityIndex();
            XMFLOAT3 t_entityVelocity = XMFLOAT3(0, 0, 0);
            m_inputHandler = InputHandler::GetInstance();
            if(m_playerEntityID >= 0 && m_playerEntityID <= t_checkIfWeHavePlayer && m_inputHandler != nullptr)
            {
                if(EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::CharacterController | (int)ComponentType::Transform))
                {
                    // Get transform component
                    TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(m_playerEntityID);
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
                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Forward))
                    {
                        movementVec += dirVec * m_moveSpeed;
                    }

                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Backward))
                    {
                        movementVec -= dirVec * m_moveSpeed;
                    }
                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Left))
                    {
                        movementVec -= rightVec * m_moveSpeed;
                    }
                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Right))
                    {
                        movementVec += rightVec * m_moveSpeed;
                    }

                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Jump))
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
                        m_sharedContext.GetPhysicsModule().GetCharacterControlManager().MoveController(m_playerEntityID, movement,
                                                                                                       0.017); // TODOJB no hardcoded dt...
                    }
                    /// Handle mouse input
                    // Get mouse input
                    int t_mouseMovementX = m_inputHandler->GetMouseMovementX();
                    if(t_mouseMovementX)
                    {
                        // Get current angle
                        float angle;
                        XMVECTOR oldDir; // This really is only needed for the parameter below...
                        XMQuaternionToAxisAngle(&oldDir, &angle, orientationVec);
                        // Change the angle
                        angle += t_mouseMovementX * m_turnSpeed;
                        // Single quaternions don't really like angles over 2*pi, we do this
                        if(angle > 2 * 3.1415)
                        {
                            angle -= 2 * 3.1415;
                        }
                        else if(angle < 0)
                        {
                            angle += 2 * 3.1415;
                        }
                        // Create a new quaternion with the new angle
                        orientationVec = XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(0, 1, 0)), angle);
                        // Store results
                        XMStoreFloat4(&transComp->rotation, orientationVec);
                    }
                }

                //////////////////////////////////////////////////////////////////////
                // WILL NOT BE CALLED SINCE RIGID BODY COMP IS REMOVED FROM PLAYER!!//
                //////////////////////////////////////////////////////////////////////
                if(EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::RigidBody))
                {
                    RigidBodyComponent* t_rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(m_playerEntityID);
                    t_entityVelocity = m_sharedContext.GetPhysicsModule().GetRigidBodyManager().GetBodyVelocity(t_rigidComp->p_bodyID);
                    t_entityVelocity.x = t_entityVelocity.x * m_autoRetardation;
                    t_entityVelocity.z = t_entityVelocity.z * m_autoRetardation;

                    MovementComponent* playerMovement = EntityHandler::GetInstance().GetComponentFromStorage<MovementComponent>(m_playerEntityID);
                    playerMovement->forwardAcceleration = 0;
                    playerMovement->rightAcceleration = 0;
                    bool moving = false;
                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Forward))
                    {
                        playerMovement->forwardAcceleration += m_moveSpeed;
                        moving = true;
                    }

                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Backward))
                    {
                        playerMovement->forwardAcceleration += -m_moveSpeed;
                        moving = true;
                    }
                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Left))
                    {
                        playerMovement->rightAcceleration += -m_moveSpeed;
                        moving = true;
                    }
                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Right))
                    {
                        playerMovement->rightAcceleration += m_moveSpeed;
                        moving = true;
                    }

                    if(m_inputHandler->CheckForOnePress((int)UserCommandPlaying::Jump))
                    {
                        m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(t_rigidComp->p_bodyID, XMFLOAT3(0, 2000, 0));
                        moving = true;
                    }
                    int t_mouseMovementX = m_inputHandler->GetMouseMovementX();
                    XMFLOAT3 t_torqueParameter;
                    if(t_mouseMovementX)
                    {
                        t_torqueParameter = XMFLOAT3(0, m_inputHandler->GetMouseMovementX() * 400, 0);
                        m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddTorqueToBody(t_rigidComp->p_bodyID, t_torqueParameter);
                    }
                    else
                    {
                        t_torqueParameter = XMFLOAT3(0, 0, 0);
                        m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyAngularVelocity(t_rigidComp->p_bodyID, t_torqueParameter);
                    }
                }

                // Fire weapon TODOJB move this someplace that makes sense. Also fix input. Scroll wheel is silly...
                if(m_inputHandler->CheckForOnePress((int)UserCommandPlaying::ScrollWpnUp))
                {
                    /// Calculate where we want the shot to appear
                    // Get position and orientation of player
                    XMFLOAT4 orientation = GetComponent<TransformComponent>(m_playerEntityID)->rotation;
                    XMFLOAT3 playerPos = GetComponent<TransformComponent>(m_playerEntityID)->position;
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
                    int bulletID = EntityHandler::GetInstance().CreateEntity(Blueprints::EnemyEntity);
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
    }
}