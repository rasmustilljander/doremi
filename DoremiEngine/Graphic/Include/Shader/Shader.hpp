#pragma once

#include <string>
struct ID3D11DeviceContext;
struct ID3D11Device;
namespace DoremiEngine
{
    namespace Graphic
    {
        
        class Shader
        {
        public:
            Shader();
            ~Shader();
            virtual void SetActiveShader(ID3D11DeviceContext* p_deviceContext);
            //Loads the shader with directX, returns success
            virtual bool LoadShader(const std::string& p_fileName, ID3D11Device* p_device);

        };
    }
}
