#include <Shader/PixelShader.hpp>
#include <HelpFunctions.hpp>
// Standard 
#include <Windows.h>
#include <d3dcompiler.h>
#include <d3d11_1.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        PixelShader::PixelShader()
        {
        }


        PixelShader::~PixelShader()
        {
        }
        void PixelShader::SetActiveShader(ID3D11DeviceContext* p_deviceContext)
        {
            p_deviceContext->PSSetShader(m_shader, 0, 0);
        }

        bool PixelShader::LoadShader(const std::string& p_fileName, ID3D11Device* p_device)
        {
            DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined(_DEBUG)
            shaderFlags |= D3DCOMPILE_DEBUG;
            shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif  
            ID3DBlob *tShader;
            std::wstring convertedName = StringToWstring(p_fileName);
            D3DCompileFromFile(convertedName.c_str(), 0, 0, "PS_main", "ps_5_0", shaderFlags, 0, &tShader, 0);
            HRESULT res = p_device->CreatePixelShader(tShader->GetBufferPointer(), tShader->GetBufferSize(), NULL, &m_shader);
            bool success = CheckHRESULT(res, "Error Compiling from file " + p_fileName);
            if (!success)
            {
                return false;
            }
            return true;
        }
    }
}