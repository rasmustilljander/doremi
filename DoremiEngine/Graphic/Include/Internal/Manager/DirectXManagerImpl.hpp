#pragma once
#include <Interface/Manager/DirectXManager.hpp>
#include <Internal/Mesh/MeshRenderData.hpp>

#include <vector>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct GraphicModuleContext;
        // class DepthStencilState;
        // class RasterizerState;
        class DirectXManagerImpl : public DirectXManager
        {
            public:
            DirectXManagerImpl(const GraphicModuleContext& p_graphicContext);
            virtual ~DirectXManagerImpl();
            /**
            Initializes important direct x thingies like device and device context.
            Until later it also contains backbuffer and som other stuff
            */
            void InitializeDirectX();
            ID3D11Device* GetDevice() override;
            void SetScreenResolution(DirectX::XMFLOAT2) override;
            DirectX::XMFLOAT2 GetScreenResolution() override;
            ID3D11DeviceContext* GetDeviceContext() override;
            // TODOKO should end draw be here?
            void EndDraw() override;

            /**
                Adds a mesh for later rendering
            */
            void AddMeshForRendering(MeshRenderData& p_renderData);

            DepthStencilState* CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC p_depthStencilDesc) override;
            RasterizerState* CreateRasterizerState(D3D11_RASTERIZER_DESC p_rasterizerDesc) override;
            ID3D11SamplerState* CreateSamplerState(D3D11_SAMPLER_DESC p_samplerDesc) override;
            void SwapRasterizerState(RasterizerState* p_rasterizerState) override;
            void SwapDepthStencilState(DepthStencilState* p_depthStencilState) override;
            ID3D11SamplerState* GetDefaultSamplerState() override { return m_defaultSamplerState; }
            void DrawCurrentRenderList(ID3D11RasterizerState* p_rasterizerState, ID3D11DepthStencilState* p_depthStencilState) override;

            private:
            void RenderAllMeshs();
            void BuildWorldMatrix();

            std::vector<MeshRenderData> renderData;
            const GraphicModuleContext& m_graphicContext;
            ID3D11Device* m_device;
            ID3D11DeviceContext* m_deviceContext;
            IDXGISwapChain* m_swapChain;
            ID3D11RenderTargetView* m_backBuffer;
            ID3D11Texture2D* m_depthBuffer;
            ID3D11DepthStencilView* m_depthView;
            ID3D11Buffer* m_worldMatrix;
<<<<<<< HEAD
            ID3D11SamplerState* m_defaultSamplerState;

            int i = 0;
=======
            DirectX::XMFLOAT2 m_screenResolution;
>>>>>>> Light culling compute shader almost done
        };
    }
}