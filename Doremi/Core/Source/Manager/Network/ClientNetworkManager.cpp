// Project specific

// Managers
#include <Manager/Network/ClientNetworkManager.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>

#include <Manager/Network/NetMessage.hpp>

// Modules
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>

#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>

// Handlers
#include <EntityComponent/EntityHandler.hpp>
#include <InterpolationHandler.hpp>
#include <AudioHandler.hpp>
#include <PlayerHandlerClient.hpp>
#include <InputHandlerClient.hpp>
#include <FrequencyBufferHandler.hpp>
#include <PositionCorrectionHandler.hpp>

#include <Doremi/Core/Include/NetworkEventReceiver.hpp>

#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>

// Components
#include <EntityComponent/Components/TransformComponent.hpp>

#include <iostream> // TODOCM remove after test
#include <SequenceMath.hpp>
#include <DoremiEngine/Configuration/Include/ConfigurationModule.hpp>

// Temporary should be removed
#include <Doremi/Core/Include/EntityComponent/Components/PressureParticleComponent.hpp>

namespace Doremi
{
    namespace Core
    {
        ClientNetworkManager::ClientNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "ClientNetworkManager"),
              m_masterConnectionState(ConnectionState::DISCONNECTED),
              m_serverConnectionState(ConnectionState::CONNECTING),
              m_nextUpdateTimer(0.0f),
              m_updateInterval(1.0f),
              m_timeoutInterval(3.0f),
              m_playerID(0)
        {
            LoadIPFromConfigFile(p_sharedContext);

            m_serverUnreliableSocketHandle = p_sharedContext.GetNetworkModule().CreateUnreliableSocket();

            NetMessage Message = NetMessage();
            Message.MessageID = MessageID::CONNECT_REQUEST;
            p_sharedContext.GetNetworkModule().SendUnreliableData(&Message, sizeof(Message), m_serverUnreliableSocketHandle, m_unreliableServerAdress);
        }

        ClientNetworkManager::~ClientNetworkManager() {}

