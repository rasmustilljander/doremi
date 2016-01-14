#pragma once
#include <Interface/Manager/MeshManager.hpp>
#include <string>
#include <vector>
#include <map>
namespace DirectX
{
    struct XMFLOAT4X4;
    struct XMFLOAT2;
}
struct ID3D11Buffer;
namespace DoremiEngine
{
    namespace Graphic
    {
        struct GraphicModuleContext;
        class ModelLoader;
        class MeshManagerImpl : public MeshManager
        {
            public:
            // TODOKO add documents
            MeshManagerImpl(const GraphicModuleContext& p_graphicContext);
            virtual ~MeshManagerImpl();
            MeshInfo* BuildMeshInfo(const std::string& p_fileName) override;
            MeshInfo* BuildQuadMeshInfo(const std::string& p_fileName) override;
            MeshInfo* BuildSphereMeshInfo(const std::string& p_fileName, int p_latLines, int p_longLines) override;
            MeshInfo* BuildMeshInfoFromBuffer(const std::vector<Vertex>& p_buffer, const std::string& p_meshName) override;
            MeshInfo* GetMeshInfo(const std::string& p_meshName) override;
            MaterialInfo* BuildMaterialInfo(const std::string& p_fileName) override;
            void AddToRenderList(MeshInfo& p_mesh, MaterialInfo& p_material, const DirectX::XMFLOAT4X4& p_orientationMatrix) override;
            void Draw() override;

            private:
            const GraphicModuleContext& m_graphicContext;
            ModelLoader* m_modelLoader;
            std::map<std::string, MeshInfo*> m_meshInfo;
            std::map<std::string, MaterialInfo*> m_materialInfo;
        };
    }
}