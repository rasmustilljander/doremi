#pragma once

#include <Interface/Manager/LightManager.hpp>
#include <vector>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct GraphicModuleContext;
        class LightManagerImpl : public LightManager
        {
            public:
            LightManagerImpl(const GraphicModuleContext& p_graphicContext);
            virtual ~LightManagerImpl();
            int AddLight(LightInfo light) override;
            LightInfo GetLight(int index) override;
            std::vector<LightInfo> GetLightList() override;

            private:
            const GraphicModuleContext& m_graphicContext;
            std::vector<LightInfo> lightList;
        };
    }
}
