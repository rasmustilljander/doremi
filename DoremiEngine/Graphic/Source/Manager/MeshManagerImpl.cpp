#pragma once
#include <Internal/Manager/MeshManagerImpl.hpp>
#include <Internal/Mesh/MeshInfoImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <GraphicModuleImplementation.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>

#include <ModelLoader.hpp>
#include <DirectXMath.h>
#include <string>

namespace DoremiEngine
{
    namespace Graphic
    {
        MeshManagerImpl::MeshManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext)
        {
            m_modelLoader = new ModelLoader();
        }
        MeshManagerImpl::~MeshManagerImpl()
        {
        }
        MeshInfo* MeshManagerImpl::BuildMeshInfo(const std::string& p_fileName)
        {
            MeshInfo* newMesh = new MeshInfoImpl();
            newMesh->SetFileName(p_fileName);
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            m_modelLoader->LoadMesh(newMesh, p_fileName, m_directX.GetDeviceContext(), m_directX.GetDevice());
            return newMesh;
        }
        MaterialInfo* MeshManagerImpl::BuildMaterialInfo(const std::string& p_fileName)
        {
            return nullptr;
        }
        void MeshManagerImpl::AddToRenderList(const MeshInfo& p_mesh, const MaterialInfo& p_material, DirectX::XMFLOAT4X4 p_orientationMatrix)
        {
        }
        void MeshManagerImpl::Draw()
        {
        }
    }
}