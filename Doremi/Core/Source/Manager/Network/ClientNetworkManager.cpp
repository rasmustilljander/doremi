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
#include <AudioHandler.hpp>
#include <InputHandlerClient.hpp>
#include <AddRemoveSyncHandler.hpp>
#include <SequenceMath.hpp>
#include <FrequencyBufferHandler.hpp>
#include <PositionCorrectionHandler.hpp>

#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>

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
            m_unreliableServerAdress = p_sharedContext.GetNetworkModule().CreateAdress(127, 0, 0, 1, 5050); // TODOCM remove test code
            m_reliableServerAdress = p_sharedContext.GetNetworkModule().CreateAdress(127, 0, 0, 1, 4050);

            m_serverUnreliableSocketHandle = p_sharedContext.GetNetworkModule().CreateUnreliableSocket();

            NetMessage Message = NetMessage();
            Message.MessageID = MessageID::CONNECT_REQUEST;
            p_sharedContext.GetNetworkModule().SendUnreliableData(&Message, sizeof(Message), m_serverUnreliableSocketHandle, m_unreliableServerAdress);

            cout << "Change IP? Enter each byte with enter. (if other local is assumed)" << endl;

            int a, b, c, d;
            cin >> a;
            if(0 > a || a > 255)
            {
                return;
            }
            cin >> b;
            if(0 > b || b > 255)
            {
                return;
            }
            cin >> c;
            if(0 > c || c > 255)
            {
                return;
            }
            cin >> d;
            if(0 > d || d > 255)
            {
                return;
            }
            SetServerIP(a, b, c, d);
        }

        ClientNetworkManager::~ClientNetworkManager() {}

        void ClientNetworkManager::Update(double p_dt)
        {
            //std::cout << "Real: " << (uint32_t)(InterpolationHandler::GetInstance()->GetRealSnapshotSequence()) << std::endl;

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
                                BitStreamer Streamer = BitStreamer();
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
                                //delete m_reliableServerAdress;
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

                            PlayerHandler::GetInstance()->RemoveAllPlayers();

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

        void ClientNetworkManager::RecieveMapLoading(NetMessage& p_message)
        {
            if(m_serverConnectionState == ConnectionState::MAP_LOADING)
            {
                EntityHandler& entityHandler = EntityHandler::GetInstance();

                BitStreamer Streamer = BitStreamer();
                unsigned char* dataPointer = p_message.Data;
                Streamer.SetTargetBuffer(dataPointer, sizeof(p_message.Data));

                // Read number of existing players
                uint32_t numPlayers = Streamer.ReadUnsignedInt32();

                for(size_t i = 0; i < numPlayers; i++)
                {
                    entityHandler.CreateEntity(Blueprints::NetworkPlayerEntity);
                }

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

                uint32_t ReadOffset = 0;

                // Update frequency buffer by acc
                PlayerHandler::GetInstance()->GetDefaultFrequencyBufferHandler()->UpdateBufferFromSequence(Streamer.ReadUnsignedInt8());
                ReadOffset += sizeof(uint8_t);

                // Check add/remove items
                PlayerHandler::GetInstance()->GetAddRemoveSyncHandlerForPlayer(m_playerID)->CheckNewAddRemoves(Streamer, sizeof(p_message.Data), ReadOffset);

                uint32_t ReadOffset2 = ReadOffset;

                // Create new snapshot
                Snapshot* NewSnapshot = new Snapshot();

                // Read sequence of snapshot
                NewSnapshot->SnapshotSequence = Streamer.ReadUnsignedInt8();

                if(p_initial)
                {
                    InterpolationHandler::GetInstance()->SetSequence(NewSnapshot->SnapshotSequence);
                }

                // Read sequence of incomming position
                uint8_t PositionCheckSequence = Streamer.ReadUnsignedInt8();

                // Read position to check against
                DirectX::XMFLOAT3 PositionToCheck = Streamer.ReadFloat3();

                // Read how many objects we got in the message
                NewSnapshot->NumOfObjects = Streamer.ReadUnsignedInt8();

                // Add objects to snapshot
                for(size_t i = 0; i < NewSnapshot->NumOfObjects; i++)
                {
                    NewSnapshot->Objects[i].EntityID = Streamer.ReadUnsignedInt32();
                    NewSnapshot->Objects[i].Component.position = Streamer.ReadFloat3();
                    NewSnapshot->Objects[i].Component.rotation = Streamer.ReadRotationQuaternion();
                }

                // Queue snapshot
                InterpolationHandler::GetInstance()->QueueSnapshot(NewSnapshot);

                // Update last response
                m_serverLastResponse = 0;

                // TODOCM notify a handler or something that new buffer exists

                // Check the position we got from server
                PositionCorrectionHandler::GetInstance()->CheckPositionFromServer(m_playerID, PositionToCheck, PositionCheckSequence);
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
            switch (m_serverConnectionState)
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

            // TODOCM Write bits for stuff

            // std::cout << "Sending Connect request message." << std::endl; // TODOCM logg instead

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
            BitStreamer Streamer = BitStreamer();
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
            BitStreamer Streamer = BitStreamer();

            // Set message buffer to stream
            unsigned char* BufferPointer = p_message.Data;
            Streamer.SetTargetBuffer(BufferPointer, sizeof(p_message.Data));

            uint32_t bytesWritten = 0;

            // Write sequence
            Streamer.WriteUnsignedInt8(InterpolationHandler::GetInstance()->GetRealSnapshotSequence());
            bytesWritten += sizeof(uint8_t);
            //cout << "Writing " << (uint32_t)InterpolationHandler::GetInstance()->GetRealSnapshotSequence() << endl;

            // Write input(position directions) to stream
            Streamer.WriteUnsignedInt32(inputHandler->GetInputBitMask());
            bytesWritten += sizeof(uint32_t);

            // Get orientation
            EntityID id = 0;
            if(!PlayerHandler::GetInstance()->GetDefaultPlayerEntityID(id))
            {
                cout << "wrong in createinput message" << endl;
            }

            // Write orientation and translation
            Streamer.WriteRotationQuaternion(GetComponent<TransformComponent>(id)->rotation);
            bytesWritten += sizeof(float) * 4;

            Streamer.WriteFloat3(GetComponent<TransformComponent>(id)->position);
            bytesWritten += sizeof(float) * 3;

            // Write last sequence used for add remove
            uint8_t lastSequenceUsed = PlayerHandler::GetInstance()->GetAddRemoveSyncHandlerForPlayer(m_playerID)->GetNextSequenceUsed();

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
            PlayerHandler::GetInstance()->RemoveAllPlayers();

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