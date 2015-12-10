#pragma once
#include <Interface/Manager/ParticleSystemManager.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct GraphicModuleContext;
        class ParticleSystemManagerImpl : public ParticleSystemManager
        {
            public:
            ParticleSystemManagerImpl(const GraphicModuleContext& p_graphicContext);
            virtual ~ParticleSystemManagerImpl();
            // TODOKO add something
            private:
            const GraphicModuleContext& m_graphicContext;
        };
    }
}