#include <GraphicModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
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


    }
}

DoremiEngine::Graphic::GraphicModule* CreateGraphicModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Graphic::GraphicModule* graphic = new DoremiEngine::Graphic::GraphicModuleImplementation();
    return graphic;
}