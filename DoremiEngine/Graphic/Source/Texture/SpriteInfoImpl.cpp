#pragma once
#include <Internal/Texture/SpriteInfoImpl.hpp>

#include <d3d11_1.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        SpriteInfoImpl::SpriteInfoImpl() {}
        SpriteInfoImpl::~SpriteInfoImpl() {}

        void SpriteInfoImpl::SetData(const SpriteData& p_spriteData) { m_datainfo = SpriteData(p_spriteData); }
    }
}