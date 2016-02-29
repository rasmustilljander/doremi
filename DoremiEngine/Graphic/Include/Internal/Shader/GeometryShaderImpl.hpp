#pragma once
#include <Interface/Shader/GeometryShader.hpp>
namespace DoremiEngine
{
    namespace Graphic
    {
        class GeometryShaderImpl : public GeometryShader
        {
        public:
            GeometryShaderImpl();
            virtual ~GeometryShaderImpl();
            const std::string& GetShaderName() override;
            ID3D11GeometryShader* GetShaderHandle() override;

            virtual void SetShaderName(const std::string& p_shaderName) override { m_shaderName = p_shaderName; };
            virtual void SetShaderHandle(ID3D11GeometryShader* p_shaderHandle) override { m_shaderHandle = p_shaderHandle; };

        private:
            ID3D11GeometryShader* m_shaderHandle;
            std::string m_shaderName;
        };
    }
}