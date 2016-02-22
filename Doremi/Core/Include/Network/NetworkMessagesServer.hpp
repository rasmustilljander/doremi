#pragma once

namespace Doremi
{
    namespace Core
    {
        class NetworkMessagesServer
        {
        public:
            /**
                TODOCM doc
            */
            void ReceiveLoadWorldMessage(NetMessage& p_message, Connection* p_connection);

            /**
                Receive a connection request message
            */
            void ReceiveConnectionRequest(const NetMessage& m_message, const DoremiEngine::Network::Adress& m_Adress);

            /**
                Receives a version check message
            */
            void ReceiveVersionCheck(NetMessage& m_message, const DoremiEngine::Network::Adress& m_Adress);

            /**
                Receives a disconnect message
            */
            void ReceiveDisconnect(const NetMessage& m_message, const DoremiEngine::Network::Adress& m_adress);

            /**
                Send disconnect message to adress
            */
            void SendDisconnect(const DoremiEngine::Network::Adress& m_adress, std::string p_outString);

            /**
                Send version check message
            */
            void SendVersionCheck(const DoremiEngine::Network::Adress& m_adress);

            /**
                Send connect message
            */
            void SendConnect(const Connection* connection, const DoremiEngine::Network::Adress& m_adress);

            /**
                TODOCM doc
            */
            void SendConnected(Connection* p_connection);

            /**
                TODOCM doc
            */
            void SendLoadWorld(Connection* p_connection);

            /**
                TODOCM doc
            */
            void SendInGame(NetMessage& p_message, Connection* p_connection);
        };
    }
}