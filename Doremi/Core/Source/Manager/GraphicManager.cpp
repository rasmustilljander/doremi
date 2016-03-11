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
#include <DoremiEngine/Graphic/Include/Interface/Shader/PixelShader.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Shader/VertexShader.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/DepthStencilState.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/RasterizerState.hpp>
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

            D3D11_RASTERIZER_DESC rastDesc;
            ZeroMemory(&rastDesc, sizeof(rastDesc));
            rastDesc.FillMode = D3D11_FILL_SOLID;
            rastDesc.CullMode = D3D11_CULL_BACK;
            rastDesc.FrontCounterClockwise = false;
            rastDesc.DepthBias = 0;
            rastDesc.DepthBiasClamp = 0.0f;
            rastDesc.SlopeScaledDepthBias = 0.0f;
            rastDesc.DepthClipEnable = false;
            rastDesc.ScissorEnable = false;
            rastDesc.MultisampleEnable = true;
            rastDesc.AntialiasedLineEnable = false;
            m_rasterizerState = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().CreateRasterizerState(rastDesc);
            D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
            ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
            depthStencilDesc.DepthEnable = true;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
            m_depthStencilState = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().CreateDepthStencilState(depthStencilDesc);
        }

        GraphicManager::~GraphicManager() {}

        void GraphicManager::Update(double p_dt)
        {
            DoremiEngine::Graphic::SubModuleManager& submoduleManager = m_sharedContext.GetGraphicModule().GetSubModuleManager();
            submoduleManager.GetShaderManager().SetActiveVertexShader(m_vertexShader);
            submoduleManager.GetShaderManager().SetActivePixelShader(m_pixelShader);

            EntityHandler& entityHandler = EntityHandler::GetInstance();
            const size_t length = entityHandler.GetLastEntityIndex();
            int mask = (int)ComponentType::Render | (int)ComponentType::Transform;
            for(size_t i = 0; i < length; i++)
            {
                if(entityHandler.HasComponents(i, mask) && !entityHandler.HasComponents(i, (int)ComponentType::LowerBodySkeletalAnimation) &&
                   !entityHandler.HasComponents(i, (int)ComponentType::UpperBodySkeletalAnimation))
                {
                    RenderComponent* renderComp = entityHandler.GetComponentFromStorage<RenderComponent>(i);
                    TransformComponent* orientationComp = entityHandler.GetComponentFromStorage<TransformComponent>(i);
                    DirectX::XMFLOAT4X4 transMat;
                    DirectX::XMVECTOR quaternion = DirectX::XMLoadFloat4(&orientationComp->rotation);

                    DirectX::XMMATRIX tempTransMat = DirectX::XMMatrixTranspose(
                        DirectX::XMMatrixScaling(orientationComp->scale.x, orientationComp->scale.y, orientationComp->scale.z) *
                        DirectX::XMMatrixRotationQuaternion(quaternion) *
                        DirectX::XMMatrixTranslation(orientationComp->position.x, orientationComp->position.y + renderComp->offsetY, orientationComp->position.z));
                    DirectX::XMStoreFloat4x4(&transMat, tempTransMat);
                    submoduleManager.GetMeshManager().AddToRenderList(*renderComp->mesh, *renderComp->material, transMat);
                }
            }
            m_rasterizerState->GetRasterizerState();
            m_depthStencilState->GetDepthStencilState();
            submoduleManager.GetDirectXManager().DrawCurrentRenderList(m_rasterizerState->GetRasterizerState(), m_depthStencilState->GetDepthStencilState());
        }

        void GraphicManager::OnEvent(Event* p_event) {}
    }
}