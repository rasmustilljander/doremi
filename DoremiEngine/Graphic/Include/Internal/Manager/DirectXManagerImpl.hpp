#pragma once
#include <Interface/Manager/DirectXManager.hpp>
#include <Internal/Mesh/MeshRenderData.hpp>

#include <vector>

struct SDL_Window;
namespace DoremiEngine
{
    namespace Graphic
    {
        struct GraphicModuleContext;
        struct PixelShader;
        struct VertexShader;
        // class DepthStencilState;
        // class RasterizerState;

        struct WorldMatrices
        {
            DirectX::XMFLOAT4X4 worldMat;
            DirectX::XMFLOAT4X4 invTransWorldMat;
        };

        class DirectXManagerImpl : public DirectXManager
        {
        public:
            DirectXManagerImpl(const GraphicModuleContext& p_graphicContext);
            virtual ~DirectXManagerImpl();
            /**
            Get and set methods
            */
            ID3D11Device* GetDevice() override;
            DirectX::XMFLOAT2 GetScreenResolution() override;
            ID3D11DeviceContext* GetDeviceContext() override;
            ID3D11SamplerState* GetDefaultSamplerState() override { return m_defaultSamplerState; }
            RasterizerState* GetDefaultRasterizerState() override { return m_defaultRasterizerState; };
            DepthStencilState* GetDefaultDepthStencilState() override { return m_defaultDepthStencilState; };
            void SetScreenResolution(DirectX::XMFLOAT2) override;

            /**
            Set rendertarget to normal rendering
            */
            void SetRenderTargetNormal() override;
            /**
            sets two render targets for rendering glow maps as well
            */
            void SetRenderTargetGlow() override;
            /**
            Enables alpha blending. Call before rendering transparent textures/meshes
            */
            void EnableBlend() override;
            /**
            Disables alpha blending. Call after rendering transparent textures/meshes
            */
            void DisableBlend() override;
            /**
            Render screenspace objects
            */
            void Render2D(ID3D11RasterizerState* p_rasterizerState, ID3D11DepthStencilState* p_depthStencilState) override;
            /**
            Computes glow
            */
            void ComputeGlow();
            /**
            Necessary stuff before anythig gets rendered
            */
            void BeginDraw() override;
            // TODOKO should end draw be here?
            /**
            Necessary dirX stuff after everything has been rendered 
            */
            void EndDraw() override;

            /**
            Initializes important direct x thingies like device and device context.
            Until later it also contains backbuffer and som other stuff
            */
            void InitializeDirectX();
            /**
                Adds a mesh for later rendering
            */
            void AddMeshForRendering(MeshRenderData& p_renderData);
            /**
            Adds a transparent mesh for rendering after everything else
            */
            void AddTransMeshForRendering(MeshRenderData& p_renderData);
            /**
            Creates depth stencil state
            */
            DepthStencilState* CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC p_depthStencilDesc) override;
            /**
            Creates rasterizer state
            */
            RasterizerState* CreateRasterizerState(D3D11_RASTERIZER_DESC p_rasterizerDesc) override;
            /**
            Creates sampler state
            */
            ID3D11SamplerState* CreateSamplerState(D3D11_SAMPLER_DESC p_samplerDesc) override;
            /**
            Swaps between rasterizer states
            */
            void SwapRasterizerState(RasterizerState* p_rasterizerState) override;
            /**
            Swaps between depth stencil states
            */
            void SwapDepthStencilState(DepthStencilState* p_depthStencilState) override;
            /**
            Draws current render list
            */
            void DrawCurrentRenderList(ID3D11RasterizerState* p_rasterizerState, ID3D11DepthStencilState* p_depthStencilState) override;
            /**
            Draws current render list with skeletal animation
            */
            void DrawCurrentRenderListSkeletal(ID3D11RasterizerState* p_rasterizerState, ID3D11DepthStencilState* p_depthStencilState) override;

            void SetFullscreen(const bool& p_fullscreen) override;

        private:
            /**
            Render all meshes
            */
            void RenderAllMeshs();
            /**
            Render all meshes
            */
            void RenderTransMeshs();
            /**
            Dispatch compute shaders for light culling
            */
            void DispatchCompute();
            /**
            Render all meshes with skeletal animation
            */
            void RenderSkeletalMesh();
            /**
            Updates world matrix
            */
            void BuildConstantBuffers();

            std::vector<MeshRenderData> renderData;
            std::vector<MeshRenderData> transRenderData;
            const GraphicModuleContext& m_graphicContext;
            ID3D11Device* m_device;
            ID3D11DeviceContext* m_deviceContext;
            IDXGISwapChain* m_swapChain;
            ID3D11RenderTargetView* m_backBufferRTV;
            ID3D11RenderTargetView* m_sceneRTV;
            ID3D11RenderTargetView* m_glowRTV;
            ID3D11RenderTargetView* m_depthRTV;
            ID3D11ShaderResourceView* m_srv;
            ID3D11ShaderResourceView* m_backbufferSRV;
            ID3D11ShaderResourceView* m_sceneSRV;
            ID3D11ShaderResourceView* m_glowSRV;
            ID3D11ShaderResourceView* m_depthSRV;
            ID3D11UnorderedAccessView* m_backbufferUAV;
            ID3D11UnorderedAccessView* m_sceneUAV;
            ID3D11UnorderedAccessView* m_glowUAV;
            ID3D11Texture2D* m_depthBuffer;
            ID3D11Texture2D* m_glowmap;
            ID3D11Texture2D* m_scene;
            ID3D11Texture2D* m_depth;
            ID3D11Texture2D* m_renderTargetTexture[2];
            ID3D11DepthStencilView* m_depthView;
            ID3D11Buffer* m_worldMatrix;
            ID3D11Buffer* m_materialBuffer;
            ID3D11SamplerState* m_defaultSamplerState;
            ID3D11BlendState* m_enableBlendState;
            ID3D11BlendState* m_disableBlendState;
            RasterizerState* m_defaultRasterizerState;
            DepthStencilState* m_defaultDepthStencilState;
            DirectX::XMFLOAT2 m_screenResolution;
            SDL_Window* m_window = nullptr;
        };
    }
}