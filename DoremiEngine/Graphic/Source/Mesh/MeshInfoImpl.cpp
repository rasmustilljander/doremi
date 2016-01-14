#pragma once
#include <Internal/Mesh/MeshInfoImpl.hpp>
#include <d3d11_1.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        MeshInfoImpl::MeshInfoImpl() : m_bufferHandle(nullptr), m_indexBufferHandle(nullptr) {}
        MeshInfoImpl::~MeshInfoImpl() {}
        void MeshInfoImpl::SetBufferHandle(ID3D11Buffer* p_bufferHandle) { m_bufferHandle = p_bufferHandle; }
    }
}