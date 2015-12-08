#pragma once
#include <Internal/Manager/ShaderManagerImpl.hpp>
#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {        
        ShaderManagerImpl::ShaderManagerImpl() {}
        ShaderManagerImpl::~ShaderManagerImpl() {}
        VertexShader* ShaderManagerImpl::BuildVertexShader(const std::string& p_fileName) { return nullptr; }
        PixelShader* ShaderManagerImpl::BuildPixelShader(const std::string& p_fileName) { return nullptr; }

    }
}