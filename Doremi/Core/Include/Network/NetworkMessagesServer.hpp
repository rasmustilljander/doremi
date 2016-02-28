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
        struct ClientConnectionFromServer;
        struct NetMessageServerClientConnectingFromClient;
        struct NetMessageServerClientConnectedFromClient;

        class NetworkMessagesServer
        {
        public:
            static NetworkMessagesServer* GetInstance();

            static void StartupNetworkMessagesServer(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                == Receive Messages Connecting ==
            */

            /**
                Receive a connection request message
            */
            void ReceiveConnectionRequest(NetMessageServerClientConnectingFromClient& p_message, const DoremiEngine::Network::Adress& p_adress);

            /**
                Receives a version check message
            */
            void ReceiveVersionCheck(NetMessageServerClientConnectingFromClient& p_message, const DoremiEngine::Network::Adress& p_adress);

            /**
                Receives a disconnect message
            */
            void ReceiveDisconnect(NetMessageServerClientConnectingFromClient& p_message, DoremiEngine::Network::Adress& p_adress);

            /**
                == Receive Messages Connected ==
            */

            /**
                TODOCM doc
            */
            void ReceiveConnectedMessage(NetMessageServerClientConnectedFromClient& p_message, ClientConnectionFromServer* p_connection);

            /**
                TODOCM doc
            */
            void ReceiveLoadWorldMessage(NetMessageServerClientConnectedFromClient& p_message, ClientConnectionFromServer* p_connection);

            /**
                TODOCM doc
            */
            void ReceiveInGameMessage(NetMessageServerClientConnectedFromClient& p_message, ClientConnectionFromServer* p_connection);

            /**
                == Send Messages Connecting ==
            */

            /**
                Send version check message
            */
            void SendVersionCheck(const DoremiEngine::Network::Adress& p_adress);

            /**
                Send connect message
            */
            void SendConnect(const ClientConnectionFromServer* p_connection, const DoremiEngine::Network::Adress& p_adress);

            /**
                Send disconnect message to adress
            */
            void SendDisconnect(const DoremiEngine::Network::Adress& p_adress, std::string p_string);

            /**
                == Send Messages Connected ==
            */

            /**
                TODOCM doc
            */
            void SendConnected(ClientConnectionFromServer* p_connection);

            /**
                TODOCM doc
            */
            void SendLoadWorld(ClientConnectionFromServer* p_connection);

            /**
                TODOCM doc
            */
            void SendInGame(ClientConnectionFromServer* p_connection);

            /**
                Update message sequence
            */
            void UpdateSequence() { m_messageSequence++; }

        private:
            NetworkMessagesServer(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~NetworkMessagesServer();

            static NetworkMessagesServer* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;

            uint8_t m_messageSequence;
        };
    }
}