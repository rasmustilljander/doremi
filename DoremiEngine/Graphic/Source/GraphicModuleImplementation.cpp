#include <GraphicModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <GraphicMain.hpp>
#include <iostream>

namespace DoremiEngine
{
    namespace Graphic
    {
        GraphicModuleImplementation::GraphicModuleImplementation()
        {
        }

        GraphicModuleImplementation::~GraphicModuleImplementation()
        {
        }

        void GraphicModuleImplementation::Startup()
        {
            
        }

        void GraphicModuleImplementation::SetWorkingDirectory(const std::string& p_workingDirectory)
        {
        }

        void GraphicModuleImplementation::Shutdown()
        {
        }

        int GraphicModuleImplementation::LoadObject(const std::string& p_fileName, const std::string& p_materialFileName)
        {
            return GraphicMain::GetInstance()->LoadObject(p_fileName, p_materialFileName);
        }

        int GraphicModuleImplementation::LoadShader(ShaderType p_shaderType, const std::string& p_fileName)
        {
            return GraphicMain::GetInstance()->LoadShader(p_shaderType, p_fileName);
        }

        void GraphicModuleImplementation::InitializeDirectX()
        {
            //TODORK call GraphicMain metod to init directX
        }

        void GraphicModuleImplementation::BindShader(ShaderType p_shaderType, int p_shaderID)
        {
            GraphicMain::GetInstance()->BindShader(p_shaderType, p_shaderID);
        }

        void GraphicModuleImplementation::Draw()
        {
            //TODORK call Draw method in GraphicsMain when it exists
        }

        void GraphicModuleImplementation::ComputeAfterEffects()
        {
            //TODORK call AfterEffects method in GraphicsMain when it exists
        }

    }
}

DoremiEngine::Graphic::GraphicModule* CreateGraphicModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Graphic::GraphicModule* graphic = new DoremiEngine::Graphic::GraphicModuleImplementation();
    return graphic;
}