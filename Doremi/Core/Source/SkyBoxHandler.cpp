// Project specific
#include <Doremi/Core/Include/SkyBoxHandler.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/ShaderManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <Doremi/Core/Include/CameraHandler.hpp>

// Third party
#include <d3d11_1.h>
// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        SkyBoxHandler* SkyBoxHandler::m_sigleton = nullptr;

        void SkyBoxHandler::StartupSkyBoxHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_sigleton != nullptr)
            {
                std::runtime_error("StartupSkyBoxHandler was called multiple times");
            }
            m_sigleton = new SkyBoxHandler(p_sharedContext);
        }

        SkyBoxHandler* SkyBoxHandler::GetInstance()
        {
            if(m_sigleton == nullptr)
            {
                std::runtime_error("GetInstance was called before StartupSkyBoxHandler");
            }
            return m_sigleton;
        }

        SkyBoxHandler::SkyBoxHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) { CreateSkyBox(); }

        SkyBoxHandler::~SkyBoxHandler() {}


        void SkyBoxHandler::Draw()
        {
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActiveVertexShader(m_skyBoxVertexShader);
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActivePixelShader(m_skyBoxPixelShader);

            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().SwapDepthStencilState(m_depthStencilState);
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().SwapRasterizerState(m_rasterizerState);


            DirectX::XMFLOAT3 t_cameraPosition = CameraHandler::GetInstance()->GetActiveCameraPosition();

            DirectX::XMFLOAT4X4 transMat;
            DirectX::XMMATRIX tempTransMat =
                DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(t_cameraPosition.x, t_cameraPosition.y, t_cameraPosition.z));
            DirectX::XMStoreFloat4x4(&transMat, tempTransMat);


            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(*m_meshInfo, *m_materialInfo, transMat);
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().Render2D(m_rasterizerState->GetRasterizerState(),
                                                                                                  m_depthStencilState->GetDepthStencilState());
        }

        void SkyBoxHandler::CreateSkyBox()
        {
            m_materialInfo = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("SkyBox.dds");
            D3D11_SAMPLER_DESC t_sampDesc;
            ZeroMemory(&t_sampDesc, sizeof(t_sampDesc));
            t_sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            t_sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            t_sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            t_sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            t_sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            t_sampDesc.MaxAnisotropy = 1;
            t_sampDesc.MinLOD = 0;
            t_sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

            m_materialInfo->SetSamplerState(m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().CreateSamplerState(t_sampDesc));
            m_meshInfo = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildSphereMeshInfo("Sphere", 20, 20);

            m_skyBoxPixelShader =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildPixelShader("SkyBoxPixelShader.hlsl");
            D3D11_INPUT_ELEMENT_DESC ied[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };
            m_skyBoxVertexShader =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildVertexShader("SkyBoxVertexShader.hlsl", ied, ARRAYSIZE(ied));


            // Create RasterizerState
            D3D11_RASTERIZER_DESC t_rasterDesc;
            ZeroMemory(&t_rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
            t_rasterDesc.FillMode = D3D11_FILL_SOLID;
            t_rasterDesc.FrontCounterClockwise = false;
            t_rasterDesc.CullMode = D3D11_CULL_NONE;
            m_rasterizerState = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().CreateRasterizerState(t_rasterDesc);

            // Create DepthStencilState
            D3D11_DEPTH_STENCIL_DESC t_depthStencilStateDesc;
            ZeroMemory(&t_depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
            t_depthStencilStateDesc.DepthEnable = true;
            t_depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            t_depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
            m_depthStencilState = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().CreateDepthStencilState(t_depthStencilStateDesc);
        }
    }
}