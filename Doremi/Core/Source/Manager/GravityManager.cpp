// Project specific
#include <Manager/GravityManager.hpp>
#include <EntityComponent/EntityHandler.hpp>

// Components
#include <EntityComponent/Components/JumpComponent.hpp>
#include <EntityComponent/Components/GravityComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
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

        GravityManager::GravityManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "GravityManager")
        {
            m_gravityCoefficient = 1.5;
        }

        GravityManager::~GravityManager() {}

        void GravityManager::Update(double p_dt)
        {
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Gravity | (int)ComponentType::CharacterController | (int)ComponentType::Transform | (int)ComponentType::Movement;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    GravityComponent* gravComp = EntityHandler::GetInstance().GetComponentFromStorage<GravityComponent>(i);
                    // Make sure a jump isn't in progress
                    if(!(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Jump) &&
                         EntityHandler::GetInstance().GetComponentFromStorage<JumpComponent>(i)->active)) // temporary fix
                    {
                        gravComp->travelSpeed += m_gravityCoefficient * p_dt;
                        if(gravComp->travelSpeed >= gravComp->maxFallSpeed)
                        {
                            gravComp->travelSpeed = gravComp->maxFallSpeed;
                        }
                    }
                    else
                    {
                        // Jump in progress, hence gravity isn't needed
                        gravComp->travelSpeed = 0;
                    }
                    
                    // Add to movement component
                    GetComponent<MovementComponent>(i)->movement.y += -gravComp->travelSpeed;
                }
            }
        }

        void GravityManager::OnEvent(Event* p_event) {}
    }
}