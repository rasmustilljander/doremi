// Project specific
#include <Doremi/Core/Include/Network/NetworkManagerServer.hpp>

// Modules
#include <DoremiEngine/Network/Include/NetworkModule.hpp>

// Streamer
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>

// Handlers
#include <Doremi/Core/Include/PlayerHandlerServer.hpp>

// Net messages
#include <Doremi/Core/Include/Network/NetMessages.hpp>
#include <Doremi/Core/Include/Network/NetworkMessagesServer.hpp>

// Connections
#include <Doremi/Core/Include/Network/NetworkConnectionsServer.hpp>

// Standard
#include <iostream> // TODOCM remove after test
#include <vector>
#include <algorithm>
#include <time.h>

namespace Doremi
{
    namespace Core
    {
        NetworkManagerServer::NetworkManagerServer(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "ServerNetworkManager"),
              m_nextUpdateTimer(0.0f),
              m_updateInterval(0.017f),
              m_timeoutInterval(3.0f),
              m_maxConnection(16),
              m_nextSnapshotSequence(0),
              m_maxConnectMessagesPerFrame(20),
              m_maxConnectedMessagesPerFrame(10),
        {
            // Startup network messages, TODOCM could change position of this
            NetworkMessagesServer::StartupNetworkMessagesServer(p_sharedContext);

            DoremiEngine::Network::NetworkModule& NetworkModule = p_sharedContext.GetNetworkModule();

            // Create adress for ALL incomming IP and port 5050
            DoremiEngine::Network::Adress* UnreliableAdress = NetworkModule.CreateAdress(5050);

            // Create adress for ALL incomming IP and port 4050
            DoremiEngine::Network::Adress* ReliableAdress = NetworkModule.CreateAdress(4050);

            // Create socket for unrealiable
            m_unreliableSocketHandle = NetworkModule.CreateUnreliableWaitingSocket(UnreliableAdress);

            // Create socket for relialbe
            m_reliableSocketHandle = NetworkModule.CreateReliableConnection(ReliableAdress, m_maxConnection);

            counter = 0;

            srand(time(NULL));
        }

        NetworkManagerServer::~NetworkManagerServer() {}

        void NetworkManagerServer::Update(double p_dt)
        {
            // Receive Messages
            ReceiveMessages();

            // Send Messages
            SendMessages();

            // Check for connections TODOCM not sure if I want this if we going for UDP only
            CheckForConnections();

            // Update timeouts
            UpdateTimeouts(p_dt);
        }

        void NetworkManagerServer::ReceiveMessages()
        {
            // For some incomming connecting recieved messages we send one
            ReceiveConnectingMessages();

            // Recieve connecting messages from connected clients
            ReceiveConnectedMessages();
        }

