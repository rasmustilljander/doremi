#pragma once
#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        class GraphicModuleImplementation;
        struct GraphicModuleContext
        {
            std::string m_workingDirectory;
            GraphicModuleImplementation* m_graphicModule;
            GraphicModuleContext() {}
        };
    }
}