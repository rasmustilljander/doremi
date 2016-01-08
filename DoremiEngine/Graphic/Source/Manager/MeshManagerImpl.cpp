#pragma once
#include <Internal/Manager/MeshManagerImpl.hpp>
#include <Internal/Mesh/MeshInfoImpl.hpp>
#include <Internal/Mesh/MaterialInfoImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <GraphicModuleImplementation.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <Loaders/ModelLoader.hpp>
#include <Loaders/DDSTextureLoader.h>

#include <DirectXMath.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

#include <string>

namespace DoremiEngine
{
    namespace Graphic
    {
        MeshManagerImpl::MeshManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext)
        {
            m_modelLoader = new ModelLoader();
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
        }

        MeshManagerImpl::~MeshManagerImpl() {}

        MeshInfo* MeshManagerImpl::BuildMeshInfo(const std::string& p_fileName)
        {
            if(m_meshInfo.find(p_fileName) != m_meshInfo.end())
            {
                return m_meshInfo[p_fileName];
            }
            MeshInfo* newMesh = new MeshInfoImpl();
            newMesh->SetFileName(p_fileName);
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            std::string filePath = m_graphicContext.m_workingDirectory + p_fileName; // TODOKO should add complete filepath
            m_modelLoader->LoadMesh(newMesh, filePath, m_directX.GetDeviceContext(), m_directX.GetDevice());
            m_meshInfo[p_fileName] = newMesh;
            return newMesh;
        }

        MaterialInfo* MeshManagerImpl::BuildMaterialInfo(const std::string& p_fileName)
        {
            if(m_materialInfo.find(p_fileName) != m_materialInfo.end())
            {
                return m_materialInfo[p_fileName];
            }
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            MaterialInfo* newMaterial = new MaterialInfoImpl();
            std::string fileLocation = m_graphicContext.m_workingDirectory + "Textures/" + p_fileName;
            ModelLoader t_loader = ModelLoader();
            ID3D11ShaderResourceView* newTexture = t_loader.LoadTexture(fileLocation, m_directX.GetDevice());
            newMaterial->SetMaterialName(p_fileName);
            newMaterial->SetTexture(newTexture);
            m_materialInfo[p_fileName] = newMaterial;
            return newMaterial;
        }

        void MeshManagerImpl::AddToRenderList(MeshInfo& p_mesh, MaterialInfo& p_material, const DirectX::XMFLOAT4X4& p_orientationMatrix)
        {
            // TODORT Could be redesigned so the DirectXManager asks this class for this information instead.
            MeshRenderData meshRenderData(p_orientationMatrix, p_material.GetTexture(), p_mesh.GetBufferHandle(), p_mesh.GetVerticeCount());
            m_graphicContext.m_graphicModule->GetSubModuleManagerImpl().GetDirectXManagerImpl().AddMeshForRendering(meshRenderData);
        }
        void MeshManagerImpl::Draw() {}
    }
}