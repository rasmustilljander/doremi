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
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/FluidManager.hpp>
// Render
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>

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
            int derp = 5;
            static int frame = 0;
            frame++;
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Transform || (int)ComponentType::PressureParticleSystem;

            RenderComponent* renderComp = EntityHandler::GetInstance().GetComponentFromStorage<RenderComponent>(14);
            DirectX::XMFLOAT4X4 transMat;
            DirectX::XMMATRIX tempTransMat = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(5, 5, 5));
            DirectX::XMStoreFloat4x4(&transMat, tempTransMat);
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(*renderComp->mesh, *renderComp->material, transMat);


            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Transform) &&
                   EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::PressureParticleSystem) &&
                   EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::Render))
                {
                    // Read thingies
                    vector<XMFLOAT3> returnPositions;
                    m_sharedContext.GetPhysicsModule().GetFluidManager().GetParticlePositions(i, returnPositions);

                    // Render thingies
                    RenderComponent* renderComp = EntityHandler::GetInstance().GetComponentFromStorage<RenderComponent>(i);
                    for(size_t j = 0; j < returnPositions.size(); j++)
                    {
                        DirectX::XMFLOAT4X4 transMat;
                        DirectX::XMMATRIX tempTransMat =
                            DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(returnPositions[j].x, returnPositions[j].y, returnPositions[j].z));
                        DirectX::XMStoreFloat4x4(&transMat, tempTransMat);
                        m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(*renderComp->mesh, *renderComp->material, transMat);
                    }
                }
            }
        }

        void PressureParticleManager::OnEvent(Event* p_event) {}
    }
}