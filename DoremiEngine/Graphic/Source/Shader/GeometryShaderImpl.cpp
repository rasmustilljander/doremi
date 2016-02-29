#pragma once
#include <Internal/Shader/GeometryShaderImpl.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        GeometryShaderImpl::GeometryShaderImpl() {}
        GeometryShaderImpl::~GeometryShaderImpl() {}
        const std::string& GeometryShaderImpl::GetShaderName() { return m_shaderName; }
        ID3D11GeometryShader* GeometryShaderImpl::GetShaderHandle() { return m_shaderHandle; }
    }
}