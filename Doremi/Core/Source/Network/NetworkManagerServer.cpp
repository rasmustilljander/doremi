// Project specific
#include <Doremi/Core/Include/Network/NetworkManagerServer.hpp>

// Modules
#include <DoremiEngine/Network/Include/NetworkModule.hpp>

// Streamer
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>

// Handlers
#include <Doremi/Core/Include/PlayerHandlerServer.hpp>

// Net messages
#include <Doremi/Core/Include/Network/NetMessages.hpp>
#include <Doremi/Core/Include/Network/NetworkMessagesServer.hpp>

// Connections
#include <Doremi/Core/Include/Network/NetworkConnectionsServer.hpp>

// Standard
#include <iostream> // TODOCM remove after test
#include <vector>
#include <algorithm>
#include <time.h>

namespace Doremi
{
    namespace Core
    {
        NetworkManagerServer::NetworkManagerServer(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "ServerNetworkManager"),
              m_nextUpdateTimer(0.0f),
              m_updateInterval(0.017f),
              m_timeoutInterval(3.0f),
              m_maxConnection(16),
              m_nextSnapshotSequence(0),
              m_maxConnectMessagesPerFrame(20),
              m_maxConnectedMessagesPerFrame(10),
        {
            DoremiEngine::Network::NetworkModule& NetworkModule = p_sharedContext.GetNetworkModule();

            // Create adress for ALL incomming IP and port 5050
            DoremiEngine::Network::Adress* UnreliableAdress = NetworkModule.CreateAdress(5050);

            // Create adress for ALL incomming IP and port 4050
            DoremiEngine::Network::Adress* ReliableAdress = NetworkModule.CreateAdress(4050);

            // Create socket for unrealiable
            m_unreliableSocketHandle = NetworkModule.CreateUnreliableWaitingSocket(UnreliableAdress);

            // Create socket for relialbe
            m_reliableSocketHandle = NetworkModule.CreateReliableConnection(ReliableAdress, m_maxConnection);

            counter = 0;

            srand(time(NULL));
        }

        NetworkManagerServer::~NetworkManagerServer() {}

        void NetworkManagerServer::Update(double p_dt)
        {
            // Receive Messages
            ReceiveMessages();

            // Send Messages
            SendMessages();

            // Check for connections TODOCM not sure if I want this if we going for UDP only
            CheckForConnections();

            // Update timeouts
            UpdateTimeouts(p_dt);
        }

        void NetworkManagerServer::ReceiveMessages()
        {
            // For some incomming connecting recieved messages we send one
            ReceiveConnectingMessages();

            // Recieve connecting messages from connected clients
            ReceiveConnectedMessages();
        }

