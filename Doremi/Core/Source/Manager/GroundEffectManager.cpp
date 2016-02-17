/// Project specific
// This class
#include <Manager/GroundEffectManager.hpp>
// Handles
#include <EntityComponent/EntityHandler.hpp>
// Components
#include <EntityComponent\Components\PressureParticleComponent.hpp>

/// Engine
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/FluidManager.hpp>


// debug
#include <iostream>
using namespace DirectX;
namespace Doremi
{
    namespace Core
    {

        GroundEffectManager::GroundEffectManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "GroundEffectManager")
        {
        }

        GroundEffectManager::~GroundEffectManager() {}

        void GroundEffectManager::Update(double p_dt)
        {
            EntityHandler& entityHandler = EntityHandler::GetInstance();
            const size_t length = entityHandler.GetLastEntityIndex();
            int mask = (int)ComponentType::PressureParticleSystem;
            for(size_t i = 0; i < length; i++)
            {
                // Check if we have a pressure particle system. TODOXX This will be really funny if we have just ambient particle systems
                if(entityHandler.HasComponents(i, mask))
                {
                    // Merge new positions into already existing positions
                    const vector<XMFLOAT3>& newPositions = m_sharedContext.GetPhysicsModule().GetFluidManager().GetRemovedParticlesPositions(i);
                    m_groundEffectPoints.reserve(m_groundEffectPoints.size() + newPositions.size());
                    m_groundEffectPoints.insert(m_groundEffectPoints.end(), newPositions.begin(), newPositions.end());
                    cout << m_groundEffectPoints.size();
                }
            }
        }

        void GroundEffectManager::OnEvent(Event* p_event) {}
    }
}