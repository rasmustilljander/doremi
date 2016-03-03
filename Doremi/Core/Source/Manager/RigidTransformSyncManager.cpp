#pragma once
#include <Manager/RigidTransformSyncManager.hpp>

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
        RigidTransformSyncManager::RigidTransformSyncManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "RigidTransformSyncManager")
        {
        }

        RigidTransformSyncManager::~RigidTransformSyncManager() {}

        void RigidTransformSyncManager::Update(double p_dt)
        {
            // Update simulation
            m_sharedContext.GetPhysicsModule().Update(p_dt);
            int mask = (int)ComponentType::RigidBody | (int)ComponentType::Transform;

            // Prefetch the rigid body manager
            DoremiEngine::Physics::RigidBodyManager& rigidManager = m_sharedContext.GetPhysicsModule().GetRigidBodyManager();

            // Prefetch the entityhandler
            EntityHandler& entityHandler = EntityHandler::GetInstance();


            const size_t length = entityHandler.GetLastEntityIndex();
            for(size_t i = 0; i < length; ++i)
            {
                if(entityHandler.HasComponents(i, mask))
                {
                    // Get relevant components
                    TransformComponent* const transComp = entityHandler.GetComponentFromStorage<TransformComponent>(i);

                    // Get position and orientation of rigid body
                    const XMFLOAT3 position = rigidManager.GetBodyPosition(i);
                    const XMFLOAT4 orientation = rigidManager.GetBodyOrientation(i);

                    // Write position and orientation to transform component
                    transComp->position = position;
                    transComp->rotation = orientation;
                }
            }
        }

        void RigidTransformSyncManager::OnEvent(Event* p_event) {}
    }
}