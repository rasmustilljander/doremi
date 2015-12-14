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
            //TransformComponent* t_playerStartTransform = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(p_playerEntityID);
            ///// Create rigid body (do this once when creating the entity/component
            //// 1) create a material
            //m_materialID = m_sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0.5, 0.5, 0.5);
            //// 2) create the body

            //m_bodyID = m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(t_playerStartTransform->position, t_playerStartTransform->rotation,
            //                                                                                      XMFLOAT3(0.5, 0.5, 0.5), m_materialID);
        }
        PlayerHandler* PlayerHandler::m_singleton = nullptr;
        PlayerHandler* PlayerHandler::GetInstance() { return m_singleton; }

        XMFLOAT3 PlayerHandler::UpdatePosition()
        {
            bool moving = false;
            int bodyID = EntityHandler::GetInstance().GetComponentFromStorage<RigidBodyComponent>(m_playerEntityID)->p_bodyID;
            if(m_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::Forward))
            {
                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(bodyID, XMFLOAT3(0, 0, 1));
                moving = true;
            }
            else
            {
                // Nothing
            }
            if(m_inputHandler->CheckBitMaskInputFromGame(4))
            {
                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(bodyID, XMFLOAT3(0, 0, -1));
                moving = true;
            }
            else
            {
                // Nothing
            }
            if(m_inputHandler->CheckBitMaskInputFromGame(8))
            {
                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(bodyID, XMFLOAT3(-1, 0, 0));
                moving = true;
            }
            else
            {
                // Nothing
            }
            if(m_inputHandler->CheckBitMaskInputFromGame(16))
            {
                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(bodyID, XMFLOAT3(1, 0, 0));
                moving = true;
            }
            else
            {
                // Nothing
            }

            if (!moving)
            {
                m_sharedContext.GetPhysicsModule().GetRigidBodyManager().SetBodyVelocity(bodyID, XMFLOAT3(0, 0, 0));
            }
            
            //XMFLOAT3 position = m_sharedContext.GetPhysicsModule().GetRigidBodyManager().GetBodyPosition(m_bodyID);
            return XMFLOAT3();
        }
    }
}
