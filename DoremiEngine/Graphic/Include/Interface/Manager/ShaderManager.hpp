#pragma once
#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        class PixelShader;
        class VertexShader;
        class ShaderManager
        {
        public:
            //TODOKO Add docs
            virtual VertexShader* BuildVertexShader(const std::string& p_fileName) = 0;
            virtual PixelShader* BuildPixelShader(const std::string& p_fileName) = 0;
        };
    }
}