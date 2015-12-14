// Project specific
#include <Manager/Network/ClientNetworkManager.hpp>
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <Manager/Network/NetMessage.hpp>
#include <Manager/Network/BitWriter.h>
#include <iostream> // TODOCM remove after test

namespace Doremi
{
    namespace Core
    {
        ClientNetworkManager::ClientNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext),
              m_masterConnectionState(ConnectionState::DISCONNECTED),
              m_serverConnectionState(ConnectionState::DISCONNECTED),
              m_nextUpdateTimer(0.0f),
              m_updateInterval(1.0f),
              m_timeoutInterval(5.0f)
        {
            m_serverAdress = p_sharedContext.GetNetworkModule().CreateAdress(127, 0, 0, 1, 5050); // TODOCM remove test code
            m_serverUnreliableSocketHandle = p_sharedContext.GetNetworkModule().CreateUnreliableSocket();

            NetMessage Message = NetMessage();
            Message.MessageID = MessageID::CONNECT_REQUEST;
            p_sharedContext.GetNetworkModule().SendUnreliableData(&Message, sizeof(Message), m_serverUnreliableSocketHandle, m_serverAdress);
        }

        ClientNetworkManager::~ClientNetworkManager() {}

        void ClientNetworkManager::Update(double p_dt)
        {
            // Recieve Messages
            RecieveMessages(p_dt);

            // Send Messages
            SendMessages(p_dt);

            // Check for timed out connections
            UpdateTimeouts(p_dt);
        }

        void ClientNetworkManager::RecieveMessages(double p_dt)
        {
            // For all incomming recieved unreliable messages we will prorobly change state
            RecieveUnreliable(p_dt);

            // For all icomming recieved reliable messages we will probobly get snapshot
            RecieveReliable(p_dt);
        }

        void ClientNetworkManager::RecieveUnreliable(double p_dt)
        {
            // If we're actually trying to connect to something
            if(m_serverConnectionState > ConnectionState::DISCONNECTED)
            {
                DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();
                NetMessage Message = NetMessage();

                // See if we have any messages
                while(NetworkModule.RecieveUnreliableData(&Message, sizeof(Message), m_serverUnreliableSocketHandle, m_serverAdress))
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

                                // Increase the rate of which we send messages
                                m_updateInterval = 17.0f;

                                m_serverLastResponse = 0;

                                // TODOCM add code for actuall port here
                                // TODOCM check we dont get in lock here if wrong port
                                DoremiEngine::Network::Adress* ReliableAdress = NetworkModule.CreateAdress(127, 0, 0, 1, 4050);

                                // Connect to server
                                m_serverReliableSocketHandle = NetworkModule.ConnectToReliable(ReliableAdress);

                                // TODOCM check if we need to save this adress or we can just bind it
                                delete ReliableAdress;
                            }

                            break;
                        case MessageID::DISCONNECT:

                            std::cout << "Disconnect." << std::endl;
                            ; // TODOCM logg instead

                            // Something went wrong or we disconnected
                            m_serverConnectionState = ConnectionState::CONNECTING;
                            m_serverLastResponse = 0;
                            m_updateInterval = 1000.0f;

                            break;
                        default:
                            break;
                    }
                }
            }
        }

        void ClientNetworkManager::RecieveReliable(double p_dt)
        {
            if(m_serverConnectionState == ConnectionState::CONNECTED)
            {
                DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();
                NetMessage Message = NetMessage();

                // Attempt recieve reliable message
                while(NetworkModule.RecieveReliableData(&Message, sizeof(Message), m_serverReliableSocketHandle))
                {
                    std::cout << "Recieved snapshot." << std::endl; // TODOCM logg instead

                    // Update last response
                    m_serverLastResponse = 0;
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
                    case ConnectionState::CONNECTED:

                        // TODOCM Send ping to keep an update with server?
                        SendConnectedMessage();

                        break;
                    default:
                        break;
                }
            }
        }

        void ClientNetworkManager::SendConnectRequestMessage()
        {
            // Create a message
            NetMessage Message = NetMessage();

            // Set message ID to connection request
            Message.MessageID = MessageID::CONNECT_REQUEST;

            // TODOCM Write bits for stuff

            std::cout << "Sending Connect request message." << std::endl; // TODOCM logg instead

            // Send Message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&Message, sizeof(Message), m_serverUnreliableSocketHandle, m_serverAdress);
        }

        void ClientNetworkManager::SendVersionMessage()
        {
            // Create a message
            NetMessage Message = NetMessage();

            // Set message ID to version check
            Message.MessageID = MessageID::VERSION_CHECK;

            // TODOCM Write bits for stuff

            std::cout << "Sending version message." << std::endl; // TODOCM logg instead

            // Send Message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&Message, sizeof(Message), m_serverUnreliableSocketHandle, m_serverAdress);
        }

        void ClientNetworkManager::SendConnectedMessage()
        {
            // Create a message
            NetMessage Message = NetMessage();

            // TODOCM write bits for stuff

            std::cout << "Sending connected message" << std::endl; // TODOCM logg instead

            // Send Message
            m_sharedContext.GetNetworkModule().SendReliableData(&Message, sizeof(Message), m_serverReliableSocketHandle);
        }

        void ClientNetworkManager::SendDisconnectMessage()
        {
            // Create disconnection message
            NetMessage NewMessage = NetMessage();
            NewMessage.MessageID = MessageID::DISCONNECT;

            // TODOCM add info

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&NewMessage, sizeof(NewMessage), m_serverUnreliableSocketHandle, m_serverAdress);
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