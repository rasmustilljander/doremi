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

        ServerStateHandler::ServerStateHandler()
            : m_state(ServerStates::IN_GAME), m_serverName("FirstServer"), m_maxPlayers(16), m_map(GameMap::BEST_MAP)
        {
        }

        ServerStateHandler::~ServerStateHandler() {}

        void ServerStateHandler::SetServerName(std::string p_name)
        {
            // 19 chars is max size of name
            p_name.resize(19);

            m_serverName = p_name;
        }

        std::string ServerStateHandler::GetServerName() { return m_serverName; }
    }
}