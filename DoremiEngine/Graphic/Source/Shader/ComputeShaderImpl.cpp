#pragma once
#include "..\..\Include\Internal\Shader\ComputeShaderImpl.hpp"
namespace DoremiEngine
{
    namespace Graphic
    {
        ComputeShaderImpl::ComputeShaderImpl() {}
        ComputeShaderImpl::~ComputeShaderImpl() {}
        const std::string& ComputeShaderImpl::GetShaderName() { return m_shaderName; }
        ID3D11ComputeShader* ComputeShaderImpl::GetShaderHandle() { return m_shaderHandle; }
    }
}