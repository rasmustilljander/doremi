// Project specific
#include <Doremi/Core/Include/Network/NetworkManagerMaster.hpp>

// Modules
#include <DoremiEngine/Network/Include/NetworkModule.hpp>

// Streamer
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>

// Handlers
#include <Doremi/Core/Include/PlayerHandlerServer.hpp>
#include <Doremi/Core/Include/InputHandlerServer.hpp>

// Net messages
#include <Doremi/Core/Include/Network/NetMessages.hpp>
#include <Doremi/Core/Include/Network/NetworkMessagesMaster.hpp>

// Connections
#include <Doremi/Core/Include/Network/NetworkConnectionsMaster.hpp>

// Standard
#include <iostream> // TODOCM remove after test
#include <vector>
#include <algorithm>
#include <time.h>

namespace Doremi
{
    namespace Core
    {
        NetworkManagerMaster::NetworkManagerMaster(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "NetworkManagerMaster"), m_timeoutInterval(5), m_maxClientMessagesPerFrame(10), m_maxServerMessagesPerFrame(10)
        {
            NetworkConnectionsMaster::StartupNetworkConnectionsMaster(p_sharedContext);
            NetworkMessagesMaster::StartupNetworkMessagesMaster(p_sharedContext);
        }

        NetworkManagerMaster::~NetworkManagerMaster() {}

        void NetworkManagerMaster::Update(double p_dt)
        {
            // When we receive we send
            ReceiveMessages();

            UpdateTimeouts(p_dt);
        }

        void NetworkManagerMaster::ReceiveMessages()
        {
            // Receive messages from clients
            ReceiveMessagesClients();

            // Receive messages from servers
            ReceiveMessagesServers();
        }

