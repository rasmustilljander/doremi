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
            m_inputHandler = InputHandler::GetInstance();
        }
        PlayerHandler* PlayerHandler::m_singleton = nullptr;
        PlayerHandler* PlayerHandler::GetInstance() { return m_singleton; }
        void PlayerHandler::UpdatePosition()
        {
            float movespeed = 100;
            RigidBodyComponent* t_rigidComp = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(m_playerEntityID);
            bool moving = false;
            if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Forward))
            {
                if(EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::RigidBody))
                {
                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID, XMFLOAT3(0, 0, 1));
                    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(t_rigidComp->p_bodyID, XMFLOAT3(0, 0, movespeed));
                    moving = true;
                }
            }

            if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Backward))
            {
                if(EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::RigidBody))
                {
                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID, XMFLOAT3(0, 0, -1));
                    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(t_rigidComp->p_bodyID, XMFLOAT3(0, 0, -movespeed));
                    moving = true;
                }
            }
            if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Left))
            {
                if(EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::RigidBody))
                {
                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID, XMFLOAT3(-1, 0, 0));
                    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(t_rigidComp->p_bodyID, XMFLOAT3(-movespeed, 0, 0));
                    moving = true;
                }
            }
            if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Right))
            {
                if(EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::RigidBody))
                {
                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID, XMFLOAT3(1, 0, 0));
                    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(t_rigidComp->p_bodyID, XMFLOAT3(movespeed, 0, 0));
                    moving = true;
                }
            }
            if(m_inputHandler->CheckForOnePress((int)UserCommandPlaying::Jump))
            {
                if(EntityHandler::GetInstance().HasComponents(m_playerEntityID, (int)ComponentType::RigidBody))
                {
                    XMFLOAT3 t_xmfloat3;
                    t_xmfloat3.x = m_sharedContext.GetPhysicsModule().GetRigidBodyManager().GetBodyVelocity(t_rigidComp->p_bodyID).x;
                    t_xmfloat3.y = m_sharedContext.GetPhysicsModule().GetRigidBodyManager().GetBodyVelocity(t_rigidComp->p_bodyID).y + 6;
                    t_xmfloat3.z = m_sharedContext.GetPhysicsModule().GetRigidBodyManager().GetBodyVelocity(t_rigidComp->p_bodyID).z;


                    // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID, t_xmfloat3);
                    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(t_rigidComp->p_bodyID, XMFLOAT3(0, 2000, 0));
                    moving = true;
                }
            }
            if(!moving)
            {
                // m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(t_rigidComp->p_bodyID, XMFLOAT3(0, 0, 0));
            }
        }
    }
}
