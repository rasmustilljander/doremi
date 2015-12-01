#include <Shader/Shader.hpp>

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
        void Shader::SetActiveShader()
        {}
        //Loads the shader with directX, returns success
        bool Shader::LoadShader(const std::string& p_fileName)
        {
            return true;
        }
    }
}