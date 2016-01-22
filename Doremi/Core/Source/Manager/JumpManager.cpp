// Project specific
#include <Manager/JumpManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/JumpComponent.hpp>
#include <EntityComponent/Components/MovementComponent.hpp>

// Engine
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>

// 3rd party
#include <DirectXMath.h>

using namespace DirectX;
namespace Doremi
{
    namespace Core
    {

        JumpManager::JumpManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "JumpManager") {}

        JumpManager::~JumpManager() {}

        void JumpManager::Update(double p_dt)
        {
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Jump | (int)ComponentType::CharacterController | (int)ComponentType::Movement;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    JumpComponent* jumpComp = EntityHandler::GetInstance().GetComponentFromStorage<JumpComponent>(i);
                    if(jumpComp->active)
                    {
                        jumpComp->movementRemaining -= p_dt;
                        if(jumpComp->movementRemaining <= 0)
                        {
                            jumpComp->active = false;
                        }
                        // Add to movement component
                        GetComponent<MovementComponent>(i)->movement.y += jumpComp->movementRemaining;
                    }
                }
            }
        }

        void JumpManager::OnEvent(Event* p_event) {}
    }
}