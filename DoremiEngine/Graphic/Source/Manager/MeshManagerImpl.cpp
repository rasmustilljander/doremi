#pragma once
#include <Internal/Manager/MeshManagerImpl.hpp>
#include <Internal/Mesh/MeshInfoImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <GraphicModuleImplementation.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <ModelLoader.hpp>

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
        void MeshManagerImpl::AddToRenderList(MeshInfo& p_mesh, MaterialInfo& p_material, DirectX::XMFLOAT4X4 p_orientationMatrix)
        {
            //TODOKO This should sort the meshes to enable instanced draw
            //TODOKO Remove draw from this place and down to the acctual draw..
            //DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();

            //D3D11_MAPPED_SUBRESOURCE tMS;
            //m_directX.GetDeviceContext()->Map(m_worldMatrix, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            //memcpy(tMS.pData, &p_orientationMatrix, sizeof(p_orientationMatrix));
            //m_directX.GetDeviceContext()->Unmap(m_worldMatrix, NULL);
            //m_directX.GetDeviceContext()->VSSetConstantBuffers(1, 1, &m_worldMatrix);

            //ID3D11Buffer* bufferPointer = p_mesh.GetBufferHandle();
            //unsigned int stride = sizeof(Vertex);
            //unsigned int offset = 0;
            //m_directX.GetDeviceContext()->IASetVertexBuffers(0, 1, &bufferPointer, &stride, &offset);
            //m_directX.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            //m_directX.GetDeviceContext()->Draw(p_mesh.GetVerticeCount(), 0);
        }
        void MeshManagerImpl::Draw()
        {
        }
    }
}