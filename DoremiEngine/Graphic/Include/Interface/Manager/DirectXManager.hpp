#pragma once
#include <string>
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
namespace DoremiEngine
{
    namespace Graphic
    {
        class DirectXManager
        {
            public:
            // TODOKO add documents
            virtual void InitializeDirectX() = 0;
            virtual ID3D11Device* GetDevice() = 0;
            virtual ID3D11DeviceContext* GetDeviceContext() = 0;
            // TODOKO should end draw be here?
            virtual void EndDraw() = 0;
        };
    }
}