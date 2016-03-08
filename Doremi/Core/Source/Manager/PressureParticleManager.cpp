/// Project specific
// This class
#include <Manager/PressureParticleManager.hpp>
// Handlers
#include <EntityComponent/EntityHandler.hpp>
// Components
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/PressureParticleComponent.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>

/// Engine
// Core
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/FluidManager.hpp>

// 3rd party
#include <DirectXMath.h>

#include <stdint.h>

using namespace DirectX;
namespace Doremi
{
    namespace Core
    {
        PressureParticleManager::PressureParticleManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "PreasureParticleManager")
        {
        }
        PressureParticleManager::~PressureParticleManager() {}

        void PressureParticleManager::Update(double p_dt)
        {
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Transform) &&
                   EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::PressureParticleSystem))
                {
                    // TODOJB Hard-coded since we cannot read beam width properly
                    GetComponent<ParticlePressureComponent>(i)->data.m_emissionAreaDimensions.x = 0.05f;

                    // Jaws late night fixes for better placement of particles TODO check this code
                    XMFLOAT3 pos = GetComponent<TransformComponent>(i)->position;
                    // pos.y += 0.05;
                    XMVECTOR posvec = XMLoadFloat3(&pos);

                    XMVECTOR normalDir = XMLoadFloat3(&XMFLOAT3(0, 0, 1));
                    XMVECTOR rotQuat = XMLoadFloat4(&GetComponent<TransformComponent>(i)->rotation);
                    XMVECTOR direction = XMVector3Rotate(normalDir, rotQuat);
                    XMVECTOR endPos = posvec - direction * 0.1f;
                    XMStoreFloat3(&GetComponent<ParticlePressureComponent>(i)->data.m_position, endPos);

                    GetComponent<ParticlePressureComponent>(i)->data.m_direction = GetComponent<TransformComponent>(i)->rotation;

                    m_sharedContext.GetPhysicsModule().GetFluidManager().SetParticleEmitterData(i, GetComponent<ParticlePressureComponent>(i)->data);
                }
            }
        }

        void PressureParticleManager::OnEvent(Event* p_event) {}
    }
}