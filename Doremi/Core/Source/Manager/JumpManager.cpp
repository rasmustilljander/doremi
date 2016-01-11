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

        JumpManager::JumpManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext) {}


        JumpManager::~JumpManager() {}

        void JumpManager::Update(double p_dt)
        {
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Jump| (int)ComponentType::CharacterController;
            for (size_t i = 0; i < length; i++)
            {
                if (EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    JumpComponent* jumpComp = EntityHandler::GetInstance().GetComponentFromStorage<JumpComponent>(i);
                    if (jumpComp->active)
                    {
                        // Check if a jump in progress is finished
                        if (jumpComp->jumpTimeProgress >= jumpComp->jumpTime)
                        {
                            jumpComp->active = false;
                        }
                        
                        else
                        {
                            // Update progress with p_dt
                            jumpComp->jumpTimeProgress += p_dt;
                            // WARNING! Problem when we go "beneath" the landing surface?
                            // Calculate how far we've come in our jump
                            float jumpFactor = jumpComp->jumpTimeProgress / jumpComp->jumpTime;
                            // We want a value from 1 to -1
                            jumpFactor -= 0.5;
                            jumpFactor *= -2;
                            float moveDistance = jumpComp->jumpIntensity * jumpFactor;
                            m_sharedContext.GetPhysicsModule().GetCharacterControlManager().MoveController(i, XMFLOAT3(0, moveDistance, 0),p_dt);
                        }
                    }
                }
            }
        }

        void JumpManager::OnEvent(Event* p_event)
        {

        }
    }
}