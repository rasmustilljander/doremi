#include <Doremi/Core/Include/Handler/TreeHandler.hpp>

namespace Doremi
{
    namespace Core
    {
        TreeHandler* TreeHandler::m_singleton = nullptr;

        TreeHandler* TreeHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new TreeHandler();
            }
            return m_singleton;
        }

        TreeHandler::~TreeHandler() {}

        void TreeHandler::Update()
        {

        }
        void TreeHandler::OnEvent(Event* p_event)
        {

        }
    }
}