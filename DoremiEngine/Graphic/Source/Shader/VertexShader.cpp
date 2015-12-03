#include <Shader/VertexShader.hpp>
#include <HelpFunctions.hpp>

// Third Party
#include <d3dcompiler.h>
#include <d3d11_1.h>

// Standard 
#include <Windows.h>


namespace DoremiEngine
{
    namespace Graphic
    {
        VertexShader::VertexShader()
        {
        }


        VertexShader::~VertexShader()
        {
        }

        void VertexShader::SetActiveShader(ID3D11DeviceContext* p_deviceContext)
        {
            p_deviceContext->VSSetShader(m_shader, 0, 0);
            p_deviceContext->IASetInputLayout(m_inputLayout);
        }

        bool VertexShader::LoadShader(const std::string& p_fileName, ID3D11Device* p_device)
        {
            DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined(_DEBUG)
            shaderFlags |= D3DCOMPILE_DEBUG;
            shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif              
            std::wstring convertedName = StringToWstring(p_fileName);
            HRESULT res = D3DCompileFromFile(convertedName.c_str(), 0, 0, "main", "vs_5_0", shaderFlags, 0, &m_shaderInformation, 0);
            bool success = CheckHRESULT(res, "Error Compiling from file " + p_fileName);
            if (!success)
            {
                return false;
            }
            res = p_device->CreateVertexShader(m_shaderInformation->GetBufferPointer(), m_shaderInformation->GetBufferSize(), NULL, &m_shader);
            success = CheckHRESULT(res, "Error Creating Vertex Shader");

            return success;
        }

        bool VertexShader::SetInputLayout(D3D11_INPUT_ELEMENT_DESC p_inputDescription[], int p_arraySize, ID3D11Device* p_device)
        {
            HRESULT res = p_device->CreateInputLayout(p_inputDescription, p_arraySize, m_shaderInformation->GetBufferPointer(), m_shaderInformation->GetBufferSize(), &m_inputLayout);
            bool success = CheckHRESULT(res, "Error setting input layout");
            return success;
        }
    }
}