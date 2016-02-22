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
            // EXPERIMENTAL PHYSICS. Hard-coded ID works since I thought ahead and made it signed. Tru story
            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().CreateArbitraryBody(-15);
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
                    // Add us a shape to the arbitrary body thingy
                    for(size_t j = 0; j < newPositions.size(); j++)
                    {
                        m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddShapeToBody(-15, newPositions[j]);
                    }

                    /// Time to draw!
                    ParticlePressureComponent* particleComp = entityHandler.GetComponentFromStorage<ParticlePressureComponent>(i);
                    // Get our positions
                    vector<XMFLOAT3> positions;
                    vector<float> radii;
                    m_sharedContext.GetPhysicsModule().GetRigidBodyManager().GetShapeData(-15, positions, radii);

                    // Draw the particles
                    int size = positions.size();
                    for(size_t j = 0; j < size; j++)
                    {

                        XMFLOAT4X4 mat;
                        XMStoreFloat4x4(&mat, XMMatrixTranspose(XMMatrixScaling(radii[j], radii[j], radii[j]) *
                                                                XMMatrixTranslation(positions[j].x, positions[j].y, positions[j].z)));
                        m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(*particleComp->mesh,
                                                                                                                  *particleComp->material, mat);
                    }
                }
            }
        }

        void GroundEffectManagerClient::OnEvent(Event* p_event) {}
    }
}