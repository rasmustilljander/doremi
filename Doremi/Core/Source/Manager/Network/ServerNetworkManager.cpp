// Project specific

// Managers
#include <Manager/Network/ServerNetworkManager.hpp>

// Modules
#include <DoremiEngine/Network/Include/NetworkModule.hpp>


#include <Manager/Network/NetMessage.hpp>
#include <Manager/Network/Connection.hpp>
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>

// Compnoents
#include <EntityComponent/Components/TransformComponent.hpp>

// Handlers
#include <PlayerHandlerServer.hpp>
#include <InputHandlerServer.hpp>
#include <FrequencyBufferHandler.hpp>
#include <Doremi/Core/Include/NetworkPriorityHandler.hpp>
#include <EntityComponent/EntityHandler.hpp>

#include <NetworkEventSender.hpp>

#include <SequenceMath.hpp>

#include <iostream> // TODOCM remove after test
#include <vector>
#include <algorithm>

namespace Doremi
{
    namespace Core
    {
        ServerNetworkManager::ServerNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "ServerNetworkManager"),
              m_nextUpdateTimer(0.0f),
              m_updateInterval(0.017f),
              m_timeoutInterval(3.0f),
              m_maxConnection(16),
              m_nextSnapshotSequence(0)
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
        }

        ServerNetworkManager::~ServerNetworkManager() {}

        void ServerNetworkManager::Update(double p_dt)
        {
            // Recieve Messages
            RecieveMessages(p_dt);

            // Send Messages
            SendMessages(p_dt);

            // Accept connections
            CheckForConnections();

            // Check for timed out connections
            UpdateTimeouts(p_dt);
        }

        void ServerNetworkManager::RecieveMessages(double p_dt)
        {
            // For some incomming unreliable recieved messages we send one
            RecieveUnreliableMessages();

            // Recieve reliable messages from connected clients
            RecieveReliableMessages();
        }
        // TODOCM maybe check what their timer is befor sending a new message cause of DDOS etc
        void ServerNetworkManager::RecieveUnreliableMessages()
        {
            DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();
            NetMessage Message = NetMessage();
            DoremiEngine::Network::Adress* IncommingAdress = NetworkModule.CreateAdress(); // TODOCM dont forget to remove this one

            // Check for incomming messages
            while(NetworkModule.RecieveUnreliableData(&Message, sizeof(Message), m_unreliableSocketHandle, IncommingAdress))
            {
                std::cout << "Recieved unreliable messsage: "; // TODOCM logg instead

                switch(Message.MessageID)
                {
                    case MessageID::CONNECT_REQUEST:

                        std::cout << "Connection Request." << std::endl; // TODOCM logg instead

                        // Recieve a connnection request message and interpet
                        RecieveConnectionRequest(Message, *IncommingAdress);

                        break;
                    case MessageID::VERSION_CHECK:

                        std::cout << "Version Check" << std::endl; // TODOCM logg instead

                        // Recieve a version check message and interpet
                        RecieveVersionCheck(Message, *IncommingAdress);

                        break;
                    case MessageID::DISCONNECT:

                        std::cout << "Disconnect" << std::endl; // TODOCM logg instead

                        // Recieve a disconnect message and interpet
                        RecieveDisconnect(Message, *IncommingAdress);

                        break;
                    default:
                        break;
                }

                Message = NetMessage();
            }

            delete IncommingAdress;
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

        void ServerNetworkManager::RecieveReliableMessages()
        {
            DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();

            // TODOCM make some break on the inner while, because if we're overflowed we will never get out
            // For each connection
            for(std::map<DoremiEngine::Network::Adress*, Connection*>::iterator iter = m_connections.begin(); iter != m_connections.end(); ++iter)
            {
                if(iter->second->ConnectionState >= ConnectionState::CONNECTED)
                {
                    NetMessage Message = NetMessage();

                    // Check if we got any data
                    // TODOCM maybe we want to loop to get all data? or not..
                    while(NetworkModule.RecieveReliableData(&Message, sizeof(Message), iter->second->ReliableSocketHandle))
                    {
                        // std::cout << "Recieved reliable messsage." << std::endl;
                        // TODOCM logg instead
                        switch(Message.MessageID)
                        {
                            case MessageID::CONNECTED:

                                // Add code here
                                break;

                            case MessageID::LOAD_WORLD:

                                // Add code here
                                ReceiveLoadWorldMessage(Message, iter->second);
                                break;

                            case MessageID::INPUT:

                                // Interpet n' input message
                                RecieveInputMessage(Message, iter->second);
                                break;

                            default:
                                break;
                        }

                        // TODOCM interpet data
                        iter->second->LastResponse = 0;

                        Message = NetMessage();
                    }
                }
            }
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
                if(connection->ConnectionState == ConnectionState::CONNECTED)
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
                    std::list<uint32_t>::iterator iter = std::find(m_SavedPlayerIDs.begin(), m_SavedPlayerIDs.end(), PlayerID);

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

                            // Random new playerID
                            uint32_t PlayerID = rand();

                            // Create new InputHandler
                            InputHandlerServer* NewInputHandler = new InputHandlerServer(m_sharedContext, DirectX::XMFLOAT3(0, 0, 0));

                            // I want to create a new Player" in the PlayerHandler, add start and stop, then create a new entity?
                            // Create player
                            PlayerHandler::GetInstance()->CreateNewPlayer(PlayerID, NewInputHandler);

                            // Set playerID
                            iter->second->PlayerID = PlayerID;


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

        void ServerNetworkManager::UpdateTimeouts(double t_dt)
        {
            std::map<DoremiEngine::Network::Adress*, Connection*>::iterator iter = m_connections.begin();

            while(iter != m_connections.end())
            {
                // Update timer
                iter->second->LastResponse += t_dt;

                // Check if exceeded timeout
                if(iter->second->LastResponse >= m_timeoutInterval)
                {
                    // Send disconnection message
                    SendDisconnect(*iter->first, "Timeout");

                    m_sharedContext.GetNetworkModule().DeleteSocket(iter->second->ReliableSocketHandle);

                    // Delete the memory here
                    delete iter->first;
                    delete iter->second;

                    // Remove this item in map, return will be on next object
                    iter = m_connections.erase(iter);
                }
                else
                {
                    // Move to next object
                    ++iter;
                }
            }
        }
    }
}