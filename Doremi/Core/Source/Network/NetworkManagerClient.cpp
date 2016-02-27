// Project specific
#include <Doremi/Core/Include/Network/NetworkManagerClient.hpp>

// Engine
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <DoremiEngine/Configuration/Include/ConfigurationModule.hpp>

// Connections
#include <Doremi/Core/Include/Network/NetworkConnectionsClient.hpp>

// Network messages
#include <Doremi/Core/Include/Network/NetMessages.hpp>
#include <Doremi/Core/Include/Network/NetworkMessagesClient.hpp>

// Events
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/ChangeMenuState.hpp>

#include <iostream> // TODOCM remove only debug

namespace Doremi
{
    namespace Core
    {
        NetworkManagerClient::NetworkManagerClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "ClientNetworkManager"),
              m_nextUpdateTimer(0.0f),
              m_updateInterval(1.0f),
              m_timeoutIntervalConnecting(5.0f),
              m_timeoutIntervalConnected(1.0f),
              m_maxConnectingMessagesPerFrame(10),
              m_maxConnectedMessagesPerFrame(10)

        {
            // Subscribe to events
            EventHandler::GetInstance()->Subscribe(EventType::ChangeMenuState, this);

            // Startup network messages and connection, TODOCM could change position of this
            NetworkMessagesClient::StartupNetworkMessagesClient(p_sharedContext);
            NetworkConnectionsClient::StartupNetworkConnectionsClient(p_sharedContext);

            LoadConfigFile(p_sharedContext);
        }

        NetworkManagerClient::~NetworkManagerClient() {}

        void NetworkManagerClient::LoadConfigFile(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = p_sharedContext.GetNetworkModule();
            DoremiEngine::Configuration::ConfiguartionInfo t_configInfo = p_sharedContext.GetConfigurationModule().GetAllConfigurationValues();
            NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();

            // Get last playerID
            t_connections->m_serverConnectionState.PlayerID = t_configInfo.LastServerPlayerID;

            // Get IP from config file
            std::string t_IPString = t_configInfo.IPToServer;

            // Create pointer to use
            char* t_IPCharPointer = new char[t_IPString.size() + 1];

            // Copy string to pointer
            t_IPString.copy(t_IPCharPointer, t_IPString.size());

            // Create int array to hold IP values
            int* t_IPArray = new int[4];

            // Pointer to part of string to check
            char* t_ToCheck;
            t_ToCheck = strtok(t_IPCharPointer, ".");

            size_t t_counter = 0;
            while(t_ToCheck != NULL && t_counter < 4)
            {
                // Save string part as int to array
                t_IPArray[t_counter] = std::stoi(std::string(t_ToCheck));
                t_ToCheck = strtok(NULL, ".");
                t_counter++;
            }

            // If we got values for all int slots we got an actual IP
            if(t_counter == 4)
            {
                // TODOCM Read port ? as well, change to master as well probobly
                t_connections->m_serverConnectionState.ConnectingAdress =
                    t_networkModule.CreateAdress(t_IPArray[0], t_IPArray[1], t_IPArray[2], t_IPArray[3], 5050); // TODOCM remove test code
                t_connections->m_serverConnectionState.ConnectedAdress =
                    t_networkModule.CreateAdress(t_IPArray[0], t_IPArray[1], t_IPArray[2], t_IPArray[3], 4050);
            }
            else
            {
                t_connections->m_serverConnectionState.ConnectingAdress = t_networkModule.CreateAdress(127, 0, 0, 1, 5050);
                t_connections->m_serverConnectionState.ConnectedAdress = t_networkModule.CreateAdress(127, 0, 0, 1, 4050);
            }

            // Create new socket for unreliable
            t_connections->m_serverConnectionState.ConnectingSocketHandle = t_networkModule.CreateUnreliableSocket();

            delete t_IPCharPointer;
            delete t_IPArray;
        }