        void NetworkManagerServer::ReceiveConnectingMessages()
        {
            // Get Nework module
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();

            // Create adress we can use, we don't know the incomming adress before we receive message
            // We delete this end of function
            DoremiEngine::Network::Adress* t_incommingAdress = t_networkModule.CreateAdress();

            // Create buffer NetworkMessage
            NetMessageBuffer t_networkMessage = NetMessageBuffer();

            // How much data we received
            uint32_t t_dataSizeReceived = 0;

            // Check for incomming messages
            size_t t_NumOfMessagesReceived = 0;
            while(t_networkModule.RecieveUnreliableData(&t_networkMessage, sizeof(t_networkMessage), m_unreliableSocketHandle, t_incommingAdress, t_dataSizeReceived) &&
                  t_NumOfMessagesReceived < m_maxConnectingMessagesPerFrame)
            {
                // Increment number of messages received
                t_NumOfMessagesReceived++;

                // If we don't have of that size
                if(t_dataSizeReceived != sizeof(NetMessageConnectingFromClient))
                {
                    // Null message and conitinue
                    t_networkMessage = NetMessageBuffer();
                    continue;
                }

                std::cout << "Recieved unreliable messsage: "; // TODOCM logg instead
                NetMessageConnectingFromClient& t_netMessageConnecting = *reinterpret_cast<NetMessageConnectingFromClient*>(&t_networkMessage);

                // Switch on what kind of message
                switch(t_netMessageConnecting.MessageID)
                {
                    case SendMessageIDFromClient::CONNECTION_REQUEST:
                    {
                        std::cout << "Connection Request." << std::endl; // TODOCM logg instead
                        NetworkMessagesServer::ReceiveConnectionRequest(t_netMessageConnecting, *t_incommingAdress);

                        break;
                    }
                    case SendMessageIDFromClient::VERSION_CHECK:
                    {
                        std::cout << "Version Check" << std::endl; // TODOCM logg instead
                        NetworkMessagesServer::ReceiveVersionCheck(t_netMessageConnecting, *t_incommingAdress);

                        break;
                    }
                    case SendMessageIDFromClient::DISCONNECT:
                    {
                        std::cout << "Disconnect" << std::endl; // TODOCM logg instead
                        NetworkMessagesServer::ReceiveDisconnect(t_netMessageConnecting, *t_incommingAdress);

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                // Reset message
                t_networkMessage = NetMessageBuffer();
            }

            // Delete the adress holder
            delete t_incommingAdress;
        }

        void NetworkManagerServer::ReceiveConnectedMessages()
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = m_sharedContext.GetNetworkModule();

            // For each connection
            auto& t_connectedClientConnections = NetworkConnectionsServer::GetInstance()->GetConnectedClientConnections();
            for(auto& t_connection : t_connectedClientConnections)
            {
                // Create message
                NetMessageBuffer t_message = NetMessageBuffer();

                // Max message counter
                uint32_t t_messageCounter = 0;

                // If we're connected
                if(t_connection.second->ConnectionState >= ClientConnectionStateFromServer::CONNECTED)
                {
                    // While we have something to receive and still less then max messages per frame
                    while(t_networkModule.RecieveReliableData(&t_message, sizeof(t_message), t_connection.second->ConnectedSocketHandle) &&
                          t_messageCounter < m_maxConnectedMessagesPerFrame)
                    {
                        NetMessageConnectedFromClient& t_connectedMessage = *reinterpret_cast<NetMessageConnectedFromClient*>(&t_message);

                        // Interpet message based on type
                        switch(t_connectedMessage.MessageID)
                        {
                            case SendMessageIDFromClient::CONNECTED:
                            {
                                NetworkMessagesServer::ReceiveConnectedMessage(t_connectedMessage, t_connection.second);

                                break;
                            }
                            case SendMessageIDFromClient::LOAD_WORLD:
                            {
                                NetworkMessagesServer::ReceiveLoadWorldMessage(t_connectedMessage, t_connection.second);

                                break;
                            }
                            case SendMessageIDFromClient::IN_GAME:
                            {
                                NetworkMessagesServer::ReceiveInGameMessage(t_connectedMessage, t_connection.second);

                                break;
                            }
                            default:
                            {
                                break;
                            }
                        }

                        // Reset ping timer
                        iter->second->LastResponse = 0;

                        // Reset message
                        t_message = NetMessageBuffer();
                    }
                }
            }
        }

        void NetworkManagerServer::UpdateTimeouts(double t_dt)
        {
            // Check all connected connections
            {
                auto& t_connectedConnections = NetworkConnectionsServer::GetInstance()->GetConnectedClientConnections();
                auto t_connection = t_connectedConnections.begin();

                while(t_connection != t_connectedConnections.end())
                {
                    // Update timer
                    t_connection->second->LastResponse += t_dt;

                    // If exceed timout
                    if(t_connection->second->LastResponse >= m_timeoutInterval)
                    {
                        // Send disconnection message
                        NetworkMessagesServer::SendDisconnect(*t_connection->first, "Timeout");

                        // Remove socket
                        m_sharedContext.GetNetworkModule().DeleteSocket(iter->second->ReliableSocketHandle);

                        // Remove and save player if it exists, it should
                        static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->RemoveAndSavePlayer(iter->second->PlayerID);

                        // Delete the memory here
                        delete t_connection->first;
                        delete t_connection->second;

                        // Erase from map
                        t_connection = t_connectedConnections.erase(t_connection);
                    }
                    else
                    {
                        // Else just increment
                        t_connection++;
                    }
                }
            }

            // Check all connecting connections
            {
                auto& t_connectingConnections = NetworkConnectionsServer::GetInstance()->GetConnectingClientConnections();
                auto t_connection = t_connectingConnections.begin();

                while(t_connection != t_connectingConnections.end())
                {
                    // Update timer
                    t_connection->second->LastResponse += t_dt;

                    // If exceed timout
                    if(t_connection->second->LastResponse >= m_timeoutInterval)
                    {
                        // Send disconnection message
                        NetworkMessagesServer::SendDisconnect(*t_connection->first, "Timeout");

                        // Remove socket
                        m_sharedContext.GetNetworkModule().DeleteSocket(iter->second->ReliableSocketHandle);

                        // Delete the memory here
                        delete t_connection->first;
                        delete t_connection->second;

                        // Erase from map
                        t_connection = t_connectingConnections.erase(t_connection);
                    }
                    else
                    {
                        // Else just increment
                        t_connection++;
                    }
                }
            }
        }

        void ServerNetworkManager::ReceiveLoadWorldMessage(NetMessage& p_message, Connection* p_connection)
        {
            if(p_connection->ConnectionState == ConnectionState::MAP_LOADING)
            {
                // Create a stream
                NetworkStreamer Streamer = NetworkStreamer();

                // Set message buffer to stream
                unsigned char* BufferPointer = p_message.Data;
                Streamer.SetTargetBuffer(BufferPointer, sizeof(p_message.Data));

                // Amount read
                uint32_t bytesRead = 0;

                // Read acced event
                uint32_t eventAcc = Streamer.ReadUnsignedInt32();

                // Acc rejoin event
                bool FullyLoaded = static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->UpdateRejoinQueueForPlayer(eventAcc, p_connection->PlayerID);

                // If fully loaded, set player to be in game
                if(FullyLoaded)
                {
                    p_connection->ConnectionState = ConnectionState::IN_GAME;
                    cout << "A player finished loading the map..." << endl;

                    // Update all network objects, so that all objects attempt to send their stuff right away
                    static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance())->GetNetworkPriorityHandlerForplayer(p_connection->PlayerID)->UpdateAllNetworkObject();
                }
            }
            else
            {
                // Not sure if we need to check ... maybe
            }
        }

