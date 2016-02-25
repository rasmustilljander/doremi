// Project specific
#include <Doremi/Core/Include/Network/NetworkManagerServer.hpp>

// Modules
#include <DoremiEngine/Network/Include/NetworkModule.hpp>

// Streamer
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>

// Handlers
#include <Doremi/Core/Include/PlayerHandlerServer.hpp>
#include <Doremi/Core/Include/InputHandlerServer.hpp>

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
              m_timeoutInterval(20.0f),
              m_maxConnectedMessagesPerFrame(20),
              m_maxConnectingMessagesPerFrame(10),
              m_maxAcceptConnectionsPerFrame(5)
        {
            // Startup network messages and connection, TODOCM could change position of this
            NetworkMessagesServer::StartupNetworkMessagesServer(p_sharedContext);
            NetworkConnectionsServer::StartupNetworkConnectionsClient(p_sharedContext);

            srand(time(NULL));
        }

        NetworkManagerServer::~NetworkManagerServer() {}

        void NetworkManagerServer::Update(double p_dt)
        {
            // Receive Messages
            ReceiveMessages();

            // Send Messages for connected clients
            SendConnectedMessages();

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

            // Get message class
            NetworkMessagesServer* t_netMessages = NetworkMessagesServer::GetInstance();

            // Create adress we can use, we don't know the incomming adress before we receive message
            DoremiEngine::Network::Adress* t_incommingAdress = t_networkModule.CreateAdress();

            // Get the socket used for connecting
            SocketHandle t_connectingSocketHandle = NetworkConnectionsServer::GetInstance()->GetConnectingSocketHandle();

            // Create buffer NetworkMessage
            NetMessageConnectingFromClient t_networkMessage = NetMessageConnectingFromClient();

            // How much data we received
            uint32_t t_dataSizeReceived = 0;

            // Check for incomming messages
            size_t t_NumOfMessagesReceived = 0;
            while(t_networkModule.RecieveUnreliableData(&t_networkMessage, sizeof(t_networkMessage), t_connectingSocketHandle, t_incommingAdress, t_dataSizeReceived) &&
                  ++t_NumOfMessagesReceived < m_maxConnectingMessagesPerFrame)
            {
                // If we don't have of that size
                if(t_dataSizeReceived != sizeof(NetMessageConnectingFromClient))
                {
                    // Null message and conitinue
                    t_networkMessage = NetMessageConnectingFromClient();
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
                        t_netMessages->ReceiveConnectionRequest(t_netMessageConnecting, *t_incommingAdress);

                        break;
                    }
                    case SendMessageIDFromClient::VERSION_CHECK:
                    {
                        std::cout << "Version Check" << std::endl; // TODOCM logg instead
                        t_netMessages->ReceiveVersionCheck(t_netMessageConnecting, *t_incommingAdress);

                        break;
                    }
                    case SendMessageIDFromClient::DISCONNECT:
                    {
                        std::cout << "Disconnect" << std::endl; // TODOCM logg instead
                        t_netMessages->ReceiveDisconnect(t_netMessageConnecting, *t_incommingAdress);

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                // Reset message
                t_networkMessage = NetMessageConnectingFromClient();
            }

            // Delete the adress holder
            delete t_incommingAdress;
        }

        void NetworkManagerServer::ReceiveConnectedMessages()
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();
            NetworkMessagesServer* t_netMessages = NetworkMessagesServer::GetInstance();

            // For each connection
            auto& t_connectedClientConnections = NetworkConnectionsServer::GetInstance()->GetConnectedClientConnections();
            for(auto& t_connection : t_connectedClientConnections)
            {
                // Create message
                NetMessageBuffer t_message = NetMessageBuffer();

                // Max message counter
                uint32_t t_messageCounter = 0;

                // To check how much we received
                uint32_t t_dataSizeReceived = 0;

                // If we're connected
                if(t_connection.second->ConnectionState >= ClientConnectionStateFromServer::CONNECTED)
                {
                    // While we have something to receive and still less then max messages per frame
                    while(t_networkModule.RecieveReliableData(&t_message, sizeof(t_message), t_connection.second->ConnectedSocketHandle, t_dataSizeReceived) &&
                          ++t_messageCounter < m_maxConnectedMessagesPerFrame)
                    {
                        // If we received a correct message
                        if(t_dataSizeReceived != sizeof(NetMessageConnectedFromClient))
                        {
                            t_message = NetMessageBuffer();
                            continue;
                        }

                        // Convert to correct message
                        NetMessageConnectedFromClient& t_connectedMessage = *reinterpret_cast<NetMessageConnectedFromClient*>(&t_message);

                        // Interpet message based on type
                        switch(t_connectedMessage.MessageID)
                        {
                            case SendMessageIDFromClient::CONNECTED:
                            {
                                t_netMessages->ReceiveConnectedMessage(t_connectedMessage, t_connection.second);

                                break;
                            }
                            case SendMessageIDFromClient::LOAD_WORLD:
                            {
                                t_netMessages->ReceiveLoadWorldMessage(t_connectedMessage, t_connection.second);

                                break;
                            }
                            case SendMessageIDFromClient::IN_GAME:
                            {
                                t_netMessages->ReceiveInGameMessage(t_connectedMessage, t_connection.second);

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

        void NetworkManagerServer::SendConnectedMessages()
        {
            NetworkMessagesServer::GetInstance()->UpdateSequence();
            NetworkMessagesServer* t_netMessages = NetworkMessagesServer::GetInstance();

            // Get connected clients
            auto& t_connections = NetworkConnectionsServer::GetInstance()->GetConnectedClientConnections();

            // std::cout << t_connections.size() << std::endl;

            // For each connected client
            for(auto& t_connection : t_connections)
            {
                // Send message based on state
                switch(t_connection.second->ConnectionState)
                {
                    case ClientConnectionStateFromServer::CONNECTED:
                    {
                        t_netMessages->SendConnected(t_connection.second);

                        break;
                    }
                    case ClientConnectionStateFromServer::LOAD_WORLD:
                    {
                        t_netMessages->SendLoadWorld(t_connection.second);

                        break;
                    }
                    case ClientConnectionStateFromServer::IN_GAME:
                    {
                        t_netMessages->SendInGame(t_connection.second);

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
        }

        void NetworkManagerServer::CheckForConnections()
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();
            NetworkConnectionsServer* t_netConnections = NetworkConnectionsServer::GetInstance();
            PlayerHandlerServer* t_playerHandler = static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance());

            // Get values
            SocketHandle t_connectedSocketHandle = NetworkConnectionsServer::GetInstance()->GetConnectedSocketHandle();
            SocketHandle t_outSocketHandle = 0;
            DoremiEngine::Network::Adress* t_outAdress = t_networkModule.CreateAdress();
            DoremiEngine::Network::Adress* t_savedAdress = t_networkModule.CreateAdress();

            // Holder for the connection we want to fetch
            std::pair<DoremiEngine::Network::Adress*, ClientConnectionFromServer*> t_connection;

            // Counter to not get stuck
            uint32_t t_connectCounter = 0;

            // Try to accept connections
            while(t_networkModule.AcceptConnection(t_connectedSocketHandle, t_outSocketHandle, t_outAdress) && ++t_connectCounter < m_maxAcceptConnectionsPerFrame)
            {
                // If we have a pending connection
                if(t_netConnections->AdressExistInConnecting(*t_outAdress, t_connection))
                {
                    // If the connection is in connect mode
                    if(t_connection.second->ConnectionState == ClientConnectionStateFromServer::CONNECT)
                    {
                        std::cout << "Accepting connection" << std::endl;

                        // Update connection
                        t_connection.second->ConnectionState = ClientConnectionStateFromServer::CONNECTED;

                        // Update last response
                        t_connection.second->LastResponse = 0;

                        // Add socketID
                        t_connection.second->ConnectedSocketHandle = t_outSocketHandle;

                        // New connection... TODOCM CHECK THIS........
                        t_connection.second->NewConnection = true;

                        // Create new InputHandler
                        InputHandlerServer* t_newInputHandler = new InputHandlerServer(m_sharedContext, DirectX::XMFLOAT3(0, 0, 0));

                        // Create player
                        t_playerHandler->CreateNewPlayer(t_connection.second->PlayerID, t_newInputHandler);

                        // Upgrade connection to connected
                        t_netConnections->SetConnecting(t_connection);
                    }
                    else
                    {
                        // If we're not in the connect stage, its bad pattern
                        // TODOXX This will trigger if there are two on same IP joining at same time, will crash hard
                        NetworkMessagesServer::GetInstance()->SendDisconnect(*t_outAdress, "Bad pattern: Error in connect");
                    }
                }
                else
                {
                    // If we're not in the connect stage, its bad pattern
                    NetworkMessagesServer::GetInstance()->SendDisconnect(*t_outAdress, "Bad pattern: Multiple connect attempts");
                }
            }

            // Delete the adress
            delete t_outAdress;
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
                        m_sharedContext.GetNetworkModule().DeleteSocket(t_connection->second->ConnectedSocketHandle);

                        // Remove and save player if it exists, it should
                        static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->RemoveAndSavePlayer(t_connection->second->PlayerID);

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
                        m_sharedContext.GetNetworkModule().DeleteSocket(t_connection->second->ConnectedSocketHandle);

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