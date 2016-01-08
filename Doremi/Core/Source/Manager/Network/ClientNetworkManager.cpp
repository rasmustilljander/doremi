// Project specific
#include <Manager/Network/ClientNetworkManager.hpp>
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <Manager/Network/NetMessage.hpp>
#include <Manager/Network/BitStreamer.h>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <InterpolationHandler.hpp>
#include <iostream> // TODOCM remove after test
#include <PlayerHandler.hpp>
#include <InputHandlerClient.hpp>


namespace Doremi
{
    namespace Core
    {
        ClientNetworkManager::ClientNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext),
              m_masterConnectionState(ConnectionState::DISCONNECTED),
              m_serverConnectionState(ConnectionState::CONNECTING),
              m_nextUpdateTimer(0.0f),
              m_updateInterval(1.0f),
              m_timeoutInterval(3.0f),
              m_playerID(0)
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

                                // Get the player ID
                                BitStreamer Streamer = BitStreamer();
                                unsigned char* DataPointer = Message.Data;
                                Streamer.SetTargetBuffer(DataPointer, sizeof(Message.Data));
                                m_playerID = Streamer.ReadUnsignedInt32();

                                // Increase the rate of which we send messages
                                m_updateInterval = 0.017f;

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

                            // TODOCM add if we're disconnected already we skip connecting back

                            // Something went wrong or we disconnected
                            m_serverConnectionState = ConnectionState::CONNECTING;
                            m_serverLastResponse = 0;
                            m_updateInterval = 1.0f;

                            break;
                        default:
                            break;
                    }
                }
            }
        }


        void ClientNetworkManager::RecieveConnected(NetMessage& Message)
        {
            BitStreamer Streamer = BitStreamer();
            unsigned char* BufferPointer = Message.Data;
            Streamer.SetTargetBuffer(BufferPointer, sizeof(Message.Data));

            bool GameStarts = Streamer.ReadBool();

            if(GameStarts)
            {
                m_serverConnectionState = ConnectionState::MAP_LOADING;
            }
        }

        void ClientNetworkManager::RecieveLoadWorld(NetMessage& p_message)
        {
            if(m_serverConnectionState == ConnectionState::MAP_LOADING)
            {
                // TODOCM add real loadworld code here
                InputHandlerClient* NewInputHandler = new InputHandlerClient(m_sharedContext);

                PlayerHandler::GetInstance()->CreateNewPlayer(m_playerID, NewInputHandler);

                m_serverConnectionState = ConnectionState::IN_GAME;

                // Update last response
                m_serverLastResponse = 0;
            }
            else
            {
                // TODOCM If reliable messge is wrong?
            }
        }

        void ClientNetworkManager::RecieveSnapshot(NetMessage& p_message, bool p_initial = false)
        {
            if(m_serverConnectionState == ConnectionState::IN_GAME)
            {
                // Read and translate message, for each position we update buffer array?
                BitStreamer Streamer = BitStreamer();
                unsigned char* BufferPointer = p_message.Data;
                Streamer.SetTargetBuffer(BufferPointer, sizeof(p_message.Data));

                EntityHandler& EntityHandler = EntityHandler::GetInstance();

                Snapshot* NewSnapshot = new Snapshot();


                NewSnapshot->SnapshotSequence = Streamer.ReadUnsignedInt8();
                if(p_initial)
                {
                    InterpolationHandler::GetInstance()->SetSequence(NewSnapshot->SnapshotSequence);
                }

                NewSnapshot->NumOfObjects = Streamer.ReadUnsignedInt8();

                for(size_t i = 0; i < NewSnapshot->NumOfObjects; i++)
                {
                    NewSnapshot->Objects[i].EntityID = Streamer.ReadUnsignedInt32();
                    NewSnapshot->Objects[i].Component.position = Streamer.ReadFloat3();
                    NewSnapshot->Objects[i].Component.rotation = Streamer.ReadRotationQuaternion();
                }

                InterpolationHandler::GetInstance()->QueueSnapshot(NewSnapshot);

                // Update last response
                m_serverLastResponse = 0;

                // TODOCM notify a handler or something that new buffer exists
            }
            else
            {
                // TODOCM If reliable messge is wrong?
            }
        }

        void ClientNetworkManager::RecieveReliable(double p_dt)
        {
            if(m_serverConnectionState >= ConnectionState::CONNECTED)
            {
                DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();
                NetMessage Message = NetMessage();

                // Attempt recieve reliable message
                while(NetworkModule.RecieveReliableData(&Message, sizeof(Message), m_serverReliableSocketHandle))
                {
                    // std::cout << "Recieved snapshot." << std::endl; // TODOCM logg instead

                    switch(Message.MessageID)
                    {
                        case MessageID::CONNECTED:

                            RecieveConnected(Message);
                            break;

                        case MessageID::LOAD_WORLD:

                            RecieveLoadWorld(Message);
                            break;

                        case MessageID::INIT_SNAPSHOT:

                            RecieveSnapshot(Message, true);
                            break;

                        case MessageID::SNAPSHOT:

                            RecieveSnapshot(Message);
                            break;

                        default:
                            break;
                    }


                    Message = NetMessage();
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
            BitStreamer Streamer = BitStreamer();
            unsigned char* BufferPointer = Message.Data;
            Streamer.SetTargetBuffer(BufferPointer, sizeof(Message.Data));

            Streamer.WriteUnsignedInt32(m_playerID);

            std::cout << "Sending version message." << std::endl; // TODOCM logg instead

            // Send Message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&Message, sizeof(Message), m_serverUnreliableSocketHandle, m_serverAdress);
        }

        void ClientNetworkManager::CreateInputMessage(NetMessage &p_message)
        {
            InputHandler* inputHandler = PlayerHandler::GetInstance()->GetDefaultInputHandler();

            // Create a stream
            BitStreamer Streamer = BitStreamer();

            // Set message buffer to stream
            unsigned char* BufferPointer = p_message.Data;
            Streamer.SetTargetBuffer(BufferPointer, sizeof(p_message));

            // Write input(position directions) to stream
            Streamer.WriteInt32(inputHandler->GetInputBitMask());

            // Get orientation
            EntityID id = PlayerHandler::GetInstance()->GetDefaultPlayerEntityID();

            if (EntityHandler::GetInstance().HasComponents(id, (int)ComponentType::Transform))
            {
                // Write orientation
                Streamer.WriteRotationQuaternion(GetComponent<TransformComponent>(id)->rotation);
            }
        }

        void ClientNetworkManager::SendConnectedMessage()
        {
            // Create a message
            NetMessage Message = NetMessage();

            Message.MessageID = MessageID::CONNECT;

            // Send Message
            m_sharedContext.GetNetworkModule().SendReliableData(&Message, sizeof(Message), m_serverReliableSocketHandle);
        }

        void ClientNetworkManager::SendMapLoadingMessage()
        {
            NetMessage Message = NetMessage();
            Message.MessageID = MessageID::LOAD_WORLD;

            // Send Message
            m_sharedContext.GetNetworkModule().SendReliableData(&Message, sizeof(Message), m_serverReliableSocketHandle);
        }

        void ClientNetworkManager::SendInGameMessage()
        {
            NetMessage Message = NetMessage();
            Message.MessageID = MessageID::INPUT;

            // std::cout << "Sending input message" << std::endl; // TODOCM logg instead
            // Createing input message


            CreateInputMessage(Message);

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