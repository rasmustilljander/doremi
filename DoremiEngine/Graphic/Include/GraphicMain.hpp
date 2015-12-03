#pragma once
// Project specific


// Third party
#include <dxgi.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
//Not used?
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <Windows.h>
#include <windowsx.h>

// Standard libraries
#include <vector>
#include <string>
#include <map>
namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
    namespace Graphic
    {
        class PixelShader;
        class VertexShader;
        struct MeshInfo;
        enum class ShaderType;

        struct MatrixBufferType
        {            
            DirectX::XMMATRIX view;
            DirectX::XMMATRIX projection;
        };

        class GraphicMain
        {
        public:
            GraphicMain(const Core::SharedContext& p_sharedContext);
            ~GraphicMain();
            void SetWorkingDirectory(const std::string& p_workingDirectory);
            /**
            Loads the shader from the given location and returns an id
            */

            void BindShader(const ShaderType& p_type, const int& p_shaderID);

            void InitializeDirectX();

            //Load Functions
            int LoadShader(const ShaderType& p_type, const std::string& p_fileName);
            int LoadMesh(const std::string& p_meshFileName);
            int LoadTexture(const std::string& p_textureFileName);

            //Draw Functions
            //TODORK Implement draws
            void Draw(const int& p_meshID, const int& p_textureID); //Should take one matrix
            void DrawInstanced(const int& p_meshID); //Should take a list of matrices too
            void DrawParticles();
            void ComputeAfterEffects();
            void EndDraw();

            //TODO Place in the correct place with SDL
            void CreateGraphicWindow();
        private:
            const Core::SharedContext& m_sharedContext;

            //Direct X main stuff
            ID3D11Device* m_device;
            ID3D11DeviceContext* m_deviceContext;
            IDXGISwapChain* m_swapChain;
            ID3D11RenderTargetView* m_backBuffer;
            ID3D11Texture2D* m_depthBuffer;
            ID3D11DepthStencilView* m_depthView;
            ID3D11SamplerState* m_sampler; //TODORK put in the right place
            ID3D11Buffer* m_worldMatrix;

            std::vector<VertexShader*> m_vertexShaders;
            std::vector<PixelShader*> m_pixelShaders;
            std::vector<MeshInfo*> m_Meshes;
            std::vector<ID3D11ShaderResourceView*> m_textures;
            std::map<std::string, int> m_loadedMeshes;
            //TODOKO List with materials
        };
    }
}
