#include <GraphicMain.hpp>
#include <Shader/VertexShader.hpp>
#include <Shader/PixelShader.hpp>
#include <GraphicObject.hpp>
#include <ModelLoader.hpp>
#include <ShaderTypeEnum.hpp>
namespace DoremiEngine
{
    namespace Graphic
    {
        GraphicMain* GraphicMain::m_singleton = nullptr;
        GraphicMain* GraphicMain::GetInstance()
        {
            if (m_singleton == nullptr)
            {
                m_singleton = new GraphicMain();
            }
            return m_singleton;
        }

        GraphicMain::GraphicMain()
        {
        }


        GraphicMain::~GraphicMain()
        {
        }
        
        int GraphicMain::LoadShader(const ShaderType& p_type, const std::string& p_fileName)
        {
            int returnID;
            bool success;
            switch (p_type)
            {
            case ShaderType::VertexShader:
            {
                VertexShader* t_shader = new VertexShader();
                success = t_shader->LoadShader(p_fileName);
                if (!success)
                {
                    //Some kind of error, use debugg logger :)
                    delete t_shader;
                    return -1;
                }
                m_vertexShaders.push_back(t_shader);
                returnID = m_vertexShaders.size() - 1;
                break;
            }
            case ShaderType::PixelShader:
            {
                PixelShader* t_shader = new PixelShader();
                success = t_shader->LoadShader(p_fileName);
                if (!success)
                {
                    //Some kind of error, use debugg logger :)
                    delete t_shader;
                    return -1;
                }
                m_pixelShaders.push_back(t_shader);
                returnID = m_pixelShaders.size() - 1;
                break;
            }
            default:
                break;
            }
            return returnID;
        }
        void GraphicMain::BindShader(const ShaderType& p_type, const int& p_shaderID)
        {
            switch (p_type)
            {
            case ShaderType::VertexShader:
                m_vertexShaders[p_shaderID]->SetActiveShader();
                break;
            case ShaderType::PixelShader:
                m_pixelShaders[p_shaderID]->SetActiveShader();
                break;
            default:
                break;
            }
        }
        int GraphicMain::LoadObject(const std::string& p_meshFileName, const std::string& p_materialName)
        {
            ModelLoader t_loader = ModelLoader();
            int meshID = t_loader.LoadMesh(p_meshFileName);
            if (meshID == -1)
            {
                //TODORK call error and make sure it doesnt crash
            }
            int materialID = t_loader.LoadMaterial(p_materialName);
            if (materialID == -1)
            {
                //TODORK call error and make sure it doesnt crash
            }
            GraphicObject* newObject = new GraphicObject(meshID, materialID);
            m_graphicObjects.push_back(newObject);
            return m_graphicObjects.size() - 1;
        }
    }
}