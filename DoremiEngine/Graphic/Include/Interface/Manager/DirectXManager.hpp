#pragma once
#include <string>
#include <map>
#include <vector>
#include <DirectXMath.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
struct D3D11_DEPTH_STENCIL_DESC;
struct D3D11_RASTERIZER_DESC;
struct ID3D11SamplerState;
struct D3D11_SAMPLER_DESC;
struct ID3D11SamplerState;
struct ID3D11RasterizerState;
struct ID3D11DepthStencilState;

namespace DoremiEngine
{
    namespace Graphic
    {
        class DepthStencilState;
        class RasterizerState;
        class DirectXManager
        {
        public:
            // TODOKO add documents
            virtual ID3D11Device* GetDevice() = 0;
            virtual ID3D11DeviceContext* GetDeviceContext() = 0;
            virtual void SetScreenResolution(DirectX::XMFLOAT2) = 0;

            virtual DirectX::XMFLOAT2 GetScreenResolution() = 0;
            virtual uint32_t GetCurrentMonitor() = 0;
            virtual uint32_t GetNumberOfMonitors() = 0;
            virtual uint32_t GetRefreshRate() = 0;


            // TODOKO should end draw be here?
            virtual DepthStencilState* CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC p_depthStencilDesc) = 0; // Fortsätt här imorgon
            virtual ID3D11SamplerState* CreateSamplerState(D3D11_SAMPLER_DESC p_samplerDesc) = 0;
            virtual void SwapDepthStencilState(DepthStencilState* p_depthStencilState) = 0;
            virtual RasterizerState* CreateRasterizerState(D3D11_RASTERIZER_DESC p_rasterizerDesc) = 0;
            virtual void SwapRasterizerState(RasterizerState* p_rasterizerState) = 0;
            virtual void SetRenderTargetGlow() = 0;
            virtual void SetRenderTargetNormal() = 0;
            virtual void EnableBlend() = 0;
            virtual void EnableTextBlend() = 0;
            virtual void DisableBlend() = 0;
            virtual void Render2D(ID3D11RasterizerState* p_rasterizerState, ID3D11DepthStencilState* p_depthStencilState) = 0;
            virtual void RenderSprites(ID3D11RasterizerState* p_rasterizerState, ID3D11DepthStencilState* p_depthStencilState) = 0;
            virtual void BeginDraw() = 0;
            virtual void EndDraw() = 0;
            virtual ID3D11SamplerState* GetDefaultSamplerState() = 0;
            virtual RasterizerState* GetDefaultRasterizerState() = 0;
            virtual DepthStencilState* GetDefaultDepthStencilState() = 0;
            virtual void DrawCurrentRenderList(ID3D11RasterizerState* p_rasterizerState, ID3D11DepthStencilState* p_depthStencilState) = 0;
            virtual void DrawCurrentRenderListSkeletal(ID3D11RasterizerState* p_rasterizerState, ID3D11DepthStencilState* p_depthStencilState) = 0;
            /**
            Sets the fullscreen to on if true or off if false
            */
            virtual void SetFullscreen(const bool& p_fullscreen) = 0;

            virtual std::vector<std::pair<uint32_t, uint32_t>> GetResolutions(const uint32_t& p_monitor) = 0;

            virtual std::vector<uint32_t> GetRefreshRates(const uint32_t& p_monitor, const std::pair<uint32_t, uint32_t>& p_resolution) = 0;

            virtual void SetResolution(const std::pair<uint32_t, uint32_t>& p_resolution) = 0;

            virtual void SetRefreshRate(uint32_t p_refreshRate) = 0;
        };
    }
}