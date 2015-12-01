#pragma once
#include <Shader/Shader.hpp>
namespace DoremiEngine
{
    namespace Graphic
    {
        class VertexShader : public Shader
        {
        public:
            VertexShader();
            ~VertexShader();
            void SetActiveShader() override;
            bool LoadShader(const std::string& p_fileName) override;
        };
    }
}
