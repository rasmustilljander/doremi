#pragma once
#include <Interface/Manager/ShaderManager.hpp>
#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        class ShaderManagerImpl : public ShaderManager
        {
        public:
            //TODOKO Add docs
            ShaderManagerImpl();
            virtual ~ShaderManagerImpl();
            VertexShader* BuildVertexShader(const std::string& p_fileName) override;
            PixelShader* BuildPixelShader(const std::string& p_fileName) override;
        };
    }
}