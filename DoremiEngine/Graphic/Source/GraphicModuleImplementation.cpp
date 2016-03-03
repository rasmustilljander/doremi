#include <GraphicModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <GraphicModuleContext.hpp>
#include <iostream>

namespace DoremiEngine
{
    namespace Graphic
    {
        GraphicModuleImplementation::GraphicModuleImplementation(const Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            m_subModuleManger = nullptr;
        }

        GraphicModuleImplementation::~GraphicModuleImplementation() {}

        SubModuleManager& GraphicModuleImplementation::GetSubModuleManager() { return *m_subModuleManger; }

        SubModuleManagerImpl& GraphicModuleImplementation::GetSubModuleManagerImpl() { return *m_subModuleManger; }

        void GraphicModuleImplementation::Update() {}

        void GraphicModuleImplementation::Startup()
        {
            if(m_subModuleManger == nullptr)
            {
                m_graphicContext = new GraphicModuleContext(m_sharedContext.GetConfigurationModule());
                m_graphicContext->m_graphicModule = this;
                m_graphicContext->m_workingDirectory = m_sharedContext.GetWorkingDirectory();
                m_subModuleManger = new SubModuleManagerImpl(*m_graphicContext);
                m_subModuleManger->Initialize();
            }
        }

        void GraphicModuleImplementation::Shutdown() {}
    }
}

DoremiEngine::Graphic::GraphicModule* CreateGraphicModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Graphic::GraphicModule* graphic = new DoremiEngine::Graphic::GraphicModuleImplementation(p_sharedContext);
    return graphic;
}