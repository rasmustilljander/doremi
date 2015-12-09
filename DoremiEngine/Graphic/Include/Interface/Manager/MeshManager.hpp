#pragma once
#include <string>
namespace DirectX
{
    struct XMFLOAT4X4;
}
namespace DoremiEngine
{
    namespace Graphic
    {
        class MeshInfo;
        class MaterialInfo;
        class MeshManager
        {
        public:
            //TODOKO add documents
            virtual MeshInfo* BuildMeshInfo(const std::string& p_fileName)           = 0;
            virtual MaterialInfo* BuildMaterialInfo(const std::string& p_fileName)   = 0;
            virtual void AddToRenderList(MeshInfo& p_mesh, MaterialInfo& p_material, DirectX::XMFLOAT4X4 p_orientationMatrix) = 0; //TODOKO Might work with a const xmfloat4x4& 
            virtual void Draw() = 0;
            //TODOKO should end draw be here?
        };
    }
}