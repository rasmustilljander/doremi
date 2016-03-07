#pragma once
#include "..\..\Include\Internal\Shader\ComputeShaderImpl.hpp"
namespace DoremiEngine
{
    namespace Graphic
    {
        ComputeShaderImpl::ComputeShaderImpl() : m_shaderHandle(nullptr) {}
        ComputeShaderImpl::~ComputeShaderImpl() {}
        const std::string& ComputeShaderImpl::GetShaderName() { return m_shaderName; }
        ID3D11ComputeShader* ComputeShaderImpl::GetShaderHandle() { return m_shaderHandle; }
    }
}