#pragma once
#include <Shader/Shader.hpp>
class ID3D11PixelShader;
namespace DoremiEngine
{
    namespace Graphic
    {      
        class PixelShader : public Shader
        {
        public:
            PixelShader();
            ~PixelShader();
            void SetActiveShader(ID3D11DeviceContext* p_deviceContext) override;
            bool LoadShader(const std::string& p_fileName, ID3D11Device* p_device) override;
        private:
            ID3D11PixelShader* m_shader;
        };
    }
}