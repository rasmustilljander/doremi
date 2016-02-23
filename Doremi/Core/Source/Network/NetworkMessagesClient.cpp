#include <Doremi/Core/Include/Network/NetworkMessagesClient.hpp>

// Engine
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Configuration/Include/ConfigurationModule.hpp>
#include <DoremiEngine/Network/Include/NetworkModule.hpp>

// Connection
#include <Doremi/Core/Include/Network/NetworkConnectionsClient.hpp>

// Streamer
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>

namespace Doremi
{
    namespace Core
    {
        NetworkMessagesClient* NetworkMessagesClient::m_singleton = nullptr;

        NetworkMessagesClient* NetworkMessagesClient::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("Called GetInstance before StartupNetworkMessagesClient.");
            }
            return m_singleton;
        }

        void NetworkMessagesClient::StartupNetworkMessagesClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("Called StartupNetworkMessagesClient multiple times.");
            }
            m_singleton = new NetworkMessagesClient(p_sharedContext);
        }


        NetworkMessagesClient::NetworkMessagesClient(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

        NetworkMessagesClient::~NetworkMessagesClient() {}

        void NetworkMessagesClient::ReceiveVersionCheck(NetMessageConnectingFromServer& p_message)
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // If our connection state is connecting
            if(t_networkConnection->m_serverConnectionState.ConnectionState == ServerConnectionStateFromClient::CONNECTING)
            {
                // Change state to version check
                t_networkConnection->m_serverConnectionState.ConnectionState = ServerConnectionStateFromClient::VERSION_CHECK;
                t_networkConnection->m_serverConnectionState.LastResponse = 0;
            }
        }

        void NetworkMessagesClient::ReceiveConnect(NetMessageConnectingFromServer& p_message)
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // If our connection state is version check
            if(t_networkConnection->m_serverConnectionState.ConnectionState == ServerConnectionStateFromClient::VERSION_CHECK)
            {
                // Ready for read
                NetworkStreamer t_streamer = NetworkStreamer();
                unsigned char* t_dataPointer = p_message.Data;
                t_streamer.SetTargetBuffer(t_dataPointer, sizeof(p_message.Data));

                // Get playerID from server
                uint32_t t_lastPlayerID = t_networkConnection->m_serverConnectionState.PlayerID;
                t_networkConnection->m_serverConnectionState.PlayerID = t_streamer.ReadUnsignedInt32();

                // Get port to join with
                uint32_t t_portToJoinWith = t_streamer.ReadUnsignedInt32();

                // If it changed, we want to update config file for now
                // TODO change not to write to config file all times we get new value?
                if(t_lastPlayerID != t_networkConnection->m_serverConnectionState.PlayerID)
                {
                    // Update the value
                    m_sharedContext.GetConfigurationModule().GetModifiableConfigurationInfo().LastServerPlayerID =
                        t_networkConnection->m_serverConnectionState.PlayerID;

                    // TODOXX if we change config file name, this will crash like hell
                    m_sharedContext.GetConfigurationModule().WriteConfigurationValuesToFile("Configuration.txt");
                }

                // Update last response
                t_networkConnection->m_serverConnectionState.LastResponse = 0;

                // Attempt connect to server
                bool t_connected = m_sharedContext.GetNetworkModule().ConnectToReliable(t_networkConnection->GetServerAdress(),
                                                                                        t_networkConnection->m_serverConnectionState.ConnectedSocketHandle);
                if(t_connected)
                {
                    // Update state
                    t_networkConnection->m_serverConnectionState.ConnectionState = ServerConnectionStateFromClient::CONNECTED;

                    // Set update interval to connected
                    t_networkConnection->m_serverConnectionState.UpdateInterval = CONNECTED_UPDATE_INTERVAL;
                }
            }
        }

        void NetworkMessagesClient::ReceiveDisconnect(NetMessageConnectingFromServer& p_message) {}
        void NetworkMessagesClient::RecieveConnected(NetMessageConnectedFromServer& p_message) {}
        void NetworkMessagesClient::RecieveLoadWorld(NetMessageConnectedFromServer& p_message) {}
        void NetworkMessagesClient::RecieveInGame(NetMessageConnectedFromServer& p_message) {}
        void NetworkMessagesClient::SendConnectionRequest() {}
        void NetworkMessagesClient::SendVersionCheck() {}
        void NetworkMessagesClient::SendDisconnect() {}
        void NetworkMessagesClient::SendConnected() {}
        void NetworkMessagesClient::SendLoadWorld() {}
        void NetworkMessagesClient::SendInGame() {}


        void ClientNetworkManager::RecieveConnected(NetMessage& Message)
        {
            NetworkStreamer Streamer = NetworkStreamer();
            unsigned char* BufferPointer = Message.Data;
            Streamer.SetTargetBuffer(BufferPointer, sizeof(Message.Data));

            m_numJoinEvents = Streamer.ReadUnsignedInt32();

            bool GameStarts = Streamer.ReadBool();

            // I think this one is unecessary for now, since we'll be start loading world when we receive that message...
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


                PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());

                uint32_t t_previousNumEvents = t_playerHandler->GetLastJoinEventRead();

                // Write events
                t_playerHandler->ReadEventsForJoin(Streamer, sizeof(p_message.Data), t_bytesWritten);

                // If we got something new, print! :3
                uint32_t t_newNumEvents = t_playerHandler->GetLastJoinEventRead();
                if(t_previousNumEvents != t_newNumEvents)
                {
                    cout << "Loaded: " << ((float)t_newNumEvents / (float)m_numJoinEvents) * 100.0f << "%..." << endl;
                }

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
    }
}