#pragma once
#include <Internal/Manager/ShaderManagerImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <GraphicModuleImplementation.hpp>
#include <Internal/Shader/PixelShaderImpl.hpp>
#include <Internal/Shader/VertexShaderImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <string>
#include <HelpFunctions.hpp>

#include <d3dcompiler.h>
#include <d3d11_1.h>
namespace DoremiEngine
{
    namespace Graphic
    {        
        ShaderManagerImpl::ShaderManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext) {}
        ShaderManagerImpl::~ShaderManagerImpl() {}
        VertexShader* ShaderManagerImpl::BuildVertexShader(const std::string& p_fileName, D3D11_INPUT_ELEMENT_DESC p_inputDescription[], int p_arraySize)
        {
            ID3D11VertexShader* shader;
            ID3D11InputLayout* inputLayout;
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined(_DEBUG)
            shaderFlags |= D3DCOMPILE_DEBUG;
            shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif            
            ID3DBlob *tShader;
            std::wstring convertedName = StringToWstring(p_fileName);
            HRESULT res = D3DCompileFromFile(convertedName.c_str(), 0, 0, "VS_main", "vs_5_0", shaderFlags, 0, &tShader, 0);

            bool success = CheckHRESULT(res, "Error Compiling from file " + p_fileName);
            if (!success)
            {
                return nullptr;
            }
            res = m_directX.GetDevice()->CreateVertexShader(tShader->GetBufferPointer(), tShader->GetBufferSize(), NULL, &shader);
            success = CheckHRESULT(res, "Error Creating Vertex Shader");
            if (!success)
            {
                return nullptr;
            }

            res = m_directX.GetDevice()->CreateInputLayout(p_inputDescription, p_arraySize, tShader->GetBufferPointer(), tShader->GetBufferSize(), &inputLayout);
            success = CheckHRESULT(res, "Error setting input layout");
            if (!success)
            {
                return nullptr;
            }

            VertexShader* newShader = new VertexShaderImpl();
            newShader->SetShaderHandle(shader);
            newShader->SetShaderName(p_fileName);
            newShader->SetInputLayout(inputLayout);
            return newShader;
        }
        PixelShader* ShaderManagerImpl::BuildPixelShader(const std::string& p_fileName) 
        {
            ID3D11PixelShader* shader;
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined(_DEBUG)
            shaderFlags |= D3DCOMPILE_DEBUG;
            shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif  
            ID3DBlob *tShader;
            std::wstring convertedName = StringToWstring(p_fileName);
            HRESULT res = D3DCompileFromFile(convertedName.c_str(), 0, 0, "PS_main", "ps_5_0", shaderFlags, 0, &tShader, 0);
            res = m_directX.GetDevice()->CreatePixelShader(tShader->GetBufferPointer(), tShader->GetBufferSize(), NULL, &shader);
            bool success = CheckHRESULT(res, "Error Compiling from file " + p_fileName);
            PixelShader* newShader = new PixelShaderImpl();
            newShader->SetShaderHandle(shader);
            newShader->SetShaderName(p_fileName);
            return newShader;
        }
        void ShaderManagerImpl::SetActiveVertexShader(VertexShader* p_shader)
        {
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            m_directX.GetDeviceContext()->VSSetShader(p_shader->GetShaderHandle(), 0, 0);
        }
        void ShaderManagerImpl::SetActivePixelShader(PixelShader* p_shader)
        {
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            m_directX.GetDeviceContext()->PSSetShader(p_shader->GetShaderHandle(), 0, 0);
        }
    }
}