#include <Doremi/Core/Include/MenuClasses/HUDHandler.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>  

namespace Doremi
{
    namespace Core
    {
        HUDHandler* HUDHandler::m_singleton = nullptr;

        HUDHandler * HUDHandler::GetInstance()
        {
            if (m_singleton == nullptr)
            {
                std::runtime_error("GetInstance called before StartHUDHandler");
            }
            return m_singleton;
        }

        void HUDHandler::StartHUDHandler(const DoremiEngine::Core::SharedContext & p_sharedContext, DirectX::XMFLOAT2 p_resolution)
        {
            if (m_singleton != nullptr)
            {
                std::runtime_error("StartHUDHandler called multiple times");
            }
            m_singleton = new HUDHandler(p_sharedContext);
        }

        HUDHandler::HUDHandler(const DoremiEngine::Core::SharedContext & p_sharedContext) : m_sharedContext(p_sharedContext)
        {

        }

        HUDHandler::~HUDHandler()
        {
        }


        void HUDHandler::Update(double p_dt)
        {
            return;
        }
    }
}