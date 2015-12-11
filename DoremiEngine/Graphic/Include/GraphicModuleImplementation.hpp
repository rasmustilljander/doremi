#pragma once
#include <GraphicModule.hpp>
#include <GraphicModuleContext.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        class SubModuleManagerImpl;

        class GraphicModuleImplementation : public GraphicModule
        {
            public:
            /**
            TODO docs
            */
            GraphicModuleImplementation(const Core::SharedContext& p_sharedContext);

            /**
            TODO docs
            */
            virtual ~GraphicModuleImplementation();

            SubModuleManager& GetSubModuleManager() override;
            /**
            TODORT docs
            */
            void Update() override;

            /**
            TODO docs
            */
            void Startup() override;

            /**
            TODO docs
            */
            void Shutdown() override;

            private:
            const Core::SharedContext& m_sharedContext;
            SubModuleManagerImpl* m_subModuleManger;
            GraphicModuleContext m_graphicContext;
        };
    }
}