#pragma once
#include <map>
#include <Doremi/Core/Include/Network/Connections.hpp>
#include <DoremiEngine/Network/Include/Adress.hpp>

namespace Doremi
{
    namespace Core
    {
        class NetworkConnectionsServer
        {
        public:
            static NetworkConnectionsServer* GetInstance();

            auto GetConnectedClientConnections() { return m_connectedClientConnections; }

            auto GetConnectingClientConnections() { return m_connectingClientConnections; }

        private:
            static NetworkConnectionsServer* m_singleton;

            NetworkConnectionsServer();

            ~NetworkConnectionsServer();

            /**
                Connection to master server
            */
            MasterConnectionFromServer m_masterConnection;

            /**
                Map with connecting clients
            */
            std::map<DoremiEngine::Network::Adress*, ClientConnectionFromServer*> m_connectingClientConnections;

            /**
                Map with connected clients
            */
            std::map<DoremiEngine::Network::Adress*, ClientConnectionFromServer*> m_connectedClientConnections;
        };
    }
}