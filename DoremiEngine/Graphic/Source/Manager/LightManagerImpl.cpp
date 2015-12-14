#include <Internal/Manager/LightManagerImpl.hpp>
#include <Internal/Light/LightInfoImpl.hpp>


namespace DoremiEngine
{
    namespace Graphic
    {
        LightManagerImpl::LightManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext) {}


        LightManagerImpl::~LightManagerImpl() {}

        int LightManagerImpl::AddLight(LightInfo light)
        {
            lightList.push_back(light);
            return lightList.size() - 1;
        }

        LightInfo LightManagerImpl::GetLight(int index) { return lightList[index]; }

        std::vector<LightInfo> LightManagerImpl::GetLightList() { return lightList; }
    }
}