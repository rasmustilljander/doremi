#include <Doremi/Core/Include/Network/NetworkMessagesClient.hpp>

// Engine
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Configuration/Include/ConfigurationModule.hpp>
#include <DoremiEngine/Network/Include/NetworkModule.hpp>

// Handlers
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>
#include <Doremi/Core/Include/InterpolationHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/AudioHandler.hpp>
#include <Doremi/Core/Include/MenuClasses/ServerBrowserHandler.hpp>
#include <Doremi/Core/Include/TimeHandler.hpp>

// Connection
#include <Doremi/Core/Include/Network/NetworkConnectionsClient.hpp>

// Messages
#include <Doremi/Core/Include/Network/NetMessages.hpp>

// Streamer
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>

// Event receiver
#include <Doremi\Core\Include\NetworkEventReceiver.hpp>

// Standard
#include <DirectXMath.h>

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

        /**
            == Receive Messages Connecting ==
        */

        void NetworkMessagesClient::ReceiveVersionCheck(NetMessageServerClientConnectingFromServer& p_message)
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // If our connection state is connecting
            if(t_networkConnection->m_serverConnection.ConnectionState == ServerConnectionStateFromClient::CONNECTING)
            {
                // Change state to version check
                t_networkConnection->m_serverConnection.ConnectionState = ServerConnectionStateFromClient::VERSION_CHECK;
                t_networkConnection->m_serverConnection.LastResponse = 0;
            }
        }

        void NetworkMessagesClient::ReceiveConnect(NetMessageServerClientConnectingFromServer& p_message)
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // If our connection state is version check
            if(t_networkConnection->m_serverConnection.ConnectionState == ServerConnectionStateFromClient::VERSION_CHECK)
            {
                // Check if we lagg, we don't want to connect if we lagg
                if(TimeHandler::GetInstance()->IsLagging())
                {
                    cout << "Declined connection, lagging" << endl;
                    return;
                }

                // Ready for read
                NetworkStreamer t_streamer = NetworkStreamer();
                unsigned char* t_dataPointer = p_message.Data;
                t_streamer.SetTargetBuffer(t_dataPointer, sizeof(p_message.Data));

                // Get playerID from server
                uint32_t t_lastPlayerID = t_networkConnection->m_serverConnection.PlayerID;
                t_networkConnection->m_serverConnection.PlayerID = t_streamer.ReadUnsignedInt32();

                // Get port to join with
                uint32_t t_portToJoinWith = t_streamer.ReadUnsignedInt32();

                // If it changed, we want to update config file for now
                // TODO change not to write to config file all times we get new value?
                if(t_lastPlayerID != t_networkConnection->m_serverConnection.PlayerID)
                {
                    // Update the value
                    m_sharedContext.GetConfigurationModule().GetModifiableConfigurationInfo().LastServerPlayerID = t_networkConnection->m_serverConnection.PlayerID;

                    // TODOXX if we change config file name, this will crash like hell
                    m_sharedContext.GetConfigurationModule().WriteConfigurationValuesToFile("Configuration.txt");
                }

                // Update last response
                t_networkConnection->m_serverConnection.LastResponse = 0;

                // Create connected adress
                uint8_t IP_a = t_networkConnection->m_serverConnection.ConnectingAdress->GetIP_A();
                uint8_t IP_b = t_networkConnection->m_serverConnection.ConnectingAdress->GetIP_B();
                uint8_t IP_c = t_networkConnection->m_serverConnection.ConnectingAdress->GetIP_C();
                uint8_t IP_d = t_networkConnection->m_serverConnection.ConnectingAdress->GetIP_D();

                t_networkConnection->m_serverConnection.ConnectedAdress =
                    m_sharedContext.GetNetworkModule().CreateAdress(IP_a, IP_b, IP_c, IP_d, t_portToJoinWith);


                // Attempt connect to server
                bool t_connected = m_sharedContext.GetNetworkModule().ConnectToReliable(t_networkConnection->m_serverConnection.ConnectedAdress,
                                                                                        t_networkConnection->m_serverConnection.ConnectedSocketHandle);
                if(t_connected)
                {
                    // Update state
                    t_networkConnection->m_serverConnection.ConnectionState = ServerConnectionStateFromClient::CONNECTED;
                }
            }
        }

        void NetworkMessagesClient::ReceiveDisconnect(NetMessageServerClientConnectingFromServer& p_message)
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // If we're above intro phase, we restart
            if(t_networkConnection->m_serverConnection.ConnectionState > ServerConnectionStateFromClient::CONNECTING)
            {
                // Ready for read
                NetworkStreamer t_streamer = NetworkStreamer();
                unsigned char* t_bufferPointer = p_message.Data;
                t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(p_message.Data));

                // TODOCM print to message box why we disconnected
                // Write disconnect string
                std::string t_disconnectMessage = t_streamer.ReadString();

                cout << "Sending disconnect: " << t_disconnectMessage << endl;
            }
        }

        /**
            == Receive Messages Connected ==
        */

        void NetworkMessagesClient::ReceiveConnected(NetMessageServerClientConnectedFromServer& p_message)
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            if(t_networkConnection->m_serverConnection.ConnectionState == ServerConnectionStateFromClient::CONNECTED)
            {
                // Ready for read
                NetworkStreamer t_streamer = NetworkStreamer();
                unsigned char* t_bufferPointer = p_message.Data;
                t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(p_message.Data));

                // Update timeout
                t_networkConnection->m_serverConnection.LastResponse = 0;
                // TODOCM check if we need something here
            }
        }

        void NetworkMessagesClient::ReceiveLoadWorld(NetMessageServerClientConnectedFromServer& p_message)
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();
            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());

            // We only care of this if we're loading map
            if(t_networkConnection->m_serverConnection.ConnectionState >= ServerConnectionStateFromClient::CONNECTED)
            {
                // If we receive this we know we should load map, change state if we're in wrong state
                if(t_networkConnection->m_serverConnection.ConnectionState == ServerConnectionStateFromClient::CONNECTED)
                {
                    t_networkConnection->m_serverConnection.ConnectionState = ServerConnectionStateFromClient::LOAD_WORLD;
                    // TODOCM change state of client as well, to load the map, but ignore events untill done there :):):):)
                }

                // Ready for read
                NetworkStreamer t_streamer = NetworkStreamer();
                unsigned char* t_bufferPointer = p_message.Data;
                t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(p_message.Data));

                // Counter for bytes read
                uint32_t t_bytesRead = 0;

                // Read how many join events there are
                uint32_t t_numOfJoinEvents = t_streamer.ReadUnsignedInt32();
                t_bytesRead += sizeof(uint32_t);

                t_playerHandler->SetMaximumNumberOfJoinEvents(t_numOfJoinEvents);

                // Read join events
                t_playerHandler->ReadEventsForJoin(t_streamer, sizeof(p_message.Data), t_bytesRead);

                // Update timeout
                t_networkConnection->m_serverConnection.LastResponse = 0;

                // TODOCM Update loading screen
            }
        }

        void NetworkMessagesClient::ReceiveInGame(NetMessageServerClientConnectedFromServer& p_message)
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();
            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());
            NetworkEventReceiver* t_eventReceiver = t_playerHandler->GetNetworkEventReceiver();

            // If we were at loading world, we assume server knows best and we're done loading!
            if(t_networkConnection->m_serverConnection.ConnectionState == ServerConnectionStateFromClient::LOAD_WORLD)
            {
                // If we didn't actually read all the events yet, we ignore this one, cause ther might be packet loss in UDP
                if(t_playerHandler->GetLastJoinEventRead() != t_playerHandler->GetMaximumNumberOfJoinEvents())
                {
                    cout << "Didn't receive all events yet" << endl;
                    return;
                }
                t_networkConnection->m_serverConnection.ConnectionState = ServerConnectionStateFromClient::IN_GAME;
            }

            // Ready for read
            NetworkStreamer t_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = p_message.Data;
            t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(p_message.Data));

            // Bytes read counter
            uint32_t t_bytesRead = 0;

            // Create a new snapshot
            Snapshot* t_newSnapshot = new Snapshot();

            // Read sequence of snapshot
            t_newSnapshot->SnapshotSequence = t_streamer.ReadUnsignedInt8();
            t_bytesRead += sizeof(uint8_t);

            // Read sequence of incomming position
            t_newSnapshot->SequenceToCheckPosAgainst = t_streamer.ReadUnsignedInt8();
            t_bytesRead += sizeof(uint8_t);

            // Read position to check against
            t_newSnapshot->PlayerPositionToCheck = t_streamer.ReadFloat3();
            t_bytesRead += sizeof(float) * 3;

            // Read events into the eventreceiver
            t_eventReceiver->ReadEvents(t_streamer, sizeof(p_message.Data), t_bytesRead);

            // Get the events and save events to snapshot
            t_newSnapshot->Events = t_eventReceiver->GetEventsReceivedFromServer();

            // If it was init message
            if(t_networkConnection->m_serverConnection.LastSequenceUpdate >= SEQUENCE_UPDATE_TIMER)
            {
                InterpolationHandler::GetInstance()->SetSequence(t_newSnapshot->SnapshotSequence);

                // Set our connection to not so special anymore
                t_networkConnection->m_serverConnection.LastSequenceUpdate = 0.0f;
            }

            // Check if we can read even more!
            if(sizeof(p_message.Data) - t_bytesRead > sizeof(uint8_t))
            {
                // Read how many objects we got in the message
                t_newSnapshot->NumOfObjects = t_streamer.ReadUnsignedInt8();
                t_bytesRead += sizeof(uint8_t);

                // Add objects to snapshot
                for(size_t i = 0; i < t_newSnapshot->NumOfObjects; i++)
                {
                    t_newSnapshot->Objects[i].EntityID = t_streamer.ReadUnsignedInt32();
                    DirectX::XMFLOAT3 t_position = t_streamer.ReadFloat3();
                    DirectX::XMFLOAT4 p_orientation = t_streamer.ReadRotationQuaternion();

                    t_newSnapshot->Objects[i].Component = TransformComponentNext(t_position, p_orientation, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)); // TODOXX setting scale here, not sure if want to send over network?
                }
            }

            // Queue snapshot
            InterpolationHandler::GetInstance()->QueueSnapshot(t_newSnapshot);


            // Update timeout
            t_networkConnection->m_serverConnection.LastResponse = 0;
        }

        /**
            == Send Messages Connecting ==
        */

        void NetworkMessagesClient::SendConnectionRequest()
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // Create a message
            NetMessageServerClientConnectingFromClient t_message = NetMessageServerClientConnectingFromClient();

            // Set correct ID
            t_message.MessageID = SendMessageIDToServerFromClient::CONNECTION_REQUEST;

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_message, sizeof(t_message), t_networkConnection->m_serverConnection.ConnectingSocketHandle,
                                                                  t_networkConnection->m_serverConnection.ConnectingAdress);
        }

        void NetworkMessagesClient::SendVersionCheck()
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // Create a message
            NetMessageServerClientConnectingFromClient t_message = NetMessageServerClientConnectingFromClient();

            // Set correct ID
            t_message.MessageID = SendMessageIDToServerFromClient::VERSION_CHECK;

            // Ready for write
            NetworkStreamer t_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = t_message.Data;
            t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(t_message.Data));

            // Write playerID
            t_streamer.WriteUnsignedInt32(t_networkConnection->m_serverConnection.PlayerID);

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_message, sizeof(t_message), t_networkConnection->m_serverConnection.ConnectingSocketHandle,
                                                                  t_networkConnection->m_serverConnection.ConnectingAdress);
        }

        void NetworkMessagesClient::SendDisconnect()
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // Create a message
            NetMessageServerClientConnectingFromClient t_message = NetMessageServerClientConnectingFromClient();

            // Set correct ID
            t_message.MessageID = SendMessageIDToServerFromClient::DISCONNECT;

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_message, sizeof(t_message), t_networkConnection->m_serverConnection.ConnectingSocketHandle,
                                                                  t_networkConnection->m_serverConnection.ConnectingAdress);
        }

        /**
            == Send Messages Connecting ==
        */

        void NetworkMessagesClient::SendConnected()
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // Create a message
            NetMessageServerClientConnectedFromClient t_message = NetMessageServerClientConnectedFromClient();

            // Set correct ID
            t_message.MessageID = SendMessageIDToServerFromClient::CONNECTED;

            // Send message
            m_sharedContext.GetNetworkModule().SendReliableData(&t_message, sizeof(t_message), t_networkConnection->m_serverConnection.ConnectedSocketHandle);
        }

        void NetworkMessagesClient::SendLoadWorld()
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();
            PlayerHandlerClient* t_playerhandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());

            // Create a message
            NetMessageServerClientConnectedFromClient t_message = NetMessageServerClientConnectedFromClient();

            // Set correct ID
            t_message.MessageID = SendMessageIDToServerFromClient::LOAD_WORLD;

            // Ready for write
            NetworkStreamer t_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = t_message.Data;
            t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(t_message.Data));

            // Bytes written counter
            uint32_t t_bytesWritten = 0;

            // Get number of events received
            uint32_t eventAcc = t_playerhandler->GetLastJoinEventRead();

            // Write it to the message
            t_streamer.WriteUnsignedInt32(eventAcc);
            t_bytesWritten += sizeof(uint32_t);

            // Send message
            m_sharedContext.GetNetworkModule().SendReliableData(&t_message, sizeof(t_message), t_networkConnection->m_serverConnection.ConnectedSocketHandle);
        }

        void NetworkMessagesClient::SendInGame()
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();
            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());
            NetworkEventReceiver* t_eventReceiver = t_playerHandler->GetNetworkEventReceiver();
            AudioHandler* t_audioHandler = AudioHandler::GetInstance();
            InputHandlerClient* t_inputHandler = t_playerHandler->GetInputHandler();

            // Create a message
            NetMessageServerClientConnectedFromClient t_message = NetMessageServerClientConnectedFromClient();

            // Set correct ID
            t_message.MessageID = SendMessageIDToServerFromClient::IN_GAME;

            // Ready for write
            NetworkStreamer t_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = t_message.Data;
            t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(t_message.Data));

            // Bytes written counter
            uint32_t t_bytesWritten = 0;

            // Get current sequence used for snapshots
            uint8_t t_currentSequence = InterpolationHandler::GetInstance()->GetRealSnapshotSequence();

            // Write sequence ack
            t_streamer.WriteUnsignedInt8(t_currentSequence);
            t_bytesWritten += sizeof(uint8_t);

            // Write input bitmask
            t_streamer.WriteUnsignedInt32(t_inputHandler->GetInputBitMask());
            t_bytesWritten += sizeof(uint32_t);

            // Get player transform component
            EntityID t_playerEntityID = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance())->GetPlayerEntityID();
            TransformComponent* t_transformComp = GetComponent<TransformComponent>(t_playerEntityID);

            // Write orientation
            t_streamer.WriteRotationQuaternion(t_transformComp->rotation);
            t_bytesWritten += sizeof(float) * 4;

            // Get sequence acc for events
            uint8_t t_eventSequenceAck = t_eventReceiver->GetNextSequenceUsed();

            // Write event ack
            t_streamer.WriteUnsignedInt8(t_eventSequenceAck);
            t_bytesWritten += sizeof(uint8_t);

            // Write sound frequencies
            float t_frequency = t_audioHandler->GetFrequency();
            t_streamer.WriteFloat(t_frequency);
            t_bytesWritten += sizeof(float);

            // Send message
            m_sharedContext.GetNetworkModule().SendReliableData(&t_message, sizeof(t_message), t_networkConnection->m_serverConnection.ConnectedSocketHandle);
        }

        void NetworkMessagesClient::ReceiveConnectedMaster(NetMessageMasterClientFromMaster& p_message)
        {
            NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();
            ServerBrowserHandler* t_ServerBrowserHandler = ServerBrowserHandler::GetInstance();

            if(t_connections->m_masterConnection.ConnectionState > MasterConnectionStateFromClient::DISCONNECTED)
            {
                // If we're connecting we're now connected
                if(t_connections->m_masterConnection.ConnectionState == MasterConnectionStateFromClient::CONNECTING)
                {
                    t_connections->m_masterConnection.ConnectionState = MasterConnectionStateFromClient::CONNECTED;
                }

                // Ready for read
                NetworkStreamer t_streamer = NetworkStreamer();
                unsigned char* p_bufferPointer = p_message.Data;
                t_streamer.SetTargetBuffer(p_bufferPointer, sizeof(p_message.Data));

                // Read number of servers
                uint8_t t_numberOfServers = t_streamer.ReadUnsignedInt8();


                for(size_t i = 0; i < t_numberOfServers; i++)
                {
                    std::string t_name = t_streamer.ReadStringShort(); // 20 bytes
                    ServerStates t_serverState = static_cast<ServerStates>(t_streamer.ReadUnsignedInt8()); // 1 byte
                    GameMap t_map = static_cast<GameMap>(t_streamer.ReadUnsignedInt8()); // 1 byte
                    uint16_t t_ping = t_streamer.ReadUnsignedInt16(); // 2 bytes

                    uint8_t t_curNumPlayers = t_streamer.ReadUnsignedInt8(); // 1 byte
                    uint8_t t_maxNumPlayers = t_streamer.ReadUnsignedInt8(); // 1 byte

                    uint16_t t_port = t_streamer.ReadUnsignedInt16(); // 2 byte
                    uint8_t t_IP_a = t_streamer.ReadUnsignedInt8(); // 1 byte
                    uint8_t t_IP_b = t_streamer.ReadUnsignedInt8(); // 1 byte
                    uint8_t t_IP_c = t_streamer.ReadUnsignedInt8(); // 1 byte
                    uint8_t t_IP_d = t_streamer.ReadUnsignedInt8(); // 1 byte

                    // cout << "Received server: " << t_name << "In state: " << (uint32_t)t_serverState << " With map: " << (uint32_t)t_map << endl;
                    // cout << "Players:" << (uint32_t)t_curNumPlayers << "/" << (uint32_t)t_maxNumPlayers << endl;
                    // cout << "IP: " << (uint32_t)t_IP_a << "." << (uint32_t)t_IP_b << "." << (uint32_t)t_IP_c << "." << (uint32_t)t_IP_d
                    //     << " with port: " << t_port << endl;

                    // Update the server list
                    t_ServerBrowserHandler->UpdateServer(t_name, t_serverState, t_map, t_ping, t_curNumPlayers, t_maxNumPlayers, t_port, t_IP_a,
                                                         t_IP_b, t_IP_c, t_IP_d);
                }

                // Update last reposnse
                t_connections->m_masterConnection.LastResponse = 0;
            }
        }

        void NetworkMessagesClient::ReceiveDisconnectMaster(NetMessageMasterClientFromMaster& p_message)
        {
            NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();
            if(t_connections->m_masterConnection.ConnectionState > MasterConnectionStateFromClient::DISCONNECTED)
            {
                // If we were connected or something above(future), we retry connecting
                if(t_connections->m_masterConnection.ConnectionState >= MasterConnectionStateFromClient::CONNECTED)
                {
                    t_connections->m_masterConnection.ConnectionState = MasterConnectionStateFromClient::CONNECTING;
                }
            }
        }

        void NetworkMessagesClient::SendConnectionRequestMaster()
        {
            // Create connection request message
            NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();

            NetMessageMasterClientFromClient t_newMessage = NetMessageMasterClientFromClient();
            t_newMessage.MessageID = SendMessageIDToMasterFromClient::CONNECTION_REQUEST;

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage), t_connections->m_masterConnection.SocketHandle,
                                                                  t_connections->m_masterConnection.Adress);
        }

        void NetworkMessagesClient::SendConnectedMaster()
        {
            // Create connected message
            NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();
            NetMessageMasterClientFromClient t_newMessage = NetMessageMasterClientFromClient();
            t_newMessage.MessageID = SendMessageIDToMasterFromClient::CONNECTED;

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage), t_connections->m_masterConnection.SocketHandle,
                                                                  t_connections->m_masterConnection.Adress);
        }

        void NetworkMessagesClient::SendDisconnectMaster()
        {
            // Create disconnect message
            NetworkConnectionsClient* t_connections = NetworkConnectionsClient::GetInstance();
            NetMessageMasterClientFromClient t_newMessage = NetMessageMasterClientFromClient();
            t_newMessage.MessageID = SendMessageIDToMasterFromClient::DISCONNECT;

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage), t_connections->m_masterConnection.SocketHandle,
                                                                  t_connections->m_masterConnection.Adress);
        }
    }
}