        void ClientNetworkManager::LoadIPFromConfigFile(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            DoremiEngine::Configuration::ConfiguartionInfo t_configInfo = p_sharedContext.GetConfigurationModule().GetAllConfigurationValues();

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

        void ClientNetworkManager::Update(double p_dt)
        {
            // Recieve Messages
            RecieveMessages(p_dt);

            // Send Messages
            SendMessages(p_dt);

            // Check for timed out connections
            UpdateTimeouts(p_dt);
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
                                m_playerID = Streamer.ReadUnsignedInt32();

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


        void ClientNetworkManager::RecieveConnected(NetMessage& Message)
        {
            NetworkStreamer Streamer = NetworkStreamer();
            unsigned char* BufferPointer = Message.Data;
            Streamer.SetTargetBuffer(BufferPointer, sizeof(Message.Data));

            bool GameStarts = Streamer.ReadBool();

            if(GameStarts)
            {
                m_serverConnectionState = ConnectionState::MAP_LOADING;
            }
        }

        void ClientNetworkManager::RecieveMapLoading(NetMessage& p_message)
        {
            // If we're not in game, we care
            if(m_serverConnectionState <= ConnectionState::IN_GAME)
            {
                // If we just connected, and receive this we move into map loading state
                if(m_serverConnectionState == ConnectionState::CONNECTED)
                {
                    m_serverConnectionState = ConnectionState::MAP_LOADING;
                }

                // Create streamer
                NetworkStreamer Streamer = NetworkStreamer();
                unsigned char* dataPointer = p_message.Data;
                Streamer.SetTargetBuffer(dataPointer, sizeof(p_message.Data));

                uint32_t t_bytesWritten = 0;

                // Write events
                static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance())->ReadEventsForJoin(Streamer, sizeof(p_message.Data), t_bytesWritten);

                // Update last response
                m_serverLastResponse = 0;
            }
            else
            {
                // TODOCM If reliable messge is wrong?
                // Could be that we receive a late package.. so shouldn't be a problem.. and we're the client, so w/e...
            }
        }

        void ClientNetworkManager::RecieveSnapshot(NetMessage& p_message, bool p_initial = false)
        {
            if(m_serverConnectionState >= ConnectionState::MAP_LOADING)
            {
                if(m_serverConnectionState == ConnectionState::MAP_LOADING)
                {
                    m_serverConnectionState = ConnectionState::IN_GAME;

                    // Create inputhandler used for client
                    InputHandlerClient* t_newInputHandler = new InputHandlerClient(m_sharedContext);

                    // Create player
                    PlayerHandler::GetInstance()->CreateNewPlayer(m_playerID, t_newInputHandler);
                }


                // Update last response
                m_serverLastResponse = 0;

                // Read and translate message, for each position we update buffer array?
                NetworkStreamer Streamer = NetworkStreamer();
                unsigned char* BufferPointer = p_message.Data;
                Streamer.SetTargetBuffer(BufferPointer, sizeof(p_message.Data));

                EntityHandler& EntityHandler = EntityHandler::GetInstance();

                uint32_t ReadOffset = 0;

                // Update frequency buffer by acc
                PlayerHandler::GetInstance()->GetDefaultFrequencyBufferHandler()->UpdateBufferFromSequence(Streamer.ReadUnsignedInt8());
                ReadOffset += sizeof(uint8_t);

                // Create new snapshot
                Snapshot* NewSnapshot = new Snapshot();

                // Read sequence of snapshot
                NewSnapshot->SnapshotSequence = Streamer.ReadUnsignedInt8();
                ReadOffset += sizeof(uint8_t);

                // Read events
                NetworkEventReceiver* t_eventReciever =
                    static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance())->GetNetworkEventReceiverForPlayer(m_playerID);
                t_eventReciever->ReadEvents(Streamer, sizeof(p_message.Data), ReadOffset);

                // Save events to snapshot
                NewSnapshot->Events = t_eventReciever->GetEventsReceivedFromServer();

                if(p_initial)
                {
                    InterpolationHandler::GetInstance()->SetSequence(NewSnapshot->SnapshotSequence);
                }

                // Check if we can read the next
                if(sizeof(p_message.Data) - ReadOffset < sizeof(uint8_t) + sizeof(float) * 3)
                {
                    return;
                }

                // Read sequence of incomming position
                NewSnapshot->SequenceToCheckPosAgainst = Streamer.ReadUnsignedInt8();

                // Read position to check against
                NewSnapshot->PlayerPositionToCheck = Streamer.ReadFloat3();

                ReadOffset += sizeof(uint8_t) + sizeof(float) * 3;

                // Check if we can read the next
                if(sizeof(p_message.Data) - ReadOffset <= sizeof(uint8_t))
                {
                    return;
                }

                // Read how many objects we got in the message
                NewSnapshot->NumOfObjects = Streamer.ReadUnsignedInt8();
                ReadOffset += sizeof(uint8_t);


                // Add objects to snapshot
                for(size_t i = 0; i < NewSnapshot->NumOfObjects; i++)
                {
                    NewSnapshot->Objects[i].EntityID = Streamer.ReadUnsignedInt32();
                    XMFLOAT3 t_position = Streamer.ReadFloat3();
                    XMFLOAT4 p_orientation = Streamer.ReadRotationQuaternion();
                    NewSnapshot->Objects[i].Component =
                        TransformComponentNext(t_position, p_orientation,
                                               XMFLOAT3(1.0f, 1.0f, 1.0f)); // TODOXX setting scale here, not sure if want to send over network?
                }

                // Queue snapshot
                InterpolationHandler::GetInstance()->QueueSnapshot(NewSnapshot);
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

                    switch(Message.MessageID)
                    {
                        case MessageID::CONNECTED:

                            RecieveConnected(Message);
                            break;

                        case MessageID::LOAD_WORLD:

                            RecieveMapLoading(Message);
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

        void ClientNetworkManager::SendConnectRequestMessage()
        {
            // Create a message
            NetMessage Message = NetMessage();

            // Set message ID to connection request
            Message.MessageID = MessageID::CONNECT_REQUEST;


            // Send Message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&Message, sizeof(Message), m_serverUnreliableSocketHandle, m_unreliableServerAdress);
        }

        void ClientNetworkManager::SendVersionMessage()
        {
            // Create a message
            NetMessage Message = NetMessage();

            // Set message ID to version check
            Message.MessageID = MessageID::VERSION_CHECK;

            // TODOCM Write bits for stuff
            NetworkStreamer Streamer = NetworkStreamer();
            unsigned char* BufferPointer = Message.Data;
            Streamer.SetTargetBuffer(BufferPointer, sizeof(Message.Data));

            Streamer.WriteUnsignedInt32(m_playerID);

            std::cout << "Sending version message." << std::endl; // TODOCM logg instead

            // Send Message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&Message, sizeof(Message), m_serverUnreliableSocketHandle, m_unreliableServerAdress);
        }

        void ClientNetworkManager::CreateInputMessage(NetMessage& p_message)
        {
            InputHandler* inputHandler = PlayerHandler::GetInstance()->GetDefaultInputHandler();

            // Create a stream
            NetworkStreamer Streamer = NetworkStreamer();

            // Set message buffer to stream
            unsigned char* BufferPointer = p_message.Data;
            Streamer.SetTargetBuffer(BufferPointer, sizeof(p_message.Data));

            uint32_t bytesWritten = 0;

            // Write sequence
            Streamer.WriteUnsignedInt8(InterpolationHandler::GetInstance()->GetRealSnapshotSequence());
            bytesWritten += sizeof(uint8_t);

            // Write input(position directions) to stream
            Streamer.WriteUnsignedInt32(inputHandler->GetInputBitMask());
            bytesWritten += sizeof(uint32_t);

            // Get orientation, shouldn't be a problem if wrong for now
            EntityID id = 0;
            if(!PlayerHandler::GetInstance()->GetDefaultPlayerEntityID(id))
            {
                // cout << "wrong in createinput message" << endl;
            }

            // Write orientation and translation
            Streamer.WriteRotationQuaternion(GetComponent<TransformComponent>(id)->rotation);
            bytesWritten += sizeof(float) * 4;

            Streamer.WriteFloat3(GetComponent<TransformComponent>(id)->position);
            bytesWritten += sizeof(float) * 3;

            // Write last sequence used for add remove
            uint8_t lastSequenceUsed =
                (static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance()))->GetNetworkEventReceiverForPlayer(m_playerID)->GetNextSequenceUsed();

            Streamer.WriteUnsignedInt8(lastSequenceUsed);
            bytesWritten += sizeof(uint8_t);

            // Write sound frequency
            PlayerHandler::GetInstance()->GetDefaultFrequencyBufferHandler()->WriteFrequencies(Streamer, sizeof(p_message.Data), bytesWritten);
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
            // Create message base
            NetMessage Message = NetMessage();
            Message.MessageID = MessageID::LOAD_WORLD;

            // Create streamer
            NetworkStreamer t_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = Message.Data;
            t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(Message.Data));

            // Amount written
            uint32_t t_bytesWritten = 0;

            // Write acked event
            uint32_t eventAcc = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance())->GetLastJoinEventRead();

            t_streamer.WriteUnsignedInt32(eventAcc);
            t_bytesWritten += sizeof(uint32_t);

            // Send Message
            m_sharedContext.GetNetworkModule().SendReliableData(&Message, sizeof(Message), m_serverReliableSocketHandle);
        }

        void ClientNetworkManager::SendInGameMessage()
        {
            NetMessage Message = NetMessage();
            Message.MessageID = MessageID::INPUT;


            CreateInputMessage(Message);

            // Send Message
            m_sharedContext.GetNetworkModule().SendReliableData(&Message, sizeof(Message), m_serverReliableSocketHandle);
        }

        void ClientNetworkManager::SendDisconnectMessage()
        {
            // TODOCM remove world

            // Create disconnection message
            NetMessage NewMessage = NetMessage();
            NewMessage.MessageID = MessageID::DISCONNECT;

            // TODOCM add info

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&NewMessage, sizeof(NewMessage), m_serverUnreliableSocketHandle, m_unreliableServerAdress);
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