#pragma once
#include <Internal/Manager/MeshManagerImpl.hpp>
#include <DirectXMath.h>
#include <string>
namespace DirectX
{
    struct XMFLOAT4X4;
}
namespace DoremiEngine
{
    namespace Graphic
    {
        MeshManagerImpl::MeshManagerImpl()
        {
        }
        MeshManagerImpl::~MeshManagerImpl()
        {
        }
        MeshInfo* MeshManagerImpl::BuildMeshInfo(const std::string& p_fileName)
        {
            return nullptr;
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