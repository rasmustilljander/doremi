#pragma once
// Project specific
#include <PlayerHandler.hpp>
#include <Doremi/Core/Include/InputHandler.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
//#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <iostream>
namespace Doremi
{
    namespace Core
    {
        PlayerHandler::PlayerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}
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

                if(EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::RigidBody))
                {
                    RigidBodyComponent* t_rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(m_playerEntityID);
                    t_entityVelocity = m_sharedContext.GetPhysicsModule().GetRigidBodyManager().GetBodyVelocity(t_rigidComp->p_bodyID);
                    t_entityVelocity.x = t_entityVelocity.x * m_autoRetardation;
                    t_entityVelocity.z = t_entityVelocity.z * m_autoRetardation;

                    // XMFLOAT3 t_veloctiyToAdd = XMFLOAT3(0, 0, 0);
                    bool moving = false;
                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Forward))
                    {
                        t_entityVelocity.z = t_entityVelocity.z + m_moveSpeed;
                    }
                    else
                    {
                        // Nothing
                    }
                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Backward))
                    {
                        t_entityVelocity.z = t_entityVelocity.z - m_moveSpeed;
                    }
                    else
                    {
                        // Nothing
                    }
                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Left))
                    {
                        t_entityVelocity.x = t_entityVelocity.x - m_moveSpeed;
                    }
                    else
                    {
                        // Nothing
                    }
                    if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Right))
                    {
                        t_entityVelocity.x = t_entityVelocity.x + m_moveSpeed;
                    }
                    else
                    {
                        // Nothing
                    }
                    // t_entityVelocity.y = t_entityVelocity.y + 6;
                    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID, t_entityVelocity);
                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(t_rigidComp->p_bodyID, XMFLOAT3(0, 2000, 0));
                    moving = true;

                    // if (m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Forward))
                    //{
                    //    if (EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::RigidBody))
                    //    {
                    //        // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID, XMFLOAT3(0, 0, 1));
                    //        if (abs(t_entityVelocity.x) + abs(t_entityVelocity.z) < m_maxSpeed) // TODOEA CHECK IF THIS IS NEEDED
                    //        {
                    //            //m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID, XMFLOAT3(0, 0,
                    //            m_moveSpeed));
                    //            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(t_rigidComp->p_bodyID, XMFLOAT3(0, 0,
                    //            m_moveSpeed));
                    //            moving = true;
                    //        }
                    //    }
                    //}
                    // if (m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Backward))
                    //{
                    //    if (EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::RigidBody))
                    //    {
                    //        if (abs(t_entityVelocity.x) + abs(t_entityVelocity.z) < m_maxSpeed)
                    //        {
                    //            //m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID, XMFLOAT3(0, 0,
                    //            -m_moveSpeed));
                    //            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(t_rigidComp->p_bodyID, XMFLOAT3(0, 0,
                    //            -m_moveSpeed));
                    //            moving = true;
                    //        }
                    //    }
                    //}
                    // if (m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Left))
                    //{
                    //    if (EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::RigidBody))
                    //    {
                    //        if (abs(t_entityVelocity.x) + abs(t_entityVelocity.z) < m_maxSpeed)
                    //        {
                    //            //m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID,
                    //            XMFLOAT3(-m_moveSpeed,
                    //            0,
                    //            0));
                    //            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(t_rigidComp->p_bodyID,
                    //            XMFLOAT3(-m_moveSpeed, 0,
                    //            0));
                    //            moving = true;
                    //        }
                    //    }
                    //}
                    // if (m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Right))
                    //{
                    //    if (EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::RigidBody))
                    //    {
                    //        if (abs(t_entityVelocity.x) + abs(t_entityVelocity.z) < m_maxSpeed)
                    //        {
                    //            //m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID,
                    //            XMFLOAT3(m_moveSpeed,
                    //            0,
                    //            0));
                    //            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(t_rigidComp->p_bodyID, XMFLOAT3(m_moveSpeed,
                    //            0,
                    //            0));
                    //            moving = true;
                    //        }
                    //    }
                    //}
                    if(m_inputHandler->CheckForOnePress((int)UserCommandPlaying::Jump))
                    {
                        // if (EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::RigidBody))
                        // {
                        // t_entityVelocity.y = t_entityVelocity.y + 6;
                        // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID, XMFLOAT3(0,2000,0));
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
                    int bulletID = EntityHandler::GetInstance().CreateEntity(Blueprints::BulletEntity);
                    // Get the material id
                    int physMatID = GetComponent<PhysicsMaterialComponent>(bulletID)->p_materialID;
                    // Create rigid body for the bullet
                    RigidBodyComponent* rigidComp = GetComponent<RigidBodyComponent>(bulletID);
                    rigidComp->p_bodyID =
                        m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(bulletPos, orientation, XMFLOAT3(0.5, 0.5, 0.5), physMatID);

                    // Set start velocity
                    float fireVelocity = 50;
                    XMVECTOR bulletVelVec = dirVec * fireVelocity;
                    XMFLOAT3 bulletVel;
                    XMStoreFloat3(&bulletVel, bulletVelVec);
                    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(rigidComp->p_bodyID, bulletVel);
                }
            }

            //}
        }
    }
}