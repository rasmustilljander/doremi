#pragma once
#include <map>
#include <Doremi/Core/Include/Network/Connections.hpp>

namespace DoremiEngine
{
    namespace Network
    {
        class Adress;
    }
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        class NetworkConnectionsServer
        {
        public:
            static NetworkConnectionsServer* GetInstance();

            static void StartupConnectionsServer(const DoremiEngine::Core::SharedContext& p_sharedContext);

            auto GetConnectedClientConnections() { return m_connectedClientConnections; }

            auto GetConnectingClientConnections() { return m_connectingClientConnections; }

            SocketHandle GetConnectingSocketHandle() { return m_connectingSocketHandle; }

            SocketHandle GetConnectedSocketHandle() { return m_connectedSocketHandle; }

            uint32_t GetPortConnecting() { return m_portConnecting; }

            uint32_t GetPortConnected() { return m_portConnected; }

            void CreateNewConnecting(const DoremiEngine::Network::Adress& p_adress);

            void RemoveConnection(DoremiEngine::Network::Adress& p_adress);

            bool AdressWithPortExist(const DoremiEngine::Network::Adress& p_adress, ClientConnectionFromServer*& o_connection);

            bool AdressExistInConnecting(const DoremiEngine::Network::Adress& p_adress, ClientConnectionFromServer*& o_connection);

        private:
            NetworkConnectionsServer(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~NetworkConnectionsServer();

            static NetworkConnectionsServer* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;

            /**
                Max number of connections
            */
            uint8_t m_maxConnections;

            /**
                Socket for incomming connecting messages
            */
            SocketHandle m_connectingSocketHandle;

            /**
                Socket for incomming conneted messages
            */
            SocketHandle m_connectedSocketHandle;

            /**
                Port for connecting
            */
            uint32_t m_portConnecting;

            /**
                Port for connected
            */
            uint32_t m_portConnected;

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