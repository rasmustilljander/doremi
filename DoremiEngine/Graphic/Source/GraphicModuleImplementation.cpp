#include <GraphicModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <GraphicMain.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <iostream>

namespace DoremiEngine
{
    namespace Graphic
    {
        GraphicModuleImplementation::GraphicModuleImplementation(const Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            m_graphicMain = nullptr;
            m_subModuleManger = nullptr;
        }

        GraphicModuleImplementation::~GraphicModuleImplementation() {}

        SubModuleManager& GraphicModuleImplementation::GetSubModuleManager()
        {
            return *m_subModuleManger;
        }

        void GraphicModuleImplementation::Update() {}

        void GraphicModuleImplementation::Startup()
        {
            if(m_subModuleManger == nullptr) // TODO Review if this is a good idea for avoiding multiple instanciations
            {
                m_graphicContext.m_graphicModule = this;
                m_graphicContext.m_workingDirectory = m_sharedContext.GetWorkingDirectory();
                m_subModuleManger = new SubModuleManagerImpl(m_graphicContext);
                m_subModuleManger->Initialize();
            }
        }

        void GraphicModuleImplementation::Shutdown()
        {
        }

        int GraphicModuleImplementation::LoadObject(const std::string& p_fileName, const std::string& p_materialFileName)
        {
            return m_graphicMain->LoadMesh(p_fileName);
        }

        int GraphicModuleImplementation::LoadShader(ShaderType p_shaderType, const std::string& p_fileName)
        {
            return m_graphicMain->LoadShader(p_shaderType, p_fileName);
        }

        void GraphicModuleImplementation::InitializeDirectX()
        {
            //TODORK call GraphicMain metod to init directX
        }

        void GraphicModuleImplementation::BindShader(ShaderType p_shaderType, int p_shaderID)
        {
            m_graphicMain->BindShader(p_shaderType, p_shaderID);
        }

        void GraphicModuleImplementation::Draw(const int& p_meshID, const int& p_textureID, const DirectX::XMFLOAT4X4& p_translationMatrix)
        {
            m_graphicMain->Draw(p_meshID, p_textureID, p_translationMatrix); //TODO Decide if we should use direcx math
        }

        void GraphicModuleImplementation::ComputeAfterEffects()
        {
            //TODORK call AfterEffects method in GraphicsMain when it exists
        }

        void GraphicModuleImplementation::EndDraw()
        {
            m_graphicMain->EndDraw();
        }
    }
}

DoremiEngine::Graphic::GraphicModule* CreateGraphicModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Graphic::GraphicModule* graphic = new DoremiEngine::Graphic::GraphicModuleImplementation(p_sharedContext);
    return graphic;
}