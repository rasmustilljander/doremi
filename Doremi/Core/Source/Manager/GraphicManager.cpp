// Project specific
#include <Manager/GraphicManager.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/RenderComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
// Engine
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>

namespace Doremi
{
    namespace Core
    {

        GraphicManager::GraphicManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext) {}


        GraphicManager::~GraphicManager() {}

        void GraphicManager::Update(double p_dt)
        {
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Render | (int)ComponentType::Transform;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    RenderComponent* renderComp = EntityHandler::GetInstance().GetComponentFromStorage<RenderComponent>(i);
                    TransformComponent* orientationComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i);
                    DirectX::XMFLOAT4X4 transMat;
                    DirectX::XMMATRIX tempTransMat = DirectX::XMMatrixTranspose(
                        DirectX::XMMatrixTranslation(orientationComp->position.x, orientationComp->position.y, orientationComp->position.z) *
                        DirectX::XMMatrixScaling(orientationComp->scale.x, orientationComp->scale.y, orientationComp->scale.z));
                    DirectX::XMStoreFloat4x4(&transMat, tempTransMat);
                    m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(*renderComp->mesh, *renderComp->material, transMat);
                }
            }
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().EndDraw();
        }

        void GraphicManager::OnEvent(Event* p_event) {}
    }
}