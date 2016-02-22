#pragma once


namespace DoremiEngine
{
    namespace Core
    {
        class Adress;
    }
}

namespace Doremi
{
    namespace Core
    {
        struct ClientConnectionFromServer;
        struct NetMessageConnectingFromClient;
        struct NetMessageConnectedFromClient;

        class NetworkMessagesServer
        {
        public:
            /**
                == Receive Messages Unreliable ==
            */

            /**
                Receive a connection request message
            */
            static void ReceiveConnectionRequest(const NetMessageConnectingFromClient& m_message, const DoremiEngine::Network::Adress& m_Adress);

            /**
                Receives a version check message
            */
            static void ReceiveVersionCheck(const NetMessageConnectingFromClient& m_message, const DoremiEngine::Network::Adress& m_Adress);

            /**
                Receives a disconnect message
            */
            static void ReceiveDisconnect(const NetMessageConnectingFromClient& m_message, const DoremiEngine::Network::Adress& m_adress);

            /**
                == Receive Messages Reliable ==
            */

            /**
                TODOCM doc
            */
            static void ReceiveConnectedMessage(const NetMessageConnectedFromClient& p_message, ClientConnectionFromServer* p_connection);

            /**
                TODOCM doc
            */
            static void ReceiveLoadWorldMessage(const NetMessageConnectedFromClient& p_message, ClientConnectionFromServer* p_connection);

            /**
                TODOCM doc
            */
            static void ReceiveInGameMessage(const NetMessageConnectedFromClient& p_message, ClientConnectionFromServer* p_connection);

            /**
                == Send Messages Unreliable ==
            */

            /**
                Send version check message
            */
            static void SendVersionCheck(const NetMessage& m_message, const DoremiEngine::Network::Adress& m_adress);

            /**
                Send connect message
            */
            static void SendConnect(const Connection* connection, const DoremiEngine::Network::Adress& m_adress);

            /**
                Send disconnect message to adress
            */
            static void SendDisconnect(const DoremiEngine::Network::Adress& m_adress, std::string p_outString);

            /**
                == Send Messages Reliable ==
            */

            /**
                TODOCM doc
            */
            static void SendConnected(Connection* p_connection);

            /**
                TODOCM doc
            */
            static void SendLoadWorld(Connection* p_connection);

            /**
                TODOCM doc
            */
            static void SendInGame(NetMessage& p_message, Connection* p_connection);
        };
    }
}