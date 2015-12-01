#pragma once

#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        class Shader
        {
        public:
            Shader();
            ~Shader();
            virtual void SetActiveShader();
            //Loads the shader with directX, returns success
            virtual bool LoadShader(const std::string& p_fileName);

        };
    }
}
