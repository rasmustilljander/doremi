// Project specific
#include <Manager/GravityManager.hpp>
#include <EntityComponent/EntityHandler.hpp>

// Components
#include <EntityComponent/Components/JumpComponent.hpp>
#include <EntityComponent/Components/GravityComponent.hpp>

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
            m_gravityCoefficient = 1;
        }


        GravityManager::~GravityManager() {}

        void GravityManager::Update(double p_dt)
        {
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Gravity | (int)ComponentType::CharacterController;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    GravityComponent* gravComp = EntityHandler::GetInstance().GetComponentFromStorage<GravityComponent>(i);
                    // Make sure a jump isn't in progress
                    if(!(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Jump) &&
                         EntityHandler::GetInstance().GetComponentFromStorage<JumpComponent>(i)->active))
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
                    m_sharedContext.GetPhysicsModule().GetCharacterControlManager().MoveController(i, XMFLOAT3(0, -gravComp->travelSpeed, 0), p_dt);
                }
            }
        }

        void GravityManager::OnEvent(Event* p_event) {}
    }
}