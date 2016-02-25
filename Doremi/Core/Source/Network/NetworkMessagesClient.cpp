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

// Connection
#include <Doremi/Core/Include/Network/NetworkConnectionsClient.hpp>

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
                bool t_connected = m_sharedContext.GetNetworkModule().ConnectToReliable(t_networkConnection->m_serverConnectionState.ConnectedAdress,
                                                                                        t_networkConnection->m_serverConnectionState.ConnectedSocketHandle);
                if(t_connected)
                {
                    // Update state
                    t_networkConnection->m_serverConnectionState.ConnectionState = ServerConnectionStateFromClient::CONNECTED;
                }
            }
        }

        void NetworkMessagesClient::ReceiveDisconnect(NetMessageConnectingFromServer& p_message)
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // If we're above intro phase, we restart
            if(t_networkConnection->m_serverConnectionState.ConnectionState > ServerConnectionStateFromClient::CONNECTING)
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

        void NetworkMessagesClient::ReceiveConnected(NetMessageConnectedFromServer& p_message)
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            if(t_networkConnection->m_serverConnectionState.ConnectionState == ServerConnectionStateFromClient::CONNECTED)
            {
                // Ready for read
                NetworkStreamer t_streamer = NetworkStreamer();
                unsigned char* t_bufferPointer = p_message.Data;
                t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(p_message.Data));

                // Update timeout
                t_networkConnection->m_serverConnectionState.LastResponse = 0;
                // TODOCM check if we need something here
            }
        }

        void NetworkMessagesClient::ReceiveLoadWorld(NetMessageConnectedFromServer& p_message)
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();
            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());

            // We only care of this if we're loading map
            if(t_networkConnection->m_serverConnectionState.ConnectionState >= ServerConnectionStateFromClient::CONNECTED)
            {
                // If we receive this we know we should load map, change state if we're in wrong state
                if(t_networkConnection->m_serverConnectionState.ConnectionState == ServerConnectionStateFromClient::CONNECTED)
                {
                    t_networkConnection->m_serverConnectionState.ConnectionState = ServerConnectionStateFromClient::LOAD_WORLD;
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

                // Read join events
                t_playerHandler->ReadEventsForJoin(t_streamer, sizeof(p_message.Data), t_bytesRead);

                // Update timeout
                t_networkConnection->m_serverConnectionState.LastResponse = 0;

                // TODOCM Update loading screen
            }
        }

        void NetworkMessagesClient::ReceiveInGame(NetMessageConnectedFromServer& p_message)
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();
            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());
            NetworkEventReceiver* t_eventReceiver = t_playerHandler->GetNetworkEventReceiver();

            // If we were at loading world, we assume server knows best and we're done loading!
            if(t_networkConnection->m_serverConnectionState.ConnectionState == ServerConnectionStateFromClient::LOAD_WORLD)
            {
                t_networkConnection->m_serverConnectionState.ConnectionState = ServerConnectionStateFromClient::IN_GAME;
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
            if(t_networkConnection->m_serverConnectionState.NewConnection)
            {
                InterpolationHandler::GetInstance()->SetSequence(t_newSnapshot->SnapshotSequence);

                // Set our connection to not so special anymore
                t_networkConnection->m_serverConnectionState.NewConnection = false;
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
            t_networkConnection->m_serverConnectionState.LastResponse = 0;
        }

        /**
            == Send Messages Connecting ==
        */

        void NetworkMessagesClient::SendConnectionRequest()
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // Create a message
            NetMessageConnectingFromClient t_message = NetMessageConnectingFromClient();

            // Set correct ID
            t_message.MessageID = SendMessageIDFromClient::CONNECTION_REQUEST;

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_message, sizeof(t_message), t_networkConnection->m_serverConnectionState.ConnectingSocketHandle,
                                                                  t_networkConnection->m_serverConnectionState.ConnectingAdress);
        }

        void NetworkMessagesClient::SendVersionCheck()
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // Create a message
            NetMessageConnectingFromClient t_message = NetMessageConnectingFromClient();

            // Set correct ID
            t_message.MessageID = SendMessageIDFromClient::VERSION_CHECK;

            // Ready for write
            NetworkStreamer t_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = t_message.Data;
            t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(t_message.Data));

            // Write playerID
            t_streamer.WriteUnsignedInt32(t_networkConnection->m_serverConnectionState.PlayerID);

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_message, sizeof(t_message), t_networkConnection->m_serverConnectionState.ConnectingSocketHandle,
                                                                  t_networkConnection->m_serverConnectionState.ConnectingAdress);
        }

        void NetworkMessagesClient::SendDisconnect()
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // Create a message
            NetMessageConnectingFromClient t_message = NetMessageConnectingFromClient();

            // Set correct ID
            t_message.MessageID = SendMessageIDFromClient::DISCONNECT;

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_message, sizeof(t_message), t_networkConnection->m_serverConnectionState.ConnectingSocketHandle,
                                                                  t_networkConnection->m_serverConnectionState.ConnectingAdress);
        }

        /**
            == Send Messages Connecting ==
        */

        void NetworkMessagesClient::SendConnected()
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();

            // Create a message
            NetMessageConnectedFromClient t_message = NetMessageConnectedFromClient();

            // Set correct ID
            t_message.MessageID = SendMessageIDFromClient::CONNECTED;

            // Send message
            m_sharedContext.GetNetworkModule().SendReliableData(&t_message, sizeof(t_message), t_networkConnection->m_serverConnectionState.ConnectedSocketHandle);
        }

        void NetworkMessagesClient::SendLoadWorld()
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();
            PlayerHandlerClient* t_playerhandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());

            // Create a message
            NetMessageConnectedFromClient t_message = NetMessageConnectedFromClient();

            // Set correct ID
            t_message.MessageID = SendMessageIDFromClient::LOAD_WORLD;

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
            m_sharedContext.GetNetworkModule().SendReliableData(&t_message, sizeof(t_message), t_networkConnection->m_serverConnectionState.ConnectedSocketHandle);
        }

        void NetworkMessagesClient::SendInGame()
        {
            NetworkConnectionsClient* t_networkConnection = NetworkConnectionsClient::GetInstance();
            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());
            NetworkEventReceiver* t_eventReceiver = t_playerHandler->GetNetworkEventReceiver();
            AudioHandler* t_audioHandler = AudioHandler::GetInstance();
            InputHandlerClient* t_inputHandler = t_playerHandler->GetInputHandler();

            // Create a message
            NetMessageConnectedFromClient t_message = NetMessageConnectedFromClient();

            // Set correct ID
            t_message.MessageID = SendMessageIDFromClient::IN_GAME;

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
            m_sharedContext.GetNetworkModule().SendReliableData(&t_message, sizeof(t_message), t_networkConnection->m_serverConnectionState.ConnectedSocketHandle);
        }
    }
}