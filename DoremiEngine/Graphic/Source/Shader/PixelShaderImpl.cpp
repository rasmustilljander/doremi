#pragma once
#include <Internal/Shader/PixelShaderImpl.hpp>
namespace DoremiEngine
{
    namespace Graphic
    {
        PixelShaderImpl::PixelShaderImpl() {}
        PixelShaderImpl::~PixelShaderImpl() {}
        const std::string& PixelShaderImpl::GetShaderName() { return m_shaderName; }
        ID3D11PixelShader* PixelShaderImpl::GetShaderHandle() { return m_shaderHandle; }
    }
}