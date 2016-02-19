#pragma once
#include <VertexStruct.hpp>
#include <string>
#include <DirectXMath.h>
#include <map>

struct ID3D11DeviceContext;
struct ID3D11Device;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
namespace DoremiEngine
{
    namespace Graphic
    {
        class MeshInfo;

        class ModelLoader
        {
        public:
            static ModelLoader* GetInstance();
            // Loads the mesh specified by filename and pushes vertices to device and changes given GraphicObject
            bool LoadMesh(MeshInfo* o_meshInfo, const std::string& p_fileName, ID3D11DeviceContext* p_deviceContext,
                          ID3D11Device* p_device); // TODORK Implement load
            bool LoadQuad(MeshInfo* o_meshInfo, ID3D11DeviceContext* p_deviceContext, ID3D11Device* p_device);
            bool LoadSphere(MeshInfo* o_meshInfo, ID3D11DeviceContext* p_deviceContext, ID3D11Device* p_device, int p_latLines, int p_longLines);
            int LoadMaterial(const std::string& p_materialName);
            ID3D11ShaderResourceView* LoadTexture(const std::string& p_fileName, ID3D11Device* p_device);

        private:
            static ModelLoader* m_singleton;

            ModelLoader();
            ~ModelLoader();

            std::map<std::string, ID3D11ShaderResourceView*> m_loadedTextureMap;
        };
    }
}
