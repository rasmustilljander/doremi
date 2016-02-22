#include <Doremi/Core/Include/Network/NetworkConnectionsServer.hpp>

namespace Doremi
{
    namespace Core
    {
        NetworkConnectionsServer* NetworkConnectionsServer::m_singleton = nullptr;

        NetworkConnectionsServer* NetworkConnectionsServer::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new NetworkConnectionsServer();
            }
            return m_singleton;
        }

        NetworkConnectionsServer::NetworkConnectionsServer() {}

        NetworkConnectionsServer::~NetworkConnectionsServer() {}
    }
}