#pragma once

#include <Interface/Manager/LightManager.hpp>
#include <Internal/Light/LightInfoImpl.hpp>
#include <d3d11_1.h>
#include <vector>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct GraphicModuleContext;
        // Global light list
        struct LightBuffer
        {
            Light lightList[100];
        };

        class LightManagerImpl : public LightManager
        {
        public:
            LightManagerImpl(const GraphicModuleContext& p_graphicContext);
            virtual ~LightManagerImpl();
            int AddLight(Light light) override;
            Light GetLight(int index) override;
            void InitLightManager() override;

        private:
            const GraphicModuleContext& m_graphicContext;
            ID3D11Device* m_device;
            ID3D11DeviceContext* m_deviceContext;
            LightBuffer m_lightBuffer;
        };
    }
}
