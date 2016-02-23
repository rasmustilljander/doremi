#pragma once
#include <Doremi/Core/Include/Network/Connections.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
    namespace Network
    {
        class Adress;
    }
}

namespace Doremi
{
    namespace Core
    {
        class NetworkConnectionsClient
        {
        public:
            static NetworkConnectionsClient* GetInstance();

            static void StartupNetworkConnectionsClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            SocketHandle GetServerConnectedSocketHandle() { return m_serverConnectedSocketHandle; }

            SocketHandle GetServerConnectingSocketHandle() { return m_serverConnectingSocketHandle; }

            DoremiEngine::Network::Adress* GetServerAdress() { return m_serverAdress; }

            DoremiEngine::Network::Adress* GetMasterAdress() { return m_masterAdress; }

            ServerConnectionFromClient m_serverConnectionState;

            MasterConnectionFromClient m_masterConnectionState;

        private:
            NetworkConnectionsClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~NetworkConnectionsClient();

            static NetworkConnectionsClient* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;

            DoremiEngine::Network::Adress* m_serverAdress;

            DoremiEngine::Network::Adress* m_masterAdress;

            SocketHandle m_serverConnectedSocketHandle;

            SocketHandle m_serverConnectingSocketHandle;
        };
    }
}