        void NetworkManagerServer::ReceiveConnectingMessages()
        {
            // Get Nework module
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();

            // Create adress we can use, we don't know the incomming adress before we receive message
            // We delete this end of function
            DoremiEngine::Network::Adress* t_incommingAdress = t_networkModule.CreateAdress();

            // Create buffer NetworkMessage
            NetMessageBuffer t_networkMessage = NetMessageBuffer();

            // How much data we received
            uint32_t t_dataSizeReceived = 0;

            // Check for incomming messages
            size_t t_NumOfMessagesReceived = 0;
            while(t_networkModule.RecieveUnreliableData(&t_networkMessage, sizeof(t_networkMessage), m_unreliableSocketHandle, t_incommingAdress, t_dataSizeReceived) &&
                  t_NumOfMessagesReceived < m_maxConnectingMessagesPerFrame)
            {
                // Increment number of messages received
                t_NumOfMessagesReceived++;

                // If we don't have of that size
                if(t_dataSizeReceived != sizeof(NetMessageConnectingFromClient))
                {
                    // Null message and conitinue
                    t_networkMessage = NetMessageBuffer();
                    continue;
                }

                std::cout << "Recieved unreliable messsage: "; // TODOCM logg instead
                NetMessageConnectingFromClient& t_netMessageConnecting = *reinterpret_cast<NetMessageConnectingFromClient*>(&t_networkMessage);

                // Switch on what kind of message
                switch(t_netMessageConnecting.MessageID)
                {
                    case SendMessageIDFromClient::CONNECTION_REQUEST:
                    {
                        std::cout << "Connection Request." << std::endl; // TODOCM logg instead
                        NetworkMessagesServer::GetInstance()->ReceiveConnectionRequest(t_netMessageConnecting, *t_incommingAdress);

                        break;
                    }
                    case SendMessageIDFromClient::VERSION_CHECK:
                    {
                        std::cout << "Version Check" << std::endl; // TODOCM logg instead
                        NetworkMessagesServer::GetInstance()->ReceiveVersionCheck(t_netMessageConnecting, *t_incommingAdress);

                        break;
                    }
                    case SendMessageIDFromClient::DISCONNECT:
                    {
                        std::cout << "Disconnect" << std::endl; // TODOCM logg instead
                        NetworkMessagesServer::GetInstance()->ReceiveDisconnect(t_netMessageConnecting, *t_incommingAdress);

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                // Reset message
                t_networkMessage = NetMessageBuffer();
            }

            // Delete the adress holder
            delete t_incommingAdress;
        }

        void NetworkManagerServer::ReceiveConnectedMessages()
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();

            // For each connection
            auto& t_connectedClientConnections = NetworkConnectionsServer::GetInstance()->GetConnectedClientConnections();
            for(auto& t_connection : t_connectedClientConnections)
            {
                // Create message
                NetMessageBuffer t_message = NetMessageBuffer();

                // Max message counter
                uint32_t t_messageCounter = 0;

                // If we're connected
                if(t_connection.second->ConnectionState >= ClientConnectionStateFromServer::CONNECTED)
                {
                    // While we have something to receive and still less then max messages per frame
                    while(t_networkModule.RecieveReliableData(&t_message, sizeof(t_message), t_connection.second->ConnectedSocketHandle) &&
                          t_messageCounter < m_maxConnectedMessagesPerFrame)
                    {
                        NetMessageConnectedFromClient& t_connectedMessage = *reinterpret_cast<NetMessageConnectedFromClient*>(&t_message);

                        // Interpet message based on type
                        switch(t_connectedMessage.MessageID)
                        {
                            case SendMessageIDFromClient::CONNECTED:
                            {
                                NetworkMessagesServer::GetInstance()->ReceiveConnectedMessage(t_connectedMessage, t_connection.second);

                                break;
                            }
                            case SendMessageIDFromClient::LOAD_WORLD:
                            {
                                NetworkMessagesServer::GetInstance()->ReceiveLoadWorldMessage(t_connectedMessage, t_connection.second);

                                break;
                            }
                            case SendMessageIDFromClient::IN_GAME:
                            {
                                NetworkMessagesServer::GetInstance()->ReceiveInGameMessage(t_connectedMessage, t_connection.second);

                                break;
                            }
                            default:
                            {
                                break;
                            }
                        }

                        // Reset ping timer
                        t_connection.second->LastResponse = 0;

                        // Reset message
                        t_message = NetMessageBuffer();
                    }
                }
            }
        }

        void NetworkManagerServer::UpdateTimeouts(double t_dt)
        {
            // Check all connected connections
            {
                auto& t_connectedConnections = NetworkConnectionsServer::GetInstance()->GetConnectedClientConnections();
                auto t_connection = t_connectedConnections.begin();

                while(t_connection != t_connectedConnections.end())
                {
                    // Update timer
                    t_connection->second->LastResponse += t_dt;

                    // If exceed timout
                    if(t_connection->second->LastResponse >= m_timeoutInterval)
                    {
                        // Send disconnection message
                        NetworkMessagesServer::GetInstance()->SendDisconnect(*t_connection->first, "Timeout");

                        // Remove socket
                        m_sharedContext.GetNetworkModule().DeleteSocket(iter->second->ReliableSocketHandle);

                        // Remove and save player if it exists, it should
                        static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->RemoveAndSavePlayer(iter->second->PlayerID);

                        // Delete the memory here
                        delete t_connection->first;
                        delete t_connection->second;

                        // Erase from map
                        t_connection = t_connectedConnections.erase(t_connection);
                    }
                    else
                    {
                        // Else just increment
                        t_connection++;
                    }
                }
            }

            // Check all connecting connections
            {
                auto& t_connectingConnections = NetworkConnectionsServer::GetInstance()->GetConnectingClientConnections();
                auto t_connection = t_connectingConnections.begin();

                while(t_connection != t_connectingConnections.end())
                {
                    // Update timer
                    t_connection->second->LastResponse += t_dt;

                    // If exceed timout
                    if(t_connection->second->LastResponse >= m_timeoutInterval)
                    {
                        // Send disconnection message
                        NetworkMessagesServer::GetInstance()->SendDisconnect(*t_connection->first, "Timeout");

                        // Remove socket
                        m_sharedContext.GetNetworkModule().DeleteSocket(iter->second->ReliableSocketHandle);

                        // Delete the memory here
                        delete t_connection->first;
                        delete t_connection->second;

                        // Erase from map
                        t_connection = t_connectingConnections.erase(t_connection);
                    }
                    else
                    {
                        // Else just increment
                        t_connection++;
                    }
                }
            }
        }
    }
}