        void NetworkManagerMaster::ReceiveMessagesClients()
        {
            // Get Nework module
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();

            // Get message class
            NetworkMessagesMaster* t_netMessages = NetworkMessagesMaster::GetInstance();

            // Create adress we can use, we don't know the incomming adress before we receive message
            DoremiEngine::Network::Adress* t_incommingAdress = t_networkModule.CreateAdress();

            // Get the socket used for connecting
            SocketHandle t_socketHandle = NetworkConnectionsMaster::GetInstance()->GetSocketHandleForClients();

            // Create buffer NetworkMessage
            NetMessageMasterClientFromClient t_networkMessage = NetMessageMasterClientFromClient();

            // How much data we received
            uint32_t t_dataSizeReceived = 0;

            // Check for incomming messages
            size_t t_NumOfMessagesReceived = 0;
            while(t_networkModule.ReceiveUnreliableData(&t_networkMessage, sizeof(t_networkMessage), t_socketHandle, t_incommingAdress, t_dataSizeReceived) &&
                  ++t_NumOfMessagesReceived < m_maxClientMessagesPerFrame)
            {
                // If we don't have of that size
                if(t_dataSizeReceived != sizeof(NetMessageMasterClientFromClient))
                {
                    // Null message and conitinue
                    t_networkMessage = NetMessageMasterClientFromClient();
                    continue;
                }


                // Switch on what kind of message
                switch (t_networkMessage.MessageID)
                {
                    case SendMessageIDToMasterFromClient::CONNECTION_REQUEST:
                    {
                        std::cout << "Connection Request." << std::endl; // TODOCM logg instead
                        t_netMessages->ReceiveConnectionRequestClient(t_networkMessage, *t_incommingAdress);

                        break;
                    }
                    case SendMessageIDToMasterFromClient::CONNECTED:
                    {
                        t_netMessages->ReceiveConnectedClient(t_networkMessage, *t_incommingAdress);

                        break;
                    }
                    case SendMessageIDToMasterFromClient::DISCONNECT:
                    {
                        std::cout << "Disconnect" << std::endl; // TODOCM logg instead
                        t_netMessages->ReceiveDisconnectClient(t_networkMessage, *t_incommingAdress);

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                // Reset message
                t_networkMessage = NetMessageMasterClientFromClient();
            }

            // Delete the adress holder
            delete t_incommingAdress;
        }

        void NetworkManagerMaster::ReceiveMessagesServers()
        {
            // Get Nework module
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();

            // Get message class
            NetworkMessagesMaster* t_netMessages = NetworkMessagesMaster::GetInstance();

            // Create adress we can use, we don't know the incomming adress before we receive message
            DoremiEngine::Network::Adress* t_incommingAdress = t_networkModule.CreateAdress();

            // Get the socket used for connecting
            SocketHandle t_socketHandle = NetworkConnectionsMaster::GetInstance()->GetSocketHandleForServers();

            // Create buffer NetworkMessage
            NetMessageMasterServerFromServer t_networkMessage = NetMessageMasterServerFromServer();

            // How much data we received
            uint32_t t_dataSizeReceived = 0;

            // Check for incomming messages
            size_t t_NumOfMessagesReceived = 0;
            while(t_networkModule.ReceiveUnreliableData(&t_networkMessage, sizeof(t_networkMessage), t_socketHandle, t_incommingAdress, t_dataSizeReceived) &&
                  ++t_NumOfMessagesReceived < m_maxClientMessagesPerFrame)
            {
                // If we don't have of that size
                if(t_dataSizeReceived != sizeof(NetMessageMasterServerFromServer))
                {
                    // Null message and conitinue
                    t_networkMessage = NetMessageMasterServerFromServer();
                    continue;
                }


                // Switch on what kind of message
                switch (t_networkMessage.MessageID)
                {
                    case SendMessageIDToMasterFromServer::CONNECTION_REQUEST:
                    {
                        std::cout << "Connection Request." << std::endl; // TODOCM logg instead
                        t_netMessages->ReceiveConnectionRequestServer(t_networkMessage, *t_incommingAdress);

                        break;
                    }
                    case SendMessageIDToMasterFromServer::CONNECTED:
                    {
                        t_netMessages->ReceiveConnectedServer(t_networkMessage, *t_incommingAdress);

                        break;
                    }
                    case SendMessageIDToMasterFromServer::DISCONNECT:
                    {
                        std::cout << "Disconnect" << std::endl; // TODOCM logg instead
                        t_netMessages->ReceiveDisconnectServer(t_networkMessage, *t_incommingAdress);

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                // Reset message
                t_networkMessage = NetMessageMasterServerFromServer();
            }

            // Delete the adress holder
            delete t_incommingAdress;
        }

        void NetworkManagerMaster::UpdateTimeouts(double p_dt)
        {
            // Check all clients
            UpdateTimeoutsClients(p_dt);

            // Check all servers
            UpdateTimeoutsServers(p_dt);
        }

        void NetworkManagerMaster::UpdateTimeoutsClients(double p_dt)
        {
            NetworkConnectionsMaster* t_connections = NetworkConnectionsMaster::GetInstance();
            NetworkMessagesMaster* t_messages = NetworkMessagesMaster::GetInstance();
            
            // Check all clients
            auto& t_clientConnections = t_connections->GetClientConnections();
            auto t_connection = t_clientConnections.begin();

            while (t_connection != t_clientConnections.end())
            {
                // Increment last response
                t_connection->second->LastResponse += p_dt;

                if (t_connection->second->LastResponse > m_timeoutInterval)
                {
                    std::cout << "Timeout client: " << t_connection->second->LastResponse << " seconds." << std::endl;

                    // Send disconnection message
                    t_messages->SendDisconnectClient(*t_connection->first);

                    // Erase from map
                    t_connection = t_clientConnections.erase(t_connection);
                }
                else
                {
                    ++t_connection;
                }
            }
        }

        void NetworkManagerMaster::UpdateTimeoutsServers(double p_dt)
        {
            NetworkConnectionsMaster* t_connections = NetworkConnectionsMaster::GetInstance();
            NetworkMessagesMaster* t_messages = NetworkMessagesMaster::GetInstance();

            // Check all clients
            auto& t_serverConnections = t_connections->GetServerConnections();
            auto t_connection = t_serverConnections.begin();

            while (t_connection != t_serverConnections.end())
            {
                // Increment last response
                t_connection->second->LastResponse += p_dt;
                t_connection->second->SequenceLastResponse += p_dt;

                if (t_connection->second->LastResponse > m_timeoutInterval)
                {
                    std::cout << "Timeout server: " << t_connection->second->LastResponse << " seconds." << std::endl;

                    // Send disconnection message
                    t_messages->SendDisconnectServer(*t_connection->first);

                    // Erase from map
                    t_connection = t_serverConnections.erase(t_connection);
                }
                else
                {
                    ++t_connection;
                }
            }
        }
    }
}