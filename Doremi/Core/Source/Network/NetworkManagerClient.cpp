// Project specific
#include <Doremi/Core/Include/Network/NetworkManagerClient.hpp>

// Engine
#include <DoremiEngine/Network/Include/NetworkModule.hpp>

// Connections
#include <Doremi/Core/Include/Network/NetworkConnectionsClient.hpp>

// Network messages
#include <Doremi/Core/Include/Network/NetMessages.hpp>
#include <Doremi/Core/Include/Network/NetworkMessagesClient.hpp>

namespace Doremi
{
    namespace Core
    {
        NetworkManagerClient::NetworkManagerClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "ClientNetworkManager"),
              m_masterConnectionState(ConnectionState::DISCONNECTED),
              m_serverConnectionState(ConnectionState::CONNECTING),
              m_nextUpdateTimer(0.0f),
              m_updateInterval(1.0f),
              m_timeoutInterval(3.0f),
              m_numJoinEvents(0)
        {
            LoadConfigFile(p_sharedContext);

            m_serverUnreliableSocketHandle = p_sharedContext.GetNetworkModule().CreateUnreliableSocket();

            NetMessage Message = NetMessage();
            Message.MessageID = MessageID::CONNECT_REQUEST;
            p_sharedContext.GetNetworkModule().SendUnreliableData(&Message, sizeof(Message), m_serverUnreliableSocketHandle, m_unreliableServerAdress);
        }

        NetworkManagerClient::~NetworkManagerClient() {}

        void ClientNetworkManager::LoadConfigFile(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            DoremiEngine::Configuration::ConfiguartionInfo t_configInfo = p_sharedContext.GetConfigurationModule().GetAllConfigurationValues();

            // Get last playerID
            m_playerID = t_configInfo.LastServerPlayerID;

            // Get IP from config file
            std::string IPString = t_configInfo.IPToServer;
            char* IPCharPointer = new char[IPString.size() + 1];

            // We need it as a char pointer
            IPString.copy(IPCharPointer, IPString.size());

            int* IPArray = new int[4];

            char* ToCheck;

            ToCheck = strtok(IPCharPointer, ".");

            size_t counter = 0;
            while(ToCheck != NULL && counter < 4)
            {
                IPArray[counter] = std::stoi(std::string(ToCheck));
                ToCheck = strtok(NULL, ".");
                counter++;
            }

            // If we got values for all
            if(counter == 4)
            {
                m_unreliableServerAdress = p_sharedContext.GetNetworkModule().CreateAdress(IPArray[0], IPArray[1], IPArray[2], IPArray[3], 5050); // TODOCM remove test code
                m_reliableServerAdress = p_sharedContext.GetNetworkModule().CreateAdress(IPArray[0], IPArray[1], IPArray[2], IPArray[3], 4050);
            }
            else
            {
                m_unreliableServerAdress = p_sharedContext.GetNetworkModule().CreateAdress(127, 0, 0, 1, 5050); // TODOCM remove test code
                m_reliableServerAdress = p_sharedContext.GetNetworkModule().CreateAdress(127, 0, 0, 1, 4050);
            }

            delete IPCharPointer;
            delete IPArray;
        }

        void ClientNetworkManager::SetServerIP(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
        {
            // Change IP on unreliable socket
            m_unreliableServerAdress->SetIP(a, b, c, d);
            m_unreliableServerAdress->ComposeAdress();

            // Change IP on reliable socket
            m_reliableServerAdress->SetIP(a, b, c, d);
            m_reliableServerAdress->ComposeAdress();

            // Remove the old sockets
            m_sharedContext.GetNetworkModule().DeleteSocket(m_serverUnreliableSocketHandle);

            // Create new sockets
            m_serverUnreliableSocketHandle = m_sharedContext.GetNetworkModule().CreateUnreliableSocket();

            // Send intro message
            NetMessage Message = NetMessage();
            Message.MessageID = MessageID::CONNECT_REQUEST;
            m_sharedContext.GetNetworkModule().SendUnreliableData(&Message, sizeof(Message), m_serverUnreliableSocketHandle, m_unreliableServerAdress);
        }

        void NetworkManagerClient::Update(double p_dt)
        {
            // Recieve Messages
            ReceiveMessages();

            // Send Messages
            SendMessages();

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

            // Get message class
            NetworkMessagesClient* t_netMessages = NetworkMessagesClient::GetInstance();

            // Get connecting socket
            SocketHandle t_serverConnectingSocketHandle = NetworkConnectionsClient::GetInstance()->GetServerConnectingSocketHandle();

            // Create buffer message
            NetMessageBuffer t_newMessage = NetMessageBuffer();

            // To check how much we received
            uint32_t t_dataSizeReceived = 0;

            // Receive messages
            // TODOCM not sure if need to send in out adress here
            while(t_networkModule.RecieveUnreliableData(&t_newMessage, sizeof(t_newMessage), t_serverConnectingSocketHandle, t_dataSizeReceived))
            {
                // If wrong size of message
                if(t_dataSizeReceived != sizeof(NetMessageConnectingFromServer))
                {
                    t_newMessage = NetMessageBuffer();
                    continue;
                }

                // Convert message to proper
                NetMessageConnectingFromServer& t_messageConnecting = *reinterpret_cast<NetMessageConnectingFromServer*>(&t_newMessage);

                // Check ID and interpet
                switch(t_messageConnecting.MessageID)
                {
                    case SendMessageIDFromServer::VERSION_CHECK:
                    {
                        t_netMessages->ReceiveVersionCheck(t_messageConnecting);

                        break;
                    }
                    case SendMessageIDFromServer::CONNECT:
                    {
                        t_netMessages->ReceiveConnect(t_messageConnecting);

                        break;
                    }
                    case SendMessageIDFromServer::DISCONNECT:
                    {
                        t_netMessages->ReceiveDisconnect(t_messageConnecting);

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                // Reset message
                t_newMessage = NetMessageBuffer();
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

            // Try receive messages
            while(t_networkModule.RecieveReliableData(&t_newMessage, sizeof(t_newMessage), t_connections->m_serverConnectionState.ConnectedSocketHandle, t_dataSizeReceived))
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
                        t_netMessages->SendConnectionRequest();
                    }
                    case ServerConnectionStateFromClient::VERSION_CHECK:
                    {
                        t_netMessages->SendVersionCheck();
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
            switch(m_serverConnectionState)
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

                // Check if exceeded timeout
                if(t_connections->m_serverConnectionState.LastResponse > m_timeoutInterval)
                {
                    // Set state as disconnected
                    m_serverConnectionState = ServerConnectionStateFromClient::DISCONNECTED;

                    // Send disconnection message to server for good measure
                    t_netMessages->SendDisconnect();

                    // TODO send change gamestate event
                }
            }
        }
    }
}