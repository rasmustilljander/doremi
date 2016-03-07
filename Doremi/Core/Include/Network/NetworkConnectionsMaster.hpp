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
        class NetworkConnectionsMaster
        {
        public:
            static NetworkConnectionsMaster* GetInstance();

            static void StartupNetworkConnectionsMaster(const DoremiEngine::Core::SharedContext& p_sharedContext);

            auto& GetClientConnections() { return m_clientConnections; }

            auto& GetServerConnections() { return m_serverConnections; }

            SocketHandle GetSocketHandleForClients() { return m_socketHandleClients; };

            SocketHandle GetSocketHandleForServers() { return m_socketHandleServers; };

            bool AdressWithPortExistClients(const DoremiEngine::Network::Adress& p_adress, ClientConnectionFromMaster*& o_connection);

            bool AdressWithPortExistServers(const DoremiEngine::Network::Adress& p_adress, ServerConnectionFromMaster*& o_connection);

            ClientConnectionFromMaster* CreateNewConnectionClients(const DoremiEngine::Network::Adress& p_adress);

            ServerConnectionFromMaster* CreateNewConnectionServers(const DoremiEngine::Network::Adress& p_adress);


        private:
            NetworkConnectionsMaster(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~NetworkConnectionsMaster();

            static NetworkConnectionsMaster* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;

            /**
            Socket for incomming connecting messages
            */
            SocketHandle m_socketHandleClients;

            /**
            Socket for incomming connecting messages
            */
            SocketHandle m_socketHandleServers;

            /**
                Port for connecting
            */
            uint32_t m_port;

            /**
            Map with connecting clients
            */
            std::map<DoremiEngine::Network::Adress*, ClientConnectionFromMaster*> m_clientConnections;

            /**
            Map with connected clients
            */
            std::map<DoremiEngine::Network::Adress*, ServerConnectionFromMaster*> m_serverConnections;
        };
    }
}