        void NetworkManagerClient::SetServerIP(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
        {
            NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();

            // Change IP on connecting socket
            t_connections->m_serverConnectionState.ConnectingAdress->SetIP(a, b, c, d);
            t_connections->m_serverConnectionState.ConnectingAdress->ComposeAdress();

            // TODOCM remove this when we get connected socket by message
            // Change IP on connected socket
            t_connections->m_serverConnectionState.ConnectedAdress->SetIP(a, b, c, d);
            t_connections->m_serverConnectionState.ConnectedAdress->ComposeAdress();

            // Remove the old sockets
            t_networkModule.DeleteSocket(t_connections->m_serverConnectionState.ConnectingSocketHandle);
            t_networkModule.DeleteSocket(t_connections->m_serverConnectionState.ConnectedSocketHandle);

            // Create new socket for unreliable
            t_connections->m_serverConnectionState.ConnectingSocketHandle = t_networkModule.CreateUnreliableSocket();
        }

        void NetworkManagerClient::Update(double p_dt)
        {
            // Recieve Messages
            ReceiveMessages();

            // Send Messages
            SendMessages(p_dt);

            // Check for timed out connections
            UpdateTimeouts(p_dt);
        }

        void NetworkManagerClient::ReceiveMessages()
        {
            // Receive connecting messages
            ReceiveConnectingMessages();

            // Receive conneted messages
            ReceiveConnectedMessages();
        }

        void NetworkManagerClient::ReceiveConnectingMessages()
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();
            NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();

            // Get message class
            NetworkMessagesClient* t_netMessages = NetworkMessagesClient::GetInstance();

            // Get connecting socket
            SocketHandle t_serverConnectingSocketHandle = t_connections->m_serverConnectionState.ConnectingSocketHandle;

            // Create buffer message
            NetMessageConnectingFromServer t_newMessage = NetMessageConnectingFromServer();

            // To check how much we received
            uint32_t t_dataSizeReceived = 0;

            // Counter for checking we dont read to much
            uint32_t t_numOfMessages = 0;

            // Receive messages
            // TODOCM not sure if need to send in out adress here
            while(t_networkModule.RecieveUnreliableData(&t_newMessage, sizeof(t_newMessage), t_serverConnectingSocketHandle,
                                                        t_connections->m_serverConnectionState.ConnectingAdress, t_dataSizeReceived) &&
                  ++t_numOfMessages < m_maxConnectingMessagesPerFrame)
            {
                // If wrong size of message
                if(t_dataSizeReceived != sizeof(NetMessageConnectingFromServer))
                {
                    t_newMessage = NetMessageConnectingFromServer();
                    continue;
                }

                // Convert message to proper
                NetMessageConnectingFromServer& t_messageConnecting = *reinterpret_cast<NetMessageConnectingFromServer*>(&t_newMessage);

                // Check ID and interpet
                switch(t_messageConnecting.MessageID)
                {
                    case SendMessageIDFromServer::VERSION_CHECK:
                    {
                        std::cout << "Received version check" << std::endl; // TODOCM remove deubgg
                        t_netMessages->ReceiveVersionCheck(t_messageConnecting);

                        break;
                    }
                    case SendMessageIDFromServer::CONNECT:
                    {
                        std::cout << "Received connect" << std::endl; // TODOCM remove deubgg
                        t_netMessages->ReceiveConnect(t_messageConnecting);

                        break;
                    }
                    case SendMessageIDFromServer::DISCONNECT:
                    {
                        std::cout << "Received disconnect" << std::endl; // TODOCM remove deubgg
                        t_netMessages->ReceiveDisconnect(t_messageConnecting);

                        break;
                    }
                    default:
                    {
                        std::cout << "Some error message received" << std::endl; // TODOCM remove deubgg
                        break;
                    }
                }

                // Reset message
                t_newMessage = NetMessageConnectingFromServer();
            }
        }

        void NetworkManagerClient::ReceiveConnectedMessages()
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();
            NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();
            NetworkMessagesClient* t_netMessages = NetworkMessagesClient::GetInstance();

            // TODOCM check if we need to check if we're connected

            // Create buffer message
            NetMessageBuffer t_newMessage = NetMessageBuffer();

            // To check how much we received
            uint32_t t_dataSizeReceived = 0;

            // Counter for checking we dont read to much
            uint32_t t_numOfMessages = 0;

