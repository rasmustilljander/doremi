#pragma once

#include <string>
#include <DirectXMath.h>
struct ID3D11DeviceContext;
struct ID3D11Device;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
namespace DoremiEngine
{
    namespace Graphic
    {
        class MeshInfo;
        struct Vertex
        {
            float x, y, z;
            float texcoord[2];
            float nx, ny, nz;
        };
        class ModelLoader
        {
            public:
            ModelLoader();
            ~ModelLoader();
            // Loads the mesh specified by filename and pushes vertices to device and changes given GraphicObject
            bool LoadMesh(MeshInfo* o_meshInfo, const std::string& p_fileName, ID3D11DeviceContext* p_deviceContext,
                          ID3D11Device* p_device); // TODORK Implement load
            bool LoadQuad(MeshInfo* o_meshInfo, ID3D11DeviceContext* p_deviceContext, ID3D11Device* p_device);
            int LoadMaterial(const std::string& p_materialName);
            ID3D11ShaderResourceView* LoadTexture(const std::string& p_fileName, ID3D11Device* p_device);
        };
    }
}
