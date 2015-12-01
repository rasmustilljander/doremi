#pragma once
// Project specific


// Third party
#include <dxgi.h>
#include <d3d11_1.h>
//Not used?
#include <d3dcommon.h>
#include <d3dcompiler.h>

// Standard libraries
#include <vector>
#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        class PixelShader;
        class VertexShader;
        class GraphicObject;
        enum class ShaderType;
        class GraphicMain
        {
        public:
            static GraphicMain* GetInstance();
            ~GraphicMain();
            /**
            Loads the shader from the given location and returns an id
            */
            int LoadShader(const ShaderType& p_type, const std::string& p_fileName);
            void BindShader(const ShaderType& p_type, const int& p_shaderID);
            int LoadObject(const std::string& p_meshFileName, const std::string& p_materialName);
            
        private:
            static GraphicMain* m_singleton;
            GraphicMain();
            void InitializeDirectX();

            //Direct X main stuff
            ID3D11Device* m_device;
            ID3D11DeviceContext* m_deviceContext;
            IDXGISwapChain* m_swapChain;
            ID3D11RenderTargetView* m_backBuffer;
            ID3D11Texture2D* m_depthBuffer;
            ID3D11DepthStencilView* m_depthView;

            std::vector<VertexShader*> m_vertexShaders;
            std::vector<PixelShader*> m_pixelShaders;
            std::vector<GraphicObject*> m_graphicObjects;      
        };
    }
}
