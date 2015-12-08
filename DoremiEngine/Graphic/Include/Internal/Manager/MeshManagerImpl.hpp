#pragma once
#include <Interface/Manager/MeshManager.hpp>
#include <string>
namespace DirectX
{
    struct XMFLOAT4X4;
}
namespace DoremiEngine
{
    namespace Graphic
    {
        class MeshManagerImpl : public MeshManager
        {
        public:
            //TODOKO add documents
            MeshManagerImpl();
            virtual ~MeshManagerImpl();
            MeshInfo* BuildMeshInfo(const std::string& p_fileName) override;
            MaterialInfo* BuildMaterialInfo(const std::string& p_fileName) override;
            void AddToRenderList(const MeshInfo& p_mesh, const MaterialInfo& p_material, DirectX::XMFLOAT4X4 p_orientationMatrix) override; //TODOKO Might work with a const xmfloat4x4& 
            void Draw() override;
            //TODOKO should end draw be here?
        };
    }
}