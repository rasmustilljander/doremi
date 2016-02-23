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
                        t_netMessages->

                            break;
                    }
                    case SendMessageIDFromServer::CONNECT:
                    {
                        break;
                    }
                    case SendMessageIDFromServer::DISCONNECT:
                    {
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


        void ClientNetworkManager::RecieveUnreliable(double p_dt)
        {
            // If we're actually trying to connect to something
            if(m_serverConnectionState > ConnectionState::DISCONNECTED)
            {
                DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();
                NetMessage Message = NetMessage();

                // See if we have any messages
                while(NetworkModule.RecieveUnreliableData(&Message, sizeof(Message), m_serverUnreliableSocketHandle, m_unreliableServerAdress))
                {
                    std::cout << "Recieved unreliable messsage: "; // TODOCM logg instead

                    switch(Message.MessageID)
                    {
                        case MessageID::VERSION_CHECK:

                            std::cout << "Version Check." << std::endl;
                            ; // TODOCM logg instead

                            // If we're connecting we set our state to version checking
                            if(m_serverConnectionState == ConnectionState::CONNECTING)
                            {
                                m_serverConnectionState = ConnectionState::VERSION_CHECK;
                                m_serverLastResponse = 0;
                            }

                            break;
                        case MessageID::CONNECT:

                            std::cout << "Connect." << std::endl;
                            ; // TODOCM logg instead

                            // If our state was version checking we get info about how to set up reliable connection
                            if(m_serverConnectionState == ConnectionState::VERSION_CHECK)
                            {
                                m_serverConnectionState = ConnectionState::CONNECTED;

                                // Get the player ID
                                NetworkStreamer Streamer = NetworkStreamer();
                                unsigned char* DataPointer = Message.Data;
                                Streamer.SetTargetBuffer(DataPointer, sizeof(Message.Data));

                                // Get playerID from server
                                uint32_t t_lastPlayerID = m_playerID;
                                m_playerID = Streamer.ReadUnsignedInt32();

                                // If it changed, we want to update config file for now
                                // TODO change not to write to config file all times we get new value?
                                if(t_lastPlayerID != m_playerID)
                                {
                                    // Update the value
                                    m_sharedContext.GetConfigurationModule().GetModifiableConfigurationInfo().LastServerPlayerID = m_playerID;

                                    // TODOXX if we change config file name, this will crash like hell
                                    m_sharedContext.GetConfigurationModule().WriteConfigurationValuesToFile("Configuration.txt");
                                    cout << "Writing new playerID to file.." << m_playerID << endl;
                                }

                                // Increase the rate of which we send messages
                                m_updateInterval = 0.017f;

                                m_serverLastResponse = 0;

                                // TODOCM add code for actuall port here
                                // TODOCM check we dont get in lock here if wrong port

                                // Connect to server
                                m_serverReliableSocketHandle = NetworkModule.ConnectToReliable(m_reliableServerAdress);

                                // TODOCM check if we need to save this adress or we can just bind it
                                // delete m_reliableServerAdress;
                            }

                            break;
                        case MessageID::DISCONNECT:

                            std::cout << "Disconnect." << std::endl;
                            ; // TODOCM logg instead

                            // TODOCM add if we're disconnected already we skip connecting back

                            // Something went wrong or we disconnected
                            m_serverConnectionState = ConnectionState::CONNECTING;
                            m_serverLastResponse = 0;
                            m_updateInterval = 1.0f;

                            // TODOCM remove world

                            break;
                        default:
                            break;
                    }
                }
            }
        }

        void ClientNetworkManager::SendMessages(double p_dt)
        {
            // Update DeltaTime
            m_nextUpdateTimer += p_dt;


            if(m_nextUpdateTimer >= m_updateInterval)
            {
                m_nextUpdateTimer -= m_updateInterval;

                // Based on our state we send different message
                switch(m_serverConnectionState)
                {
                    case ConnectionState::DISCONNECTED:

                        // Nothing yet
                        break;

                    case ConnectionState::CONNECTING:

                        // Send a connecting message
                        SendConnectRequestMessage();
                        break;

                    case ConnectionState::VERSION_CHECK:

                        // Send a version message
                        SendVersionMessage();
                        break;

                    default:
                        break;
                }
            }


            // Always send if were connected
            switch(m_serverConnectionState)
            {
                case ConnectionState::CONNECTED:

                    // TODOCM Send ping to keep an update with server?
                    SendConnectedMessage();
                    break;

                case ConnectionState::MAP_LOADING:

                    SendMapLoadingMessage();
                    break;

                case ConnectionState::IN_GAME:

                    SendInGameMessage();
                    break;
                default:
                    break;
            }
        }


        void ClientNetworkManager::UpdateTimeouts(double p_dt)
        {
            if(m_serverConnectionState > ConnectionState::DISCONNECTED)
            {
                // Update timer
                m_serverLastResponse += p_dt;

                // Check if exceeded timeout
                if(m_serverLastResponse > m_timeoutInterval)
                {
                    // Set state as disconnected
                    // TODOCM change this to DISCONNECT perhaps
                    m_serverConnectionState = ConnectionState::CONNECTING;

                    // Send disconnection message to server for good measure
                    SendDisconnectMessage();

                    // TODOCM send disconnection event to game
                }
            }
        }
    }
}