        void ServerNetworkManager::RecieveInputMessage(NetMessage& p_message, Connection* p_connection)
        {
            InputHandlerServer* inputHandler = (InputHandlerServer*)PlayerHandler::GetInstance()->GetInputHandlerForPlayer(p_connection->PlayerID);
            FrequencyBufferHandler* frequencyHandler = PlayerHandler::GetInstance()->GetFrequencyBufferHandlerForPlayer(p_connection->PlayerID);

            // Create a stream
            NetworkStreamer Streamer = NetworkStreamer();

            // Set message buffer to stream
            unsigned char* BufferPointer = p_message.Data;
            Streamer.SetTargetBuffer(BufferPointer, sizeof(p_message.Data));

            uint32_t bytesRead = 0;

            // Read sequence
            uint8_t newSequence = Streamer.ReadUnsignedInt8();
            bytesRead += sizeof(uint8_t);

            // Read input from Stream
            uint32_t InputMask = Streamer.ReadUnsignedInt32();
            bytesRead += sizeof(uint32_t);

            // Set orientation
            EntityID entityID = 0;

            // If we have a player with the ID
            if(!PlayerHandler::GetInstance()->GetEntityIDForPlayer(p_connection->PlayerID, entityID))
            {
                cout << "wrong in recieveinput message" << endl;
            }

            // Set orientation to be updated
            DirectX::XMFLOAT4 playerOrientation = Streamer.ReadRotationQuaternion();
            bytesRead += sizeof(float) * 4;

            // Queue input
            inputHandler->QueueInput(InputMask, playerOrientation, newSequence);

            // Read client position, TODOCM debug
            DirectX::XMFLOAT3 pos = Streamer.ReadFloat3();
            bytesRead += sizeof(float) * 3;

            // Save event sequence
            uint8_t clientEventSequence = Streamer.ReadUnsignedInt8();
            bytesRead += sizeof(uint8_t);

            // Update event queue with acc sequence
            (static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance()))
                ->GetNetworkEventSenderForPlayer(p_connection->PlayerID)
                ->UpdateBufferWithRecievedClientSequenceAcc(clientEventSequence);

            // Read frequency
            frequencyHandler->ReadNewFrequencies(Streamer, sizeof(p_message.Data), bytesRead);
        }


        bool ServerNetworkManager::AdressExist(const DoremiEngine::Network::Adress& m_Adress, Connection*& m_connection)
        {
            // Create iterator for outside use
            std::map<DoremiEngine::Network::Adress*, Connection*>::iterator iter;

            // Check if we got adress already stored
            for(iter = m_connections.begin(); iter != m_connections.end(); ++iter)
            {
                // Custom check cause of adress interface ( check AdressImplementation )
                if(*(iter->first) == m_Adress)
                {
                    m_connection = iter->second;
                    return true;
                }
            }

            return false;
        }

        void ServerNetworkManager::RecieveConnectionRequest(const NetMessage& m_message, const DoremiEngine::Network::Adress& m_adress)
        {
            DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();
            Connection* connection = nullptr;

            // See if we have adress saved
            bool FoundAdress = AdressExist(m_adress, connection);


            // If connection is old, we check if Connected, simply something is wrong and we disconnect connected players
            if(FoundAdress)
            {
                // TODOCM if added more states add here as well to disconnect if wrong is sent
                if(connection->ConnectionState >= ConnectionState::CONNECTED)
                {
                    // Set client state to disconnected, let him timeout to remove or reconnect
                    connection->ConnectionState = ConnectionState::DISCONNECTED;

                    // Send disconnect message
                    SendDisconnect(m_adress, "Bad pattern");
                }
            }
            else // If connection is new, we create a new connection
            {
                // Create a new connection
                Connection* newConnection = new Connection();
                newConnection->LastResponse = 0;
                newConnection->ConnectionState = ConnectionState::CONNECTING;
                newConnection->NewConnection = false;

                // Create a copy of the adress and save it with connection to our map
                DoremiEngine::Network::Adress* NewAdress = NetworkModule.CreateAdress(m_adress);
                m_connections[NewAdress] = newConnection;

                // Send version check message
                SendVersionCheck(m_adress);
            }
        }

        void ServerNetworkManager::RecieveVersionCheck(NetMessage& m_message, const DoremiEngine::Network::Adress& m_adress)
        {
            DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();
            Connection* connection = nullptr;

            // See if we have adress saved
            bool FoundAdress = AdressExist(m_adress, connection);

            if(FoundAdress)
            {
                if(connection->ConnectionState == ConnectionState::CONNECTING)
                {
                    // Change connection state to connecting
                    connection->ConnectionState = ConnectionState::VERSION_CHECK;
                    connection->LastResponse = 0;

                    NetworkStreamer Streamer = NetworkStreamer();
                    unsigned char* DataPOinter = m_message.Data;
                    Streamer.SetTargetBuffer(DataPOinter, sizeof(m_message.Data));
                    uint32_t PlayerID = Streamer.ReadUnsignedInt32();

                    // Check if player is saved
                    // TODOCM change the way its saved
                    // std::list<uint32_t>::iterator iter = std::find(m_SavedPlayerIDs.begin(), m_SavedPlayerIDs.end(), PlayerID);

                    PlayerHandlerServer* t_playerHandler = static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance());
                    // If we don't have saved data of the player, we need to generate a newID
                    if(!t_playerHandler->InactivePlayerIDExists(PlayerID))
                    {
                        // TODO change better way of generation random unique number, then checking against existing
                        PlayerID = rand();

                        while(t_playerHandler->ActivePlayerIDExists(PlayerID))
                        {
                            PlayerID = rand();
                        }
                    }

                    // Set playerID
                    connection->PlayerID = PlayerID;

                    // Send Connected Message
                    SendConnect(connection, m_adress);
                }
                // If packet is dropped, they might still need the connect message
                else if(connection->ConnectionState == ConnectionState::VERSION_CHECK)
                {
                    // Send Connected Message
                    SendConnect(connection, m_adress);
                }
            }
            else
            {
                // Don't have adress, send disconnect?
                // TODOCM maybe dont send anything, because of DDOS
                SendDisconnect(m_adress, "Bad adress/pattern in recv version check");
            }
        }

        void ServerNetworkManager::RecieveDisconnect(const NetMessage& m_message, const DoremiEngine::Network::Adress& m_adress)
        {
            // Remove the adress from list if we have it
            std::map<DoremiEngine::Network::Adress*, Connection*>::iterator iter = m_connections.begin();
            while(iter != m_connections.end())
            {
                // Custom check cause of adress interface ( check AdressImplementation )
                if(*(iter->first) == m_adress)
                {
                    // Delete the memory here
                    delete iter->first;
                    delete iter->second;

                    // Remove this item in map, return will be on next object
                    iter = m_connections.erase(iter);

                    // Step out
                    break;
                }

                // Move to next object
                ++iter;
            }
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

        void ServerNetworkManager::SendDisconnect(const DoremiEngine::Network::Adress& m_adress, std::string p_outString = "")
        {
            if(p_outString != "")
            {
                std::cout << "Sending disconnect: " << p_outString << std::endl;
            }
            else
            {
                std::cout << "Sending disconnect." << std::endl;
            }

            // TODOCM logg instead

            // Create disconnection message
            NetMessage NewMessage = NetMessage();
            NewMessage.MessageID = MessageID::DISCONNECT;

            // TODOCM add info

            // Send message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&NewMessage, sizeof(NewMessage), m_unreliableSocketHandle, &m_adress);
        }

        void ServerNetworkManager::SendVersionCheck(const DoremiEngine::Network::Adress& m_adress)
        {
            std::cout << "Sending version check." << std::endl;
            ; // TODOCM logg instead

            // Create version check message
            NetMessage NewMessage = NetMessage();
            NewMessage.MessageID = MessageID::VERSION_CHECK;

            // TODOCM add info

            // Send version check message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&NewMessage, sizeof(NewMessage), m_unreliableSocketHandle, &m_adress);
        }

        void ServerNetworkManager::SendConnect(const Connection* connection, const DoremiEngine::Network::Adress& m_adress)
        {
            std::cout << "Sending connect." << std::endl;
            ; // TODOCM logg instead

            // Create version check message
            NetMessage NewMessage = NetMessage();
            NewMessage.MessageID = MessageID::CONNECT;

            // TODOCM add info - like port etc...
            NetworkStreamer Streamer = NetworkStreamer();
            unsigned char* t_bufferPointer = NewMessage.Data;
            Streamer.SetTargetBuffer(t_bufferPointer, sizeof(NewMessage.Data));

            Streamer.WriteUnsignedInt32(connection->PlayerID);

            // Send connect message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&NewMessage, sizeof(NewMessage), m_unreliableSocketHandle, &m_adress);
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