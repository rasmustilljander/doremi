/// Project specific
// This class
#include <Manager/GroundEffectManagerClient.hpp>
// Handles
#include <EntityComponent/EntityHandler.hpp>
// Components
#include <EntityComponent\Components\PressureParticleComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>

/// Engine
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/FluidManager.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
// Graphics
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>

using namespace DirectX;
namespace Doremi
{
    namespace Core
    {

        GroundEffectManagerClient::GroundEffectManagerClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "GroundEffectManagerClient")
        {
        }

        GroundEffectManagerClient::~GroundEffectManagerClient() {}

        void GroundEffectManagerClient::Update(double p_dt)
        {
            // TODOXX This only works if there's ONE particle system
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

                    /// Draw the fields
                    // Get particle component TODOJB Shouldn't draw it as the same mesh and material as the system itself...
                    ParticlePressureComponent* particleComp = entityHandler.GetComponentFromStorage<ParticlePressureComponent>(i);
                    // Iterate through all positions
                    size_t size = m_groundEffectPoints.size();
                    if(size > 0)
                    {
                        for(size_t j = 0; j < size; j++)
                        {
                            XMFLOAT4X4 transMat;
                            // DX oneliner: create a translation matrix, then transpose it, then store it
                            XMStoreFloat4x4(&transMat, XMMatrixTranspose(XMMatrixTranslation(m_groundEffectPoints[j].x, m_groundEffectPoints[j].y,
                                                                                             m_groundEffectPoints[j].z)));
                            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(*particleComp->mesh,
                                                                                                                      *particleComp->material, transMat);
                        }
                    }
                }
            }
        }

        void GroundEffectManagerClient::OnEvent(Event* p_event) {}
    }
}