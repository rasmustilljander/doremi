#include <Doremi/Core/Include/ServerStateHandler.hpp>

namespace Doremi
{
    namespace Core
    {
        ServerStateHandler* ServerStateHandler::m_singleton = nullptr;

        ServerStateHandler* ServerStateHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new ServerStateHandler();
            }
            return m_singleton;
        }

        ServerStateHandler::ServerStateHandler() : m_state(ServerStates::LOBBY) {}

        ServerStateHandler::~ServerStateHandler() {}
    }
}