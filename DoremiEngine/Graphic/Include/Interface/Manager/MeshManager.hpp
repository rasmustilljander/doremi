#pragma once
#include <string>
#include <vector>

namespace DirectX
{
    struct XMFLOAT4X4;
    struct XMFLOAT2;
}
namespace DoremiEngine
{
    namespace Graphic
    {
        struct Vertex;
        class MeshInfo;
        class MaterialInfo;
        class MeshManager
        {
            public:
            // Builds a mesh from the given file, the filename then becomes the mesh's id
            virtual MeshInfo* BuildMeshInfo(const std::string& p_fileName) = 0;
            virtual MeshInfo* BuildQuadMeshInfo(const std::string& p_fileName) = 0;
            // Builds a mesh from the given vector of vertices, the given string becomes the mesh's id
            virtual MeshInfo* BuildMeshInfoFromBuffer(const std::vector<Vertex>& p_buffer, const std::string& p_meshName) = 0;
            /**
            Returns a pointer to the mesh info for the given mesh name, if no mesh exists a nullptr will be returned
            */
            virtual MeshInfo* GetMeshInfo(const std::string& p_meshName) = 0;
            // TODOKO change to acctually load material and not just textures
            virtual MaterialInfo* BuildMaterialInfo(const std::string& p_fileName) = 0;
            virtual void AddToRenderList(MeshInfo& p_mesh, MaterialInfo& p_material, const DirectX::XMFLOAT4X4& p_orientationMatrix) = 0;
            virtual void Draw() = 0;
        };
    }
}