            // Try receive messages
            while(t_networkModule.RecieveReliableData(&t_newMessage, sizeof(t_newMessage), t_connections->m_serverConnectionState.ConnectedSocketHandle, t_dataSizeReceived) &&
                  ++t_numOfMessages < m_maxConnectedMessagesPerFrame)
            {
                // If wrong size of message
                if(t_dataSizeReceived != sizeof(NetMessageConnectedFromServer))
                {
                    t_newMessage = NetMessageBuffer();
                    continue;
                }

                // Convert message to proper
                NetMessageConnectedFromServer& t_messageConnecting = *reinterpret_cast<NetMessageConnectedFromServer*>(&t_newMessage);

                // Check ID and interpet
                switch(t_messageConnecting.MessageID)
                {
                    case SendMessageIDFromServer::CONNECTED:
                    {
                        t_netMessages->ReceiveConnected(t_messageConnecting);

                        break;
                    }
                    case SendMessageIDFromServer::LOAD_WORLD:
                    {
                        t_netMessages->ReceiveLoadWorld(t_messageConnecting);

                        break;
                    }
                    case SendMessageIDFromServer::IN_GAME:
                    {
                        t_netMessages->ReceiveInGame(t_messageConnecting);

                        break;
                    }

                    default:
                        break;
                }

                // Reset message
                t_newMessage = NetMessageBuffer();
            }
        }

        void NetworkManagerClient::SendMessages(double p_dt)
        {
            // Send connecting messages if we're in those states
            SendConnectingMessages(p_dt);

            // Send connected messages if we're in those states
            SendConnectedMessages();
        }

        void NetworkManagerClient::SendConnectingMessages(double p_dt)
        {
            NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();
            NetworkMessagesClient* t_netMessages = NetworkMessagesClient::GetInstance();

            // Update timer
            m_nextUpdateTimer += p_dt;

            // If we're not connected we only send at intervals
            if(m_nextUpdateTimer >= m_updateInterval)
            {
                // Reduce timer
                m_nextUpdateTimer -= m_updateInterval;

                // Based on our state we send different message
                switch(t_connections->m_serverConnectionState.ConnectionState)
                {
                    case ServerConnectionStateFromClient::CONNECTING:
                    {
                        std::cout << "Sending connecting" << std::endl; // TODOCM remove deubgg
                        t_netMessages->SendConnectionRequest();

                        break;
                    }
                    case ServerConnectionStateFromClient::VERSION_CHECK:
                    {
                        std::cout << "Sending version check" << std::endl; // TODOCM remove deubgg
                        t_netMessages->SendVersionCheck();

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
        }

        void NetworkManagerClient::SendConnectedMessages()
        {
            NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();
            NetworkMessagesClient* t_netMessages = NetworkMessagesClient::GetInstance();

            // If we're connected we always send based on connection
            switch(t_connections->m_serverConnectionState.ConnectionState)
            {
                case ServerConnectionStateFromClient::CONNECTED:
                {
                    t_netMessages->SendConnected();

                    break;
                }
                case ServerConnectionStateFromClient::LOAD_WORLD:
                {
                    t_netMessages->SendLoadWorld();

                    break;
                }
                case ServerConnectionStateFromClient::IN_GAME:
                {
                    t_netMessages->SendInGame();

                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        void NetworkManagerClient::UpdateTimeouts(double p_dt)
        {
            NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();
            NetworkMessagesClient* t_netMessages = NetworkMessagesClient::GetInstance();

            // If we're not disconnected we want to disconnect if timeout
            if(t_connections->m_serverConnectionState.ConnectionState > ServerConnectionStateFromClient::DISCONNECTED)
            {
                // Update timer
                t_connections->m_serverConnectionState.LastResponse += p_dt;
                t_connections->m_serverConnectionState.LastSequenceUpdate += p_dt;

                // Check if exceeded timeout
                if(t_connections->m_serverConnectionState.ConnectionState >= ServerConnectionStateFromClient::CONNECTED &&
                    t_connections->m_serverConnectionState.LastResponse > m_timeoutIntervalConnected ||
                    t_connections->m_serverConnectionState.ConnectionState < ServerConnectionStateFromClient::CONNECTED &&
                    t_connections->m_serverConnectionState.LastResponse > m_timeoutIntervalConnecting)
                {
                    std::cout << "Timeout server: " << t_connections->m_serverConnectionState.LastResponse << " seconds." << std::endl;

                    // Set state as disconnected
                    t_connections->m_serverConnectionState.ConnectionState = ServerConnectionStateFromClient::DISCONNECTED;

                    // Send disconnection message to server for good measure
                    t_netMessages->SendDisconnect();

                    // TODO send change gamestate event
                }
            }
        }

        void NetworkManagerClient::OnEvent(Event* p_event)
        {
            if(p_event->eventType == EventType::ChangeMenuState)
            {
                ChangeMenuState* t_changeMenuEvent = static_cast<ChangeMenuState*>(p_event);
                NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();

                // If we are processing to run game we set us to connecting
                if(t_changeMenuEvent->state == DoremiStates::RUNGAME)
                {
                    // Change connection state
                    t_connections->m_serverConnectionState.ConnectionState = ServerConnectionStateFromClient::CONNECTING;

                    // TODOCM maybe send first message here, or check vs other event that sends IP to server or something
                }
                else
                {
                    // TODOXX if we want multiple states(in game top menu) this wont work
                    // Change connection state
                    t_connections->m_serverConnectionState.ConnectionState = ServerConnectionStateFromClient::DISCONNECTED;
                }
            }
        }
    }
}