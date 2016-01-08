#pragma once
#include <Interface/Manager/DirectXManager.hpp>
#include <Internal/Mesh/MeshRenderData.hpp>

#include <vector>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct GraphicModuleContext;

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
            ID3D11DeviceContext* GetDeviceContext() override;
            // TODOKO should end draw be here?
            void EndDraw() override;

            /**
                Adds a mesh for later rendering
            */
            void AddMeshForRendering(MeshRenderData& p_renderData);

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
        };
    }
}