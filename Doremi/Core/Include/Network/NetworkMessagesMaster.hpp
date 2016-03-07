#pragma once
#include <string>


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
        struct NetMessageMasterClientFromClient;
        struct NetMessageMasterServerFromServer;
        struct ClientConnectionFromMaster;
        struct ServerConnectionFromMaster;

        class NetworkMessagesMaster
        {
        public:
            static NetworkMessagesMaster* GetInstance();

            static void StartupNetworkMessagesMaster(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                Receive Client
            */

            void ReceiveConnectionRequestClient(NetMessageMasterClientFromClient& p_message, DoremiEngine::Network::Adress& p_adress);

            void ReceiveConnectedClient(NetMessageMasterClientFromClient& p_message, DoremiEngine::Network::Adress& p_adress);

            void ReceiveDisconnectClient(NetMessageMasterClientFromClient& p_message, DoremiEngine::Network::Adress& p_adress);

            /**
                Receive Server
            */

            void ReceiveConnectionRequestServer(NetMessageMasterServerFromServer& p_message, DoremiEngine::Network::Adress& p_adress);

            void ReceiveConnectedServer(NetMessageMasterServerFromServer& p_message, DoremiEngine::Network::Adress& p_adress);

            void ReceiveDisconnectServer(NetMessageMasterServerFromServer& p_message, DoremiEngine::Network::Adress& p_adress);

            /**
                Send Client
            */

            void SendConnectedClient(DoremiEngine::Network::Adress& p_adress);

            void SendDisconnectClient(DoremiEngine::Network::Adress& p_adress);

            /**
                Send Server
            */

            void SendConnectedServer(DoremiEngine::Network::Adress& p_adress, uint8_t p_sequence);

            void SendDisconnectServer(DoremiEngine::Network::Adress& p_adress);

        private:
            explicit NetworkMessagesMaster(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~NetworkMessagesMaster();

            static NetworkMessagesMaster* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}