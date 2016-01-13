#pragma once
#include <Manager/CharacterControlSyncManager.hpp>

/// Project specific
// Engine
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>

// Handlers
#include <EntityComponent/EntityHandler.hpp>

// Componenets
#include <EntityComponent/Components/TransformComponent.hpp>

// 3rd party
#include <DirectXMath.h>

using namespace DirectX;
namespace Doremi
{
    namespace Core
    {
        CharacterControlSyncManager::CharacterControlSyncManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "CharacterControlSyncManager")
        {
        }
        CharacterControlSyncManager::~CharacterControlSyncManager() {}

        void CharacterControlSyncManager::Update(double p_dt)
        {
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::CharacterController;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    // Get relevant components
                    TransformComponent* transComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i);

                    // Get the character control manager
                    DoremiEngine::Physics::CharacterControlManager& characterControlManager = m_sharedContext.GetPhysicsModule().GetCharacterControlManager();

                    // Get position and orientation of character controller
                    XMFLOAT3 position = characterControlManager.GetPosition(i);

                    // Write position and orientation to transform component
                    transComp->position = position;
                }
            }
        }
        void CharacterControlSyncManager::OnEvent(Event* p_event) {}
    }
}