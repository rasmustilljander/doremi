#pragma once
// This class
#include <Manager/ExtraDrainSyncManager.hpp>

/// Project specific
// Engine
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>

// Handlers
#include <EntityComponent/EntityHandler.hpp>
// Components
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>

// 3rd party
#include <DirectXMath.h>

using namespace DirectX;
namespace Doremi
{
    namespace Core
    {
        ExtraDrainSyncManager::ExtraDrainSyncManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "ExtraDrainSyncManager")
        {
        }

        ExtraDrainSyncManager::~ExtraDrainSyncManager() {}

        void ExtraDrainSyncManager::Update(double p_dt)
        {
            // Update simulation
            m_sharedContext.GetPhysicsModule().Update(p_dt);
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::ExtraDrain | (int)ComponentType::RigidBody;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    // Get relevant components
                    TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i);

                    // Get rigid body manager
                    DoremiEngine::Physics::RigidBodyManager& rigidManager = m_sharedContext.GetPhysicsModule().GetRigidBodyManager();

                    // Calculate a rotated orientation
                    XMFLOAT4 newOrientation;
                    XMVECTOR rotationModVec = XMQuaternionRotationAxis(XMLoadFloat3(&XMFLOAT3(0, 0, 1)), 3.1415 / 2);

                    XMStoreFloat4(&newOrientation, XMQuaternionMultiply(XMLoadFloat4(&transComp->rotation), rotationModVec));
                    // Set position of drain so it follows us around
                    rigidManager.SetBodyPosition(i, transComp->position, newOrientation);
                }
            }
        }

        void ExtraDrainSyncManager::OnEvent(Event* p_event) {}
    }
}