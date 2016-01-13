// Project specific
#include <Manager/JumpManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/JumpComponent.hpp>

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
            int mask = (int)ComponentType::Jump | (int)ComponentType::CharacterController;
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
                        m_sharedContext.GetPhysicsModule().GetCharacterControlManager().MoveController(i, XMFLOAT3(0, jumpComp->movementRemaining, 0), p_dt);
                    }
                }
            }
        }

        void JumpManager::OnEvent(Event* p_event) {}
    }
}