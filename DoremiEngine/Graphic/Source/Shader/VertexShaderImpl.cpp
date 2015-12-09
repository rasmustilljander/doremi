#pragma once
#include <Internal/Shader/VertexShaderImpl.hpp>
namespace DoremiEngine
{
    namespace Graphic
    {
        VertexShaderImpl::VertexShaderImpl(){}
        VertexShaderImpl::~VertexShaderImpl(){}
        const std::string& VertexShaderImpl::GetShaderName() { return m_shaderName; }
        ID3D11VertexShader* VertexShaderImpl::GetShaderHandle() { return m_shaderHandle; }
        ID3D11InputLayout* VertexShaderImpl::GetInputLayout() { return m_inputLayout; }
    }
}