#include <Doremi/Core/Include/Network/NetworkMessagesServer.hpp>

// Connections
#include <Doremi/Core/Include/Network/NetworkConnectionsServer.hpp>

// Engine
#include <DoremiEngine/Core/Include/SharedContext.hpp>

// Network
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <DoremiEngine/Network/Include/Adress.hpp>

// Net messages
#include <Doremi/Core/Include/Network/NetMessages.hpp>

// Streamer
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>

// Handler
#include <Doremi/Core/Include/PlayerHandlerServer.hpp>
#include <Doremi/Core/Include/NetworkPriorityHandler.hpp>
#include <Doremi/Core/Include/InputHandlerServer.hpp>
#include <Doremi/Core/Include/FrequencyBufferHandler.hpp>
#include <Doremi/Core/Include/ServerStateHandler.hpp>

#include <Doremi/Core/Include/NetworkEventSender.hpp>

#include <iostream> // TODOCM remove this debug

namespace Doremi
{
    namespace Core
    {
        NetworkMessagesServer* NetworkMessagesServer::m_singleton = nullptr;

        NetworkMessagesServer* NetworkMessagesServer::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("Called GetInstance before StartupNetworkMessagesServer.");
            }
            return m_singleton;
        }

        void NetworkMessagesServer::StartupNetworkMessagesServer(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("Called StartupNetworkMessagesServer multiple times.");
            }
            m_singleton = new NetworkMessagesServer(p_sharedContext);
        }

        NetworkMessagesServer::NetworkMessagesServer(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

        NetworkMessagesServer::~NetworkMessagesServer() {}

        /**
            == Receive Messages Connecting ==
        */
        void NetworkMessagesServer::ReceiveConnectionRequest(NetMessageServerClientConnectingFromClient& p_message, const DoremiEngine::Network::Adress& p_adress)
        {
            ClientConnectionFromServer* t_connection = nullptr;

            // If we have the adress we ignore (might be late packet?), and wait for timeout
            if(!NetworkConnectionsServer::GetInstance()->AdressWithPortExist(p_adress, t_connection))
            {
                NetworkConnectionsServer::GetInstance()->CreateNewConnecting(p_adress);

                // Send version check message
                SendVersionCheck(p_adress);
            }
            else if(t_connection->ConnectionState == ClientConnectionStateFromServer::VERSION_CHECK)
            {
                // Send version check message
                SendVersionCheck(p_adress);
            }
        }


        void NetworkMessagesServer::ReceiveVersionCheck(NetMessageServerClientConnectingFromClient& p_message, const DoremiEngine::Network::Adress& p_adress)
        {
            ClientConnectionFromServer* t_connection = nullptr;

            // If we have the adress we ignore (might be late packet?), and wait for timeout
            if(NetworkConnectionsServer::GetInstance()->AdressWithPortExist(p_adress, t_connection))
            {
                // If the state is version checking, we check if version check is ok and turn connection to connect, else we let it timeout
                if(t_connection->ConnectionState == ClientConnectionStateFromServer::VERSION_CHECK)
                {
                    // Change State to...
                    t_connection->ConnectionState = ClientConnectionStateFromServer::CONNECT;
                    t_connection->LastResponse = 0;

                    // Ready for read
                    NetworkStreamer t_streamer = NetworkStreamer();
                    unsigned char* t_dataPointer = p_message.Data;
                    t_streamer.SetTargetBuffer(t_dataPointer, sizeof(p_message.Data));

                    // Read playerID from player
                    uint32_t t_playerID = t_streamer.ReadUnsignedInt32();

                    // Get playerhandler
                    PlayerHandlerServer* t_playerHandler = static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance());

                    // Check if we have saved data for player
                    if(!t_playerHandler->InactivePlayerIDExists(t_playerID))
                    {
                        // TODO change better way of generation random unique number, then checking against existing
                        t_playerID = rand();

                        // Generate a playerID we don't already use
                        while(t_playerHandler->ActivePlayerIDExists(t_playerID))
                        {
                            t_playerID = rand();
                        }
                    }

                    // Assign playerID to connection
                    t_connection->MyPlayerID = t_playerID;

                    // Send Connected Message
                    SendConnect(t_connection, p_adress);
                }
                else if(t_connection->ConnectionState == ClientConnectionStateFromServer::CONNECT)
                {
                    // Send Connected Message
                    SendConnect(t_connection, p_adress);
                }
            }
        }

        void NetworkMessagesServer::ReceiveDisconnect(NetMessageServerClientConnectingFromClient& p_message, DoremiEngine::Network::Adress& p_adress)
        {
            ClientConnectionFromServer* t_connection = nullptr;

            // If we have the adress we ignore (might be late packet?), and wait for timeout
            if(NetworkConnectionsServer::GetInstance()->AdressWithPortExist(p_adress, t_connection))
            {
                std::cout << "Received disconnect! This might be a problem if not intentinal..." << std::endl;
                t_connection->LastResponse = 10000000; // Hard coded to disconnect
            }
        }

        /**
            == Receive Messages Connected ==
        */

        void NetworkMessagesServer::ReceiveConnected(NetMessageServerClientConnectedFromClient& p_message, ClientConnectionFromServer* p_connection)
        {
            // If we're connected stage
            if(p_connection->ConnectionState == ClientConnectionStateFromServer::CONNECTED)
            {
                p_connection->LastResponse = 0;
            }
        }

        void NetworkMessagesServer::ReceiveLoadWorld(NetMessageServerClientConnectedFromClient& p_message, ClientConnectionFromServer* p_connection)
        {
            if(p_connection->ConnectionState == ClientConnectionStateFromServer::LOAD_WORLD)
            {
                // Reset last response
                p_connection->LastResponse = 0;

                // Ready for read
                NetworkStreamer p_streamer = NetworkStreamer();
                unsigned char* p_bufferPointer = p_message.Data;
                p_streamer.SetTargetBuffer(p_bufferPointer, sizeof(p_message.Data));

                // Amount read
                uint32_t bytesRead = 0;

                // Read acced event
                uint32_t eventAcc = p_streamer.ReadUnsignedInt32();
                bytesRead += sizeof(uint32_t);


                // TODOCM add bool here if player has done loading-loading world

                // Acc rejoin by acced event
                bool t_receivedAllEvents =
                    static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->UpdateRejoinQueueForPlayer(eventAcc, p_connection->MyPlayerID);

                // If client is ready
                if(t_receivedAllEvents)
                {
                    // TODOCM log
                    std::cout << "A player finished loading the map..." << std::endl;
                    p_connection->ConnectionState = ClientConnectionStateFromServer::IN_GAME;

                    // Update all network objects, so that all objects attempt to send their stuff right away
                    static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->GetNetworkPriorityHandlerForplayer(p_connection->MyPlayerID)->UpdateAllNetworkObject();
                }
            }
        }
        //////////////////// Should have some init sequence
        void NetworkMessagesServer::ReceiveInGame(NetMessageServerClientConnectedFromClient& p_message, ClientConnectionFromServer* p_connection)
        {
            if(p_connection->ConnectionState == ClientConnectionStateFromServer::IN_GAME)
            {
                // Reset last response
                p_connection->LastResponse = 0;

                // Get input handler and frequencyhandler
                PlayerHandlerServer* t_playerHandler = static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance());
                InputHandlerServer* t_inputHandler = t_playerHandler->GetInputHandlerForPlayer(p_connection->MyPlayerID);
                FrequencyBufferHandler* t_frequencyHandler = t_playerHandler->GetFrequencyBufferHandlerForPlayer(p_connection->MyPlayerID);
                NetworkEventSender* t_networkEventSender = t_playerHandler->GetNetworkEventSenderForPlayer(p_connection->MyPlayerID);

                // Ready for read
                NetworkStreamer p_streamer = NetworkStreamer();
                unsigned char* p_bufferPointer = p_message.Data;
                p_streamer.SetTargetBuffer(p_bufferPointer, sizeof(p_message.Data));

                // Bytes read counter
                uint32_t t_bytesRead = 0;

                // Read sequence
                uint8_t t_newSequence = p_streamer.ReadUnsignedInt8();
                t_bytesRead += sizeof(uint8_t);

                // Read input bitmask
                uint32_t t_inputMask = p_streamer.ReadUnsignedInt32();
                t_bytesRead += sizeof(uint32_t);

                // Get entityID for player
                EntityID entityID = 0;

                // Ignore checks, we should have a entityID
                t_playerHandler->GetEntityIDForPlayer(p_connection->MyPlayerID, entityID);

                // Read orientation to update with
                DirectX::XMFLOAT4 t_playerOrientation = p_streamer.ReadRotationQuaternion();
                t_bytesRead += sizeof(float) * 4;

                // If we're a new connection we save the first sequence
                if(p_connection->LastSequenceUpdate >= SEQUENCE_UPDATE_TIMER)
                {
                    // MOVE THOSE..... not sure where yet
                    p_connection->LastSequenceUpdate = 0.0f;
                    t_inputHandler->SetSequence(t_newSequence);
                    t_frequencyHandler->SetSequence(t_newSequence);
                }

                // Queue input with sequence
                t_inputHandler->QueueInput(t_inputMask, t_playerOrientation, t_newSequence);

                // Read event acc sequence
                uint8_t t_clientEventSequence = p_streamer.ReadUnsignedInt8();
                t_bytesRead += sizeof(uint8_t);

                // Update event queue with acc sequence
                t_networkEventSender->UpdateBufferWithRecievedClientSequenceAcc(t_clientEventSequence);

                // Read frequency
                float t_frequency = p_streamer.ReadFloat();
                t_bytesRead += sizeof(float);
                t_frequencyHandler->QueueFrequency(t_frequency, t_newSequence);
            }
        }

        /**
            == Send Messages Connecting ==
        */

        void NetworkMessagesServer::SendVersionCheck(const DoremiEngine::Network::Adress& p_adress)
        {
            std::cout << "Sending version check" << std::endl; // TODOCM remove deubgg
            // Create message
            NetMessageServerClientConnectingFromServer t_newMessage = NetMessageServerClientConnectingFromServer();
            t_newMessage.MessageID = SendMessageIDToClientFromServer::VERSION_CHECK;

            // TODOCM add version check info?

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage),
                                                                  NetworkConnectionsServer::GetInstance()->GetConnectingSocketHandle(), &p_adress);
        }

        void NetworkMessagesServer::SendConnect(const ClientConnectionFromServer* p_connection, const DoremiEngine::Network::Adress& p_adress)
        {
            // Create version check message
            NetMessageServerClientConnectingFromServer t_newMessage = NetMessageServerClientConnectingFromServer();
            t_newMessage.MessageID = SendMessageIDToClientFromServer::CONNECT;

            // TODOCM add info - like port etc...

            // Ready for write
            NetworkStreamer p_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = t_newMessage.Data;
            p_streamer.SetTargetBuffer(t_bufferPointer, sizeof(t_newMessage.Data));

            // Write playerID
            p_streamer.WriteUnsignedInt32(p_connection->MyPlayerID);

            // Write port to connect to
            p_streamer.WriteUnsignedInt32(NetworkConnectionsServer::GetInstance()->GetPortConnected());

            // Send connect message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage),
                                                                  NetworkConnectionsServer::GetInstance()->GetConnectingSocketHandle(), &p_adress);
        }

        void NetworkMessagesServer::SendDisconnect(const DoremiEngine::Network::Adress& p_adress, std::string p_string)
        {
            // Create disconnection message
            NetMessageServerClientConnectingFromServer t_newMessage = NetMessageServerClientConnectingFromServer();
            t_newMessage.MessageID = SendMessageIDToClientFromServer::DISCONNECT;

            // Ready for write
            NetworkStreamer p_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = t_newMessage.Data;
            p_streamer.SetTargetBuffer(t_bufferPointer, sizeof(t_newMessage.Data));

            // Write disconnect string
            p_streamer.WriteString(p_string);

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage),
                                                                  NetworkConnectionsServer::GetInstance()->GetConnectingSocketHandle(), &p_adress);
        }

        /**
            == Send Messages Connected ==
        */

        void NetworkMessagesServer::SendConnected(ClientConnectionFromServer* p_connection)
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();

            // Create connected message
            NetMessageServerClientConnectedFromServer t_newMessage = NetMessageServerClientConnectedFromServer();
            t_newMessage.MessageID = SendMessageIDToClientFromServer::CONNECTED;

            // Ready for write
            NetworkStreamer p_streamer = NetworkStreamer();
            unsigned char* p_bufferPointer = t_newMessage.Data;
            p_streamer.SetTargetBuffer(p_bufferPointer, sizeof(t_newMessage.Data));

            // Check if we should start game
            bool t_shouldStart = ServerStateHandler::GetInstance()->GetState() == ServerStates::IN_GAME;

            // Change state to load world TODOCM revalueate if we need to do this
            if(t_shouldStart)
            {
                p_connection->ConnectionState = ClientConnectionStateFromServer::LOAD_WORLD;
            }

            // Send the message
            t_networkModule.SendReliableData(&t_newMessage, sizeof(t_newMessage), p_connection->ConnectedSocketHandle);
        }

        void NetworkMessagesServer::SendLoadWorld(ClientConnectionFromServer* p_connection)
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();

            // Create new load world message
            NetMessageServerClientConnectedFromServer t_newMessage = NetMessageServerClientConnectedFromServer();
            t_newMessage.MessageID = SendMessageIDToClientFromServer::LOAD_WORLD;

            // Ready for write
            NetworkStreamer t_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = t_newMessage.Data;
            t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(t_newMessage.Data));

            // Counter for writes bitten
            uint32_t t_bytesWritten = 0;

            // Write max number of events for rejoin
            uint32_t t_maxNumberOfEvents = static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->GetMaxEventForPlayer(p_connection->MyPlayerID);
            t_streamer.WriteUnsignedInt32(t_maxNumberOfEvents);
            t_bytesWritten += sizeof(uint32_t);

            // Write join events
            static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())
                ->WriteQueuedEventsFromLateJoin(t_streamer, sizeof(t_newMessage.Data), t_bytesWritten, p_connection->MyPlayerID);

            // Send the message
            t_networkModule.SendReliableData(&t_newMessage, sizeof(t_newMessage), p_connection->ConnectedSocketHandle);
        }

        void NetworkMessagesServer::SendInGame(ClientConnectionFromServer* p_connection)
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();
            PlayerHandlerServer* t_playerHandler = static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance());
            InputHandlerServer* t_inputHandler = t_playerHandler->GetInputHandlerForPlayer(p_connection->MyPlayerID);
            FrequencyBufferHandler* t_frequencyHandler = t_playerHandler->GetFrequencyBufferHandlerForPlayer(p_connection->MyPlayerID);
            NetworkEventSender* t_networkEventSender = t_playerHandler->GetNetworkEventSenderForPlayer(p_connection->MyPlayerID);
            NetworkPriorityHandler* t_networkPriorityHandler = t_playerHandler->GetNetworkPriorityHandlerForplayer(p_connection->MyPlayerID);


            // Create message
            NetMessageServerClientConnectedFromServer t_newMessage = NetMessageServerClientConnectedFromServer();
            t_newMessage.MessageID = SendMessageIDToClientFromServer::IN_GAME;

            // Ready for write
            NetworkStreamer t_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = t_newMessage.Data;
            t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(t_newMessage.Data));

            // Bytes written counter
            uint32_t t_bytesWritten = 0;

            // Write snapshot ID (1 byte
            t_streamer.WriteUnsignedInt8(m_messageSequence);
            t_bytesWritten += sizeof(uint8_t);

            // Write client last sequence (1 byte
            t_streamer.WriteUnsignedInt8(t_inputHandler->GetSequenceByLastInput());
            t_bytesWritten += sizeof(uint8_t);

            // Write position of that sequence ( 12 byte
            t_streamer.WriteFloat3(t_inputHandler->GetPositionByLastInput());
            t_bytesWritten += sizeof(float) * 3;

            // Write events
            bool wroteAllEvents = false;
            t_networkEventSender->WriteEvents(t_streamer, sizeof(t_newMessage.Data), t_bytesWritten, wroteAllEvents);

            // If we wrote all events and have enough to write snapshot with player
            if(wroteAllEvents)
            {
                // Write objects from priority
                t_networkPriorityHandler->WriteObjectsByPriority(t_streamer, sizeof(t_newMessage.Data), t_bytesWritten);
            }
            else
            {
                // Write 0 objects, if we started to write something bad in the events
                t_streamer.WriteUnsignedInt8(0);
                t_bytesWritten += sizeof(uint8_t);
            }

            // Send the message
            bool t_sendFailed = !t_networkModule.SendReliableData(&t_newMessage, sizeof(t_newMessage), p_connection->ConnectedSocketHandle);

            // If fail, we disconnect next
            //if(t_sendFailed)
            //{
            //    std::cout << "Send failed, disconnecting" << std::endl;
            //    p_connection->LastResponse = 100000; // TODOCM hard coded disconnect value =D
            //}
        }


        void NetworkMessagesServer::ReceiveConnectedMaster(NetMessageMasterServerFromMaster& p_message)
        {
            NetworkConnectionsServer* t_connections = NetworkConnectionsServer::GetInstance();
            if(t_connections->m_masterConnection.ConnectionState > MasterConnectionStateFromServer::DISCONNECTED)
            {
                // If we're connecting we're now connected
                if(t_connections->m_masterConnection.ConnectionState == MasterConnectionStateFromServer::CONNECTING)
                {
                    t_connections->m_masterConnection.ConnectionState = MasterConnectionStateFromServer::CONNECTED;
                }

                // Ready for read
                NetworkStreamer t_streamer = NetworkStreamer();
                unsigned char* p_bufferPointer = p_message.Data;
                t_streamer.SetTargetBuffer(p_bufferPointer, sizeof(p_message.Data));

                // Read sequence ack
                t_connections->m_masterConnection.AckSequence = t_streamer.ReadUnsignedInt8();

                // Update last reposnse
                t_connections->m_masterConnection.LastResponse = 0;
            }
        }

        void NetworkMessagesServer::ReceiveDisconnectMaster(NetMessageMasterServerFromMaster& p_message)
        {
            NetworkConnectionsServer* t_connections = NetworkConnectionsServer::GetInstance();
            if(t_connections->m_masterConnection.ConnectionState > MasterConnectionStateFromServer::DISCONNECTED)
            {
                // If we were connected or something above(future), we retry connecting
                if(t_connections->m_masterConnection.ConnectionState >= MasterConnectionStateFromServer::CONNECTED)
                {
                    t_connections->m_masterConnection.ConnectionState = MasterConnectionStateFromServer::CONNECTING;
                }
            }
        }

        void NetworkMessagesServer::SendConnectionRequestMaster()
        {
            // Create connection request message
            NetworkConnectionsServer* t_connections = NetworkConnectionsServer::GetInstance();
            ServerStateHandler* t_serverStateHandler = ServerStateHandler::GetInstance();

            NetMessageMasterServerFromServer t_newMessage = NetMessageMasterServerFromServer();
            t_newMessage.MessageID = SendMessageIDToMasterFromServer::CONNECTION_REQUEST;

            // Ready for write
            NetworkStreamer t_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = t_newMessage.Data;
            t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(t_newMessage.Data));

            t_streamer.WriteStringShort(t_serverStateHandler->GetServerName()); // 20 bytes
            t_streamer.WriteUnsignedInt8(static_cast<uint8_t>(t_serverStateHandler->GetState())); // 1 byte
            t_streamer.WriteUnsignedInt8(static_cast<uint8_t>(t_serverStateHandler->GetMap())); // 1 byte

            uint8_t t_currentNumClients = t_connections->GetConnectedClientConnections().size();
            t_streamer.WriteUnsignedInt8(t_currentNumClients); // 1 byte
            t_streamer.WriteUnsignedInt8(t_serverStateHandler->GetMaxPlayers()); // 1 byte

            t_streamer.WriteUnsignedInt16(t_connections->GetPortConnecting()); // 2 byte

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage), t_connections->m_masterConnection.ConnectedSocketHandle,
                                                                  t_connections->m_masterConnection.Adress);
        }

        void NetworkMessagesServer::SendConnectedMaster()
        {
            // Create connected message
            NetworkConnectionsServer* t_connections = NetworkConnectionsServer::GetInstance();
            NetMessageMasterServerFromServer t_newMessage = NetMessageMasterServerFromServer();
            t_newMessage.MessageID = SendMessageIDToMasterFromServer::CONNECTED;

            // Ready for write
            NetworkStreamer t_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = t_newMessage.Data;
            t_streamer.SetTargetBuffer(t_bufferPointer, sizeof(t_newMessage.Data));

            // Write ack sequence
            t_streamer.WriteUnsignedInt8(t_connections->m_masterConnection.AckSequence);

            // Write current state
            ServerStates t_serverState = ServerStateHandler::GetInstance()->GetState();
            t_streamer.WriteUnsignedInt8(static_cast<uint8_t>(t_serverState));

            // Write current number of clients
            uint8_t t_currentNumClients = t_connections->GetConnectedClientConnections().size();
            t_streamer.WriteUnsignedInt8(t_currentNumClients);

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage), t_connections->m_masterConnection.ConnectedSocketHandle,
                                                                  t_connections->m_masterConnection.Adress);
        }

        void NetworkMessagesServer::SendDisconnectMaster()
        {
            // Create disconnect message
            NetworkConnectionsServer* t_connections = NetworkConnectionsServer::GetInstance();
            NetMessageMasterServerFromServer t_newMessage = NetMessageMasterServerFromServer();
            t_newMessage.MessageID = SendMessageIDToMasterFromServer::DISCONNECT;

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage), t_connections->m_masterConnection.ConnectedSocketHandle,
                                                                  t_connections->m_masterConnection.Adress);
        }
    }
}