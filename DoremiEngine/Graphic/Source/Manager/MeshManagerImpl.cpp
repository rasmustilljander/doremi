#pragma once
#include <Internal/Manager/MeshManagerImpl.hpp>
#include <Internal/Mesh/MeshInfoImpl.hpp>
#include <Internal/Texture/SpriteInfoImpl.hpp>
#include <Internal/Mesh/MaterialInfoImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <GraphicModuleImplementation.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>

#include <Loaders/ModelLoader.hpp>
#include <Loaders/DDSTextureLoader.h>
#include <HelpFunctions.hpp>

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
            m_modelLoader = ModelLoader::GetInstance();
        }

        MeshManagerImpl::~MeshManagerImpl() {}

        MeshInfo* MeshManagerImpl::BuildSphereMeshInfo(const std::string& p_fileName, int p_latLines, int p_longLines)
        {
            if(m_meshInfo.find(p_fileName) != m_meshInfo.end())
            {
                return m_meshInfo[p_fileName];
            }
            MeshInfo* newMesh = new MeshInfoImpl();
            newMesh->SetFileName(p_fileName);
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            m_modelLoader->LoadSphere(newMesh, m_directX.GetDeviceContext(), m_directX.GetDevice(), p_latLines, p_longLines);
            m_meshInfo[p_fileName] = newMesh;
            return newMesh;
        }

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

        MeshInfo* MeshManagerImpl::BuildQuadMeshInfo(const std::string& p_fileName)
        {
            if(m_meshInfo.find(p_fileName) != m_meshInfo.end())
            {
                return m_meshInfo[p_fileName];
            }
            MeshInfo* newMesh = new MeshInfoImpl();
            newMesh->SetFileName(p_fileName);
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            m_modelLoader->LoadQuad(newMesh, m_directX.GetDeviceContext(), m_directX.GetDevice());
            m_meshInfo[p_fileName] = newMesh;
            return newMesh;
        }
        MeshInfo* MeshManagerImpl::BuildSkeletalMeshInfoFromBuffer(const std::vector<SkeletalVertex>& p_buffer, const std::string& p_meshName)
        {
            if(m_meshInfo.find(p_meshName) != m_meshInfo.end())
            {
                return m_meshInfo[p_meshName];
            }
            MeshInfo* newMesh = new MeshInfoImpl();
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();

            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = p_buffer.size() * sizeof(SkeletalVertex);
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            ID3D11Buffer* buffer;
            HRESULT res = m_directX.GetDevice()->CreateBuffer(&bd, NULL, &buffer);
            if(!CheckHRESULT(res, "Error when creating mesh buffer"))
            {
                // TODOKO log error
                return nullptr;
            }
            D3D11_MAPPED_SUBRESOURCE tMS;
            m_directX.GetDeviceContext()->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, p_buffer.data(), bd.ByteWidth);
            m_directX.GetDeviceContext()->Unmap(buffer, NULL);
            newMesh->SetBufferHandle(buffer);
            newMesh->SetFileName(p_meshName);
            newMesh->SetVerticeCount(p_buffer.size());
            m_meshInfo[p_meshName] = newMesh;
            return newMesh;
        }

        MeshInfo* MeshManagerImpl::BuildMeshInfoFromBuffer(const std::vector<Vertex>& p_buffer, const std::string& p_meshName)
        {
            if(m_meshInfo.find(p_meshName) != m_meshInfo.end())
            {
                return m_meshInfo[p_meshName];
            }
            MeshInfo* newMesh = new MeshInfoImpl();
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();

            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth = p_buffer.size() * sizeof(Vertex);
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            ID3D11Buffer* buffer;
            HRESULT res = m_directX.GetDevice()->CreateBuffer(&bd, NULL, &buffer);
            if(!CheckHRESULT(res, "Error when creating mesh buffer"))
            {
                // TODOKO log error
                return nullptr;
            }
            D3D11_MAPPED_SUBRESOURCE tMS;
            m_directX.GetDeviceContext()->Map(buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, p_buffer.data(), bd.ByteWidth);
            m_directX.GetDeviceContext()->Unmap(buffer, NULL);
            newMesh->SetBufferHandle(buffer);
            newMesh->SetFileName(p_meshName);
            newMesh->SetVerticeCount(p_buffer.size());
            m_meshInfo[p_meshName] = newMesh;
            return newMesh;
        }

        MeshInfo* MeshManagerImpl::GetMeshInfo(const std::string& p_meshName)
        {
            if(m_meshInfo.find(p_meshName) != m_meshInfo.end())
            {
                return m_meshInfo[p_meshName];
            }
            else
            {
                return nullptr;
            }
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
            ModelLoader& t_loader = *ModelLoader::GetInstance();
            ID3D11ShaderResourceView* newTexture = t_loader.LoadTexture(fileLocation, m_directX.GetDevice());
            newMaterial->SetMaterialName(p_fileName);
            newMaterial->SetDiffuseTexture(newTexture);
            newMaterial->SetGlowTexture(nullptr);
            // newMaterial->SetMaterialData(nullptr);
            newMaterial->SetSamplerState(m_directX.GetDefaultSamplerState());
            m_materialInfo[p_fileName] = newMaterial;
            return newMaterial;
        }

        MaterialInfo* MeshManagerImpl::BuildMaterialInfo(DoremiEditor::Core::MaterialMessage p_materialData)
        {
            // Read data from material
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            MaterialInfo* newMaterial = new MaterialInfoImpl();
            std::string diffuseFileLocation = m_graphicContext.m_workingDirectory + "Textures/" + p_materialData.diffuseTexturePath;
            std::string glowFileLocation = m_graphicContext.m_workingDirectory + "Textures/" + p_materialData.glowTexturePath;
            ModelLoader& t_loader = *ModelLoader::GetInstance();
            // Create resource to be used by shader
            ID3D11ShaderResourceView* newDiffuseTexture = t_loader.LoadTexture(diffuseFileLocation, m_directX.GetDevice());
            ID3D11ShaderResourceView* newGlowTexture = t_loader.LoadTexture(glowFileLocation, m_directX.GetDevice());
            // Create buffer for material data to send to shader as cbuffer


            // Save data to MaterialInfo
            newMaterial->SetMaterialName(p_materialData.nodeName);
            newMaterial->SetMaterialData(p_materialData);
            newMaterial->SetDiffuseTexture(newDiffuseTexture);
            newMaterial->SetGlowTexture(newGlowTexture);
            newMaterial->SetSamplerState(m_directX.GetDefaultSamplerState());
            m_materialInfo[p_materialData.diffuseTexturePath] = newMaterial;
            return newMaterial;
        }

        SpriteInfo* MeshManagerImpl::BuildSpriteInfo(SpriteData& p_spriteData)
        {
            SpriteInfo* newSpriteInfo = new SpriteInfoImpl();
            newSpriteInfo->SetData(p_spriteData);
            return newSpriteInfo;
        }

        void MeshManagerImpl::AddToRenderList(MeshInfo& p_mesh, MaterialInfo& p_material, const DirectX::XMFLOAT4X4& p_orientationMatrix)
        {
            // TODORT Could be redesigned so the DirectXManager asks this class for this information instead.
            p_material.AlphaFade();
            MeshRenderData meshRenderData(p_orientationMatrix, p_material.GetTexture(), p_material.GetGlowTexture(), p_material.GetSamplerState(),
                                          p_mesh.GetBufferHandle(), p_mesh.GetVerticeCount(), p_mesh.GetIndexBufferHandle(), p_mesh.GetIndexCount(), p_material.GetMaterialData());

            std::string materialName = p_material.GetMaterialName();
            if (materialName[0] == 'T' && materialName[1] == 'T')
            {// TODOXX Extrem fullösning för att få in transparenta meshar!!
                
                // Add transparent mesh in different list
                m_graphicContext.m_graphicModule->GetSubModuleManagerImpl().GetDirectXManagerImpl().AddTransMeshForRendering(meshRenderData);
            }
            else
            {
                m_graphicContext.m_graphicModule->GetSubModuleManagerImpl().GetDirectXManagerImpl().AddMeshForRendering(meshRenderData);
            }
        }

        void MeshManagerImpl::AddSpriteToRenderList(SpriteInfo& p_spriteInfo, MaterialInfo& p_material)
        {
            // TODORT Could be redesigned so the DirectXManager asks this class for this information instead.
            DoremiEditor::Core::MaterialMessage materialData = p_material.GetMaterialData();

            SpriteRenderData spriteRenderData(p_spriteInfo.GetData(), p_material.GetTexture(), p_material.GetGlowTexture(), materialData,
                                              p_material.GetSamplerState());

            m_graphicContext.m_graphicModule->GetSubModuleManagerImpl().GetDirectXManagerImpl().AddSpriteForRendering(spriteRenderData);
        }

        void MeshManagerImpl::Draw() {}
    }
}