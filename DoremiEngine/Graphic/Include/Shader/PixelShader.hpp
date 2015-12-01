#pragma once
#include <Shader/Shader.hpp>
namespace DoremiEngine
{
    namespace Graphic
    {
        class PixelShader : public Shader
        {
        public:
            PixelShader();
            ~PixelShader();
            void SetActiveShader() override;
            bool LoadShader(const std::string& p_fileName) override;
        };
    }
}