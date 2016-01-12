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
#include <DoremiEngine/Graphic/Include/Interface/Manager/ShaderManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <dxgi.h>
#include <d3d11_1.h>

namespace Doremi
{
    namespace Core
    {
        GraphicManager::GraphicManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "GraphicManager")
        {
            // TODOKO Should not be here!! or should it? For standard shaders? Maybee in shadermanager
            // TODOLH Maybe shouldnt be here either. Moved it from shadermodulemanagerImplementation cos this guy needs to be able to switch shader
            // before drawing
            D3D11_INPUT_ELEMENT_DESC ied[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };
            m_vertexShader = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildVertexShader("BasicVertexShader.hlsl",
                                                                                                                           ied, ARRAYSIZE(ied));
            m_pixelShader = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildPixelShader("BasicPixelShader.hlsl");
        }

        GraphicManager::~GraphicManager() {}

        void GraphicManager::Update(double p_dt)
        {
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActiveVertexShader(m_vertexShader);
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActivePixelShader(m_pixelShader);

            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            int mask = (int)ComponentType::Render | (int)ComponentType::Transform;
            for(size_t i = 0; i < length; i++)
            {
                if(EntityHandler::GetInstance().HasComponents(i, mask))
                {
                    RenderComponent* renderComp = EntityHandler::GetInstance().GetComponentFromStorage<RenderComponent>(i);
                    TransformComponent* orientationComp = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(i);
                    DirectX::XMFLOAT4X4 transMat;
                    DirectX::XMVECTOR quaternion = DirectX::XMLoadFloat4(&orientationComp->rotation);
                    DirectX::XMMATRIX tempTransMat = DirectX::XMMatrixTranspose(
                        DirectX::XMMatrixScaling(orientationComp->scale.x, orientationComp->scale.y, orientationComp->scale.z) *
                        DirectX::XMMatrixRotationQuaternion(quaternion) *
                        DirectX::XMMatrixTranslation(orientationComp->position.x, orientationComp->position.y, orientationComp->position.z));
                    DirectX::XMStoreFloat4x4(&transMat, tempTransMat);
                    m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(*renderComp->mesh, *renderComp->material, transMat);
                }
            }
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().EndDraw();
        }

        void GraphicManager::OnEvent(Event* p_event) {}
    }
}