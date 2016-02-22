#include <Doremi/Core/Include/Network/NetworkMessagesServer.hpp>

// Connections
#include <Doremi/Core/Include/Network/NetworkConnectionsServer.hpp>

// Adress
#include <DoremiEngine/Network/Include/Adress.hpp>

// Net messages
#include <Doremi/Core/Include/Network/NetMessages.hpp>


namespace Doremi
{
    namespace Core
    {

        /**
            == Receive Messages Unreliable ==
        */
        void NetworkMessagesServer::ReceiveConnectionRequest(const NetMessageConnectingFromClient& m_message, const DoremiEngine::Network::Adress& m_Adress)
        {
        }

        void NetworkMessagesServer::ReceiveVersionCheck(const NetMessageConnectingFromClient& m_message, const DoremiEngine::Network::Adress& m_Adress)
        {
        }

        void NetworkMessagesServer::ReceiveDisconnect(const NetMessageConnectingFromClient& m_message, const DoremiEngine::Network::Adress& m_adress)
        {
        }

        /**
            == Receive Messages Reliable ==
        */

        void NetworkMessagesServer::ReceiveConnectedMessage(const NetMessageConnectedFromClient& p_message, ClientConnectionFromServer* p_connection)
        {
            // doesn't exit yet
        }

        void NetworkMessagesServer::ReceiveLoadWorldMessage(const NetMessageConnectedFromClient& p_message, ClientConnectionFromServer* p_connection)
        {
        }

        void NetworkMessagesServer::ReceiveInGameMessage(const NetMessageConnectedFromClient& p_message, ClientConnectionFromServer* p_connection)
        {
            // Check name
        }

        void NetworkMessagesServer::SendVersionCheck(const NetMessage& m_message, const DoremiEngine::Network::Adress& m_adress) {}

        void NetworkMessagesServer::SendConnect(const Connection* connection, const DoremiEngine::Network::Adress& m_adress) {}

        void NetworkMessagesServer::SendDisconnect(const DoremiEngine::Network::Adress& m_adress, std::string p_outString) {}

        /**
            == Send Messages Reliable ==
        */

        void NetworkMessagesServer::SendConnected(Connection* p_connection) {}

        void NetworkMessagesServer::SendLoadWorld(Connection* p_connection) {}

        void NetworkMessagesServer::SendInGame(NetMessage& p_message, Connection* p_connection)
        {
            // Check name
        }
    }
}