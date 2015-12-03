#include <Shader/Shader.hpp>
#include <HelpFunctions.hpp>

// Third Party
#include <d3dcompiler.h>
#include <d3d11_1.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        Shader::Shader()
        {
        }


        Shader::~Shader()
        {
        }
        void Shader::SetActiveShader(ID3D11DeviceContext* p_deviceContext)
        {
        }
        //Loads the shader with directX, returns success
        bool Shader::LoadShader(const std::string& p_fileName, ID3D11Device* p_device)
        {
            return true;
        }
    }
}