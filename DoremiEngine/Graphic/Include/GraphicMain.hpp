#pragma once
// Project specific

// Standard libraries
#include <vector>
#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        class PixelShader;
        class VertexShader;
        class GraphicObject;
        enum class ShaderType { VertexShader, PixelShader };
        class GraphicMain
        {
        public:
            static GraphicMain* GetInstance();
            ~GraphicMain();
            /**
            Loads the shader from the given location and returns an id
            */
            int LoadShader(const ShaderType& p_type, const std::string& p_fileName);
            void BindShader(const ShaderType& p_type, const int& p_shaderID);
            int LoadObject(const std::string& p_meshFileName, const std::string& p_materialName);
        private:
            static GraphicMain* m_singleton;
            GraphicMain();

            std::vector<VertexShader*> m_vertexShaders;
            std::vector<PixelShader*> m_pixelShaders;
            std::vector<GraphicObject*> m_graphicObjects;      
        };
    }
}
