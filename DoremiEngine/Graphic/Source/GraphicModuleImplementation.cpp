#include <GraphicModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <GraphicMain.hpp>
#include <iostream>

namespace DoremiEngine
{
    namespace Graphic
    {
        GraphicModuleImplementation::GraphicModuleImplementation(const Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
        }

        GraphicModuleImplementation::~GraphicModuleImplementation()
        {
        }

        void GraphicModuleImplementation::Startup()
        {
            GraphicMain::GetInstance()->CreateGraphicWindow();
            GraphicMain::GetInstance()->InitializeDirectX();
        }

        void GraphicModuleImplementation::Shutdown()
        {
        }

        int GraphicModuleImplementation::LoadObject(const std::string& p_fileName, const std::string& p_materialFileName)
        {
            return GraphicMain::GetInstance()->LoadMesh(p_fileName);
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

        void GraphicModuleImplementation::EndDraw()
        {
            GraphicMain::GetInstance()->EndDraw();
        }
    }
}

DoremiEngine::Graphic::GraphicModule* CreateGraphicModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Graphic::GraphicModule* graphic = new DoremiEngine::Graphic::GraphicModuleImplementation(p_sharedContext);
    return graphic;
}