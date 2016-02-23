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

#include <Doremi/Core/Include/NetworkEventSender.hpp>

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
            == Receive Messages Unreliable ==
        */
        void NetworkMessagesServer::ReceiveConnectionRequest(NetMessageConnectingFromClient& p_message, const DoremiEngine::Network::Adress& p_adress)
        {
            ClientConnectionFromServer* t_connection = nullptr;

            // If we have the adress we ignore (might be late packet?), and wait for timeout
            if(!NetworkConnectionsServer::GetInstance()->AdressExist(p_adress, t_connection))
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


        void NetworkMessagesServer::ReceiveVersionCheck(NetMessageConnectingFromClient& p_message, const DoremiEngine::Network::Adress& p_adress)
        {
            ClientConnectionFromServer* t_connection = nullptr;

            // If we have the adress we ignore (might be late packet?), and wait for timeout
            if(NetworkConnectionsServer::GetInstance()->AdressExist(p_adress, t_connection))
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
                    t_connection->PlayerID = t_playerID;

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

        void NetworkMessagesServer::ReceiveDisconnect(NetMessageConnectingFromClient& p_message, DoremiEngine::Network::Adress& p_adress)
        {
            ClientConnectionFromServer* t_connection = nullptr;

            // If we have the adress we ignore (might be late packet?), and wait for timeout
            if(NetworkConnectionsServer::GetInstance()->AdressExist(p_adress, t_connection))
            {
                NetworkConnectionsServer::GetInstance()->RemoveConnection(p_adress);
            }
        }

        /**
            == Receive Messages Reliable ==
        */

        void NetworkMessagesServer::ReceiveConnectedMessage(NetMessageConnectedFromClient& p_message, ClientConnectionFromServer* p_connection)
        {
            // doesn't exit yet, do we need this?...
        }

        void NetworkMessagesServer::ReceiveLoadWorldMessage(NetMessageConnectedFromClient& p_message, ClientConnectionFromServer* p_connection)
        {
            if(p_connection->ConnectionState == ClientConnectionStateFromServer::LOAD_WORLD)
            {
                // Ready for read
                NetworkStreamer p_streamer = NetworkStreamer();
                unsigned char* p_bufferPointer = p_message.Data;
                p_streamer.SetTargetBuffer(p_bufferPointer, sizeof(p_message.Data));

                // Amount read
                uint32_t bytesRead = 0;

                // Read acced event
                uint32_t eventAcc = p_streamer.ReadUnsignedInt32();
                bytesRead += sizeof(uint32_t);

                // Acc rejoin by acced event
                bool t_receivedAllEvents =
                    static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->UpdateRejoinQueueForPlayer(eventAcc, p_connection->PlayerID);

                // If client is ready
                if(t_receivedAllEvents)
                {
                    // TODOCM log
                    // cout << "A player finished loading the map..." << endl;
                    p_connection->ConnectionState = ClientConnectionStateFromServer::IN_GAME;

                    // Update all network objects, so that all objects attempt to send their stuff right away
                    static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->GetNetworkPriorityHandlerForplayer(p_connection->PlayerID)->UpdateAllNetworkObject();
                }
            }
        }
        //////////////////// Should have some init sequence
        void NetworkMessagesServer::ReceiveInGameMessage(NetMessageConnectedFromClient& p_message, ClientConnectionFromServer* p_connection)
        {
            if(p_connection->ConnectionState == ClientConnectionStateFromServer::IN_GAME)
            {
                // Get input handler and frequencyhandler
                InputHandlerServer* t_inputHandler = (InputHandlerServer*)PlayerHandler::GetInstance()->GetInputHandlerForPlayer(p_connection->PlayerID);
                FrequencyBufferHandler* t_frequencyHandler = PlayerHandler::GetInstance()->GetFrequencyBufferHandlerForPlayer(p_connection->PlayerID);
                NetworkEventSender* t_networkEventSender =
                    (static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance()))->GetNetworkEventSenderForPlayer(p_connection->PlayerID);

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
                PlayerHandler::GetInstance()->GetEntityIDForPlayer(p_connection->PlayerID, entityID);

                // Read orientation to update with
                DirectX::XMFLOAT4 t_playerOrientation = p_streamer.ReadRotationQuaternion();
                t_bytesRead += sizeof(float) * 4;

                // Queue input with sequence
                t_inputHandler->QueueInput(t_inputMask, t_playerOrientation, t_newSequence);

                // Read event acc sequence
                uint8_t t_clientEventSequence = p_streamer.ReadUnsignedInt8();
                t_bytesRead += sizeof(uint8_t);

                // Update event queue with acc sequence
                t_networkEventSender->UpdateBufferWithRecievedClientSequenceAcc(t_clientEventSequence);

                // Read frequencies
                t_frequencyHandler->ReadNewFrequencies(p_streamer, sizeof(p_message.Data), t_bytesRead);
            }
        }

        /**
            == Send Messages Unreliable ==
        */

        void NetworkMessagesServer::SendVersionCheck(const DoremiEngine::Network::Adress& p_adress)
        {
            // Create message
            NetMessageConnectingFromServer t_newMessage = NetMessageConnectingFromServer();
            t_newMessage.MessageID = SendMessageIDFromServer::VERSION_CHECK;

            // TODOCM add version check info?

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage),
                                                                  NetworkConnectionsServer::GetInstance()->GetConnectingSocketHandle(), &p_adress);
        }

        void NetworkMessagesServer::SendConnect(const ClientConnectionFromServer* p_connection, const DoremiEngine::Network::Adress& p_adress)
        {
            // Create version check message
            NetMessageConnectingFromServer t_newMessage = NetMessageConnectingFromServer();
            t_newMessage.MessageID = SendMessageIDFromServer::CONNECT;

            // TODOCM add info - like port etc...

            // Ready for write
            NetworkStreamer p_streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = t_newMessage.Data;
            p_streamer.SetTargetBuffer(t_bufferPointer, sizeof(t_newMessage.Data));

            // Write playerID
            p_streamer.WriteUnsignedInt32(p_connection->PlayerID);

            // Write port to connect to
            p_streamer.WriteUnsignedInt32(NetworkConnectionsServer::GetInstance()->GetPortConnected());

            // Send connect message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&t_newMessage, sizeof(t_newMessage),
                                                                  NetworkConnectionsServer::GetInstance()->GetConnectingSocketHandle(), &p_adress);
        }

        void NetworkMessagesServer::SendDisconnect(const DoremiEngine::Network::Adress& p_adress, std::string p_string)
        {
            // Create disconnection message
            NetMessageConnectingFromServer t_newMessage = NetMessageConnectingFromServer();
            t_newMessage.MessageID = SendMessageIDFromServer::DISCONNECT;

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
            == Send Messages Reliable ==
        */

        void NetworkMessagesServer::SendConnected(ClientConnectionFromServer* p_connection)
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();

            // Create connected message
            NetMessageConnectedFromServer t_newMessage = NetMessageConnectedFromServer();
            t_newMessage.MessageID = SendMessageIDFromServer::CONNECTED;

            // Ready for write
            NetworkStreamer p_streamer = NetworkStreamer();
            unsigned char* p_bufferPointer = t_newMessage.Data;
            p_streamer.SetTargetBuffer(p_bufferPointer, sizeof(t_newMessage.Data));

            // Write max number of events for rejoin
            uint32_t t_maxNumberOfEvents = static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->GetMaxEventForPlayer(p_connection->PlayerID);
            p_streamer.WriteUnsignedInt32(t_maxNumberOfEvents);

            // Start game YES
            p_streamer.WriteBool(true);
        }


        void ServerNetworkManager::SendConnected(Connection* p_connection)
        {
            DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();

            NetMessage Message = NetMessage();

            Message.MessageID = MessageID::CONNECTED;

            NetworkStreamer Streamer = NetworkStreamer();
            unsigned char* BufferPointer = Message.Data;
            Streamer.SetTargetBuffer(BufferPointer, sizeof(Message.Data));

            Streamer.WriteUnsignedInt32(static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->GetMaxEventForPlayer(p_connection->PlayerID));

            // Start game YES
            Streamer.WriteBool(true);

            p_connection->ConnectionState = ConnectionState::MAP_LOADING;

            // Send message
            NetworkModule.SendReliableData(&Message, sizeof(Message), p_connection->ReliableSocketHandle);
        }

        void NetworkMessagesServer::SendLoadWorld(Connection* p_connection) {}

        void NetworkMessagesServer::SendInGame(NetMessage& p_message, Connection* p_connection)
        {
            // Check name
        }


        // TODOCM maybe move to somewhere else
        void ServerNetworkManager::CreateSnapshot(unsigned char* p_buffer, uint32_t p_bufferSize, Connection* p_connection)
        {
            // For all objects
            // That have a position component
            // That has a flag that it is active
            // We put info to render
            InputHandlerServer* inputHandler = (InputHandlerServer*)PlayerHandler::GetInstance()->GetInputHandlerForPlayer(p_connection->PlayerID);

            NetworkStreamer Streamer = NetworkStreamer();
            Streamer.SetTargetBuffer(p_buffer, p_bufferSize);

            uint32_t BytesWritten = 0;

            // Write sequence acc for frequence
            uint8_t sequenceAcc = PlayerHandler::GetInstance()->GetFrequencyBufferHandlerForPlayer(p_connection->PlayerID)->GetNextSequenceUsed();

            // Write snapshot sequence
            Streamer.WriteUnsignedInt8(sequenceAcc);
            BytesWritten += sizeof(uint8_t);

            // Write snapshot ID (1 byte
            Streamer.WriteUnsignedInt8(m_nextSnapshotSequence);
            BytesWritten += sizeof(uint8_t);

            // Add new Add/Remove items
            bool wroteAllEvents = false;
            static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())
                ->GetNetworkEventSenderForPlayer(p_connection->PlayerID)
                ->WriteEvents(Streamer, p_bufferSize, BytesWritten, wroteAllEvents);

            // If we have enough to write snapshot with player
            if(p_bufferSize - BytesWritten < sizeof(uint8_t) + sizeof(float) * 3)
            {
                return;
            }

            // Write client last sequence (1 byte
            Streamer.WriteUnsignedInt8(inputHandler->GetSequenceByLastInput());

            // Write position of that sequence ( 12 byte
            Streamer.WriteFloat3(inputHandler->GetPositionByLastInput());

            BytesWritten += sizeof(uint8_t) + sizeof(float) * 3;

            // Get networkPriorityHandler
            NetworkPriorityHandler* netPrioHandler =
                static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->GetNetworkPriorityHandlerForplayer(p_connection->PlayerID);

            // Write objects from priority
            netPrioHandler->WriteObjectsByPriority(Streamer, p_bufferSize, BytesWritten);
        }

        void ServerNetworkManager::SendMessages(double p_dt)
        {
            // Remove time
            m_nextUpdateTimer -= m_updateInterval;

            // Update sequence here because of the error checking..
            m_nextSnapshotSequence++;


            // For all connected clients we send messages
            for(std::map<DoremiEngine::Network::Adress*, Connection*>::iterator iter = m_connections.begin(); iter != m_connections.end(); ++iter)
            {
                if(iter->second->ConnectionState >= ConnectionState::CONNECTED)
                {
                    // Create global message
                    NetMessage Message = NetMessage();
                    Message.MessageID = MessageID::SNAPSHOT;


                    // TODOCM add snapshot info here
                    // TODOCM Now we always create a snapshot, might want to change this by a state of the server?
                    CreateSnapshot(Message.Data, sizeof(Message.Data), iter->second);

                    switch(iter->second->ConnectionState)
                    {
                        case ConnectionState::CONNECTED:

                            SendConnected(iter->second);
                            break;

                        case ConnectionState::MAP_LOADING:

                            SendLoadWorld(iter->second);
                            break;

                        case ConnectionState::IN_GAME:

                            SendInGame(Message, iter->second);
                            break;

                        default:
                            break;
                    }
                }
            }
        }


        void ServerNetworkManager::SendLoadWorld(Connection* p_connection)
        {
            DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();

            NetMessage Message = NetMessage();

            // Set message ID
            Message.MessageID = MessageID::LOAD_WORLD;


            // TODOCM send all add/removed stuff here
            NetworkStreamer Streamer = NetworkStreamer();
            unsigned char* bufferPointer = Message.Data;
            Streamer.SetTargetBuffer(bufferPointer, sizeof(Message.Data));

            uint32_t t_bytesWritten = 0;

            // Write events
            static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())
                ->WriteQueuedEventsFromLateJoin(Streamer, sizeof(Message.Data), t_bytesWritten, p_connection->PlayerID);

            // Send message
            NetworkModule.SendReliableData(&Message, sizeof(Message), p_connection->ReliableSocketHandle);
        }

        void ServerNetworkManager::SendInGame(NetMessage& p_message, Connection* p_connection)
        {
            DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();

            // If we're a new connection we send a initialise snapshot, might need this later
            if(p_connection->NewConnection)
            {
                p_message.MessageID = MessageID::INIT_SNAPSHOT;
                p_connection->NewConnection = false;
                ((InputHandlerServer*)PlayerHandler::GetInstance()->GetInputHandlerForPlayer(p_connection->PlayerID))->SetSequence(m_nextSnapshotSequence);
            }
            else
            {
                counter++; // TODOCM remove test
                int checkValue;

                if(counter < 30)
                {
                    checkValue = 100;
                }
                else
                {

                    counter = -1;
                    checkValue = -1;
                }

                // TODOCM remove, packet loss experiment

                if(100 == checkValue)
                {
                    // return;
                }
            }

            // Send message
            if(!NetworkModule.SendReliableData(&p_message, sizeof(p_message), p_connection->ReliableSocketHandle))
            {
                cout << "Failed to send, disconnecting..." << endl;
                p_connection->LastResponse = 100000;
            }
        }

        void ServerNetworkManager::CheckForConnections()
        {
            DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();
            size_t OutSocketID = 0;
            DoremiEngine::Network::Adress* OutAdress =
                NetworkModule.CreateAdress(); // TODOCM really really bad, dynamically allocating every frame, should do something about this

            // Check if anyone is attempting to accept on our channel
            if(NetworkModule.AcceptConnection(m_reliableSocketHandle, OutSocketID, OutAdress))
            {
                bool foundConnection = false;

                // Check if we have any connection like that
                std::map<DoremiEngine::Network::Adress*, Connection*>::iterator iter;
                for(iter = m_connections.begin(); iter != m_connections.end(); ++iter)
                {
                    // Custom check for only adress the same ( not port)
                    if(*iter->first *= *OutAdress)
                    {
                        // If the client trying to connect is at the right stage connect
                        if(iter->second->ConnectionState == ConnectionState::VERSION_CHECK)
                        {
                            // Set state as connected
                            iter->second->ConnectionState = ConnectionState::CONNECTED;

                            // Update last response
                            iter->second->LastResponse = 0;

                            // Add socketID
                            iter->second->ReliableSocketHandle = OutSocketID;

                            iter->second->NewConnection = true;

                            foundConnection = true;

                            // Create new InputHandler
                            InputHandlerServer* NewInputHandler = new InputHandlerServer(m_sharedContext, DirectX::XMFLOAT3(0, 0, 0));

                            // Create player
                            PlayerHandler::GetInstance()->CreateNewPlayer(iter->second->PlayerID, NewInputHandler);


                            break;
                        }
                        // I think this didn't work cause we connected from same computer = same IP, but different games, how do we tell apart?
                        // else // If not, either wrong stage or something happened, or bot.. problem here is that if we play from 2 clients.. if
                        // wrong stage it will
                        //{
                        //    // Send disconnect message
                        //    SendDisconnect(*OutAdress, "Bad adress/pattern in check for connections");

                        //    // Set their state to disconnected
                        //    iter->second->ConnectionState = ConnectionState::DISCONNECTED;

                        //    iter->second->NewConnection = false;
                        //}
                        //// Break loop
                        //// TODOCM maybe send disconnect even if we dont have him in list?
                    }
                }

                // If we didn't find connection, close socket
                if(!foundConnection)
                {
                    // TODOCM remove socket
                }
            }

            // Delete socketadress
            delete OutAdress;
        }
    }
}