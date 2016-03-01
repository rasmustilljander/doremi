#pragma once
#include <Internal/Texture/SpriteInfoImpl.hpp>

#include <d3d11_1.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        SpriteInfoImpl::SpriteInfoImpl() : m_bufferHandle(nullptr) {}
        SpriteInfoImpl::~SpriteInfoImpl() {}
        void SpriteInfoImpl::SetBufferHandle(ID3D11Buffer* p_bufferHandle) { m_bufferHandle = p_bufferHandle; }
    }
}