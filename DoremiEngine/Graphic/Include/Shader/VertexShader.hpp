#pragma once
#include <Shader/Shader.hpp>
class ID3D11VertexShader;
class ID3D11InputLayout;
struct ID3D10Blob;
struct D3D11_INPUT_ELEMENT_DESC;
namespace DoremiEngine
{
    namespace Graphic
    {
        class VertexShader : public Shader
        {
        public:
            VertexShader();
            ~VertexShader();
            void SetActiveShader(ID3D11DeviceContext* p_deviceContext) override;
            bool LoadShader(const std::string& p_fileName, ID3D11Device* p_device) override;
            bool SetInputLayout(D3D11_INPUT_ELEMENT_DESC p_inputDescription[], int p_arraySize, ID3D11Device* p_device);
        private:
            ID3D10Blob* m_shaderInformation;
            ID3D11VertexShader* m_shader;
            ID3D11InputLayout* m_inputLayout;
        };
    }
}
