// Project specific
#include <Manager/Network/ServerNetworkManager.hpp>
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <Manager/Network/NetMessage.hpp>
#include <Manager/Network/Connection.hpp>
#include <Manager/Network/BitStreamer.h>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <PlayerHandler.hpp>
#include <InputHandlerServer.hpp>
#include <AddRemoveSyncHandler.hpp>

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

        void ServerNetworkManager::RecieveInputMessage(NetMessage& p_message, Connection* p_connection)
        {
            InputHandlerServer* inputHandler = (InputHandlerServer*)PlayerHandler::GetInstance()->GetInputHandlerForPlayer(p_connection->PlayerID);

            // Create a stream
            BitStreamer Streamer = BitStreamer();

            // Set message buffer to stream
            unsigned char* BufferPointer = p_message.Data;
            Streamer.SetTargetBuffer(BufferPointer, sizeof(p_message));

            // Read sequence
            p_connection->LastSequence = Streamer.ReadUnsignedInt8();

            // Read input from Stream
            uint32_t InputMask = Streamer.ReadUnsignedInt32();

            // Set Input to handler
            inputHandler->SetInputBitMask(InputMask);

            // Set orientation
            EntityID entityID = 0;

            // If we have a player with the ID
            if(PlayerHandler::GetInstance()->GetEntityIDForPlayer(p_connection->PlayerID, entityID))
            {
                // Set orientation to be updated
                inputHandler->SetOrientationFromInput(Streamer.ReadRotationQuaternion());

                // Save position to send it on next message
                p_connection->PositionOnLastSequence = GetComponent<TransformComponent>(entityID)->position;

                uint8_t clientSequence = Streamer.ReadUnsignedInt8();

                PlayerHandler::GetInstance()->GetAddRemoveSyncHandlerForPlayer(p_connection->PlayerID)->UpdateQueueWithSequence(clientSequence);
            }
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
                newConnection->LastSequence = m_nextSnapshotSequence;
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
                    // Change connection state to connected
                    connection->ConnectionState = ConnectionState::VERSION_CHECK;
                    connection->LastResponse = 0;

                    BitStreamer Streamer = BitStreamer();
                    unsigned char* DataPOinter = m_message.Data;
                    Streamer.SetTargetBuffer(DataPOinter, sizeof(m_message.Data));
                    uint32_t PlayerID = Streamer.ReadUnsignedInt32();


                    // Check if player is saved
                    // TODOCM change the way its saved
                    std::list<uint32_t>::iterator iter = std::find(m_SavedPlayerIDs.begin(), m_SavedPlayerIDs.end(), PlayerID);

                    // If we found one we remoe it from list and use it
                    if(iter != m_SavedPlayerIDs.end())
                    {
                        m_SavedPlayerIDs.erase(iter);

                        // TODOCM maybe crete player again
                        PlayerID = rand();

                        InputHandlerServer* NewInputHandler = new InputHandlerServer(m_sharedContext);

                        // Create player
                        PlayerHandler::GetInstance()->CreateNewPlayer(PlayerID, NewInputHandler);
                    }
                    else
                    {

                        // Create a playerID
                        // TODOCM need to change this to some other method to get a unique ID, like gametime
                        PlayerID = rand();

                        InputHandlerServer* NewInputHandler = new InputHandlerServer(m_sharedContext);

                        // Create player
                        PlayerHandler::GetInstance()->CreateNewPlayer(PlayerID, NewInputHandler);
                    }

                    connection->PlayerID = PlayerID;


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
            BitStreamer Streamer = BitStreamer();
            Streamer.SetTargetBuffer(p_buffer, p_bufferSize);

            uint32_t BytesWritten = 0;

            PlayerHandler::GetInstance()->GetAddRemoveSyncHandlerForPlayer(p_connection->PlayerID)->WriteAddRemoves(Streamer, p_bufferSize, BytesWritten);

            // Write snapshot ID (1 byte
            Streamer.WriteUnsignedInt8(m_nextSnapshotSequence);


            // Write client last sequence (1 byte
            Streamer.WriteUnsignedInt8(p_connection->LastSequence);

            // Write position of that sequence ( 12 byte
            Streamer.WriteFloat3(p_connection->PositionOnLastSequence);

            BytesWritten += 14;

            uint32_t positionToWriteNumOfObjects = BytesWritten;
            // Move forward for header (14 byte + numOfObjects to be written later
            Streamer.SetReadWritePosition(BytesWritten + sizeof(uint8_t));
            BytesWritten += sizeof(uint8_t);

            EntityHandler& EntityHandler = EntityHandler::GetInstance();

            // Mask is if we have transform and network
            int MaskToCheck = (int)ComponentType::Transform | (int)ComponentType::NetworkObject;

            // Counter for how many entities we send
            uint32_t NumberOfEntitiesToSend = 0;

            // Loop over entities
            // TODOCM put them in some kind of priority so we can send them cross many snapshots
            uint32_t NumOfEntities = EntityHandler.GetLastEntityIndex();
            for(size_t EntityID = 0; EntityID < NumOfEntities && p_bufferSize > (BytesWritten + sizeof(uint32_t) + sizeof(float) * 3 + sizeof(float) * 4); EntityID++)
            {
                if(EntityHandler.HasComponents(EntityID, MaskToCheck))
                {
                    TransformComponent* TransComponent = EntityHandler.GetComponentFromStorage<TransformComponent>(EntityID);
                    Streamer.WriteUnsignedInt32(EntityID); // 4 byte
                    Streamer.WriteFloat3(TransComponent->position); // 4x3 = 12 byte
                    Streamer.WriteRotationQuaternion(TransComponent->rotation); // 4x4 = 16 byte

                    // all = 32 byte
                    NumberOfEntitiesToSend++;

                    // Increase offset
                    BytesWritten += sizeof(uint32_t) + sizeof(float) * 3 + sizeof(float) * 4;
                }
            }

            if(NumberOfEntitiesToSend > NumOfEntities)
            {
                int a = 3;
            }

            // Write how many add/remove exist for player
            Streamer.SetReadWritePosition(positionToWriteNumOfObjects);
            Streamer.WriteUnsignedInt8(NumberOfEntitiesToSend);
        }

        void ServerNetworkManager::SendMessages(double p_dt)
        {
            // Remove time
            m_nextUpdateTimer -= m_updateInterval;

            // Update sequence here because of the error checking..
            m_nextSnapshotSequence++;

            // cout << (int)m_nextSnapshotSequence << endl;


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
            BitStreamer Streamer = BitStreamer();
            Streamer.WriteUnsignedInt32(connection->PlayerID);

            // Send connect message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&NewMessage, sizeof(NewMessage), m_unreliableSocketHandle, &m_adress);
        }

        void ServerNetworkManager::SendConnected(Connection* p_connection)
        {
            DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();

            NetMessage Message = NetMessage();

            Message.MessageID = MessageID::CONNECTED;

            BitStreamer Streamer = BitStreamer();
            unsigned char* BufferPointer = Message.Data;
            Streamer.SetTargetBuffer(BufferPointer, sizeof(Message.Data));

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
            BitStreamer Streamer = BitStreamer();
            unsigned char* bufferPointer = Message.Data;
            Streamer.SetTargetBuffer(bufferPointer, sizeof(Message.Data));

            // Send created players
            uint32_t numPlayers = PlayerHandler::GetInstance()->GetNumOfPlayers();
            Streamer.WriteUnsignedInt32(numPlayers - 1);

            p_connection->ConnectionState = ConnectionState::IN_GAME;

            // Send message
            NetworkModule.SendReliableData(&Message, sizeof(Message), p_connection->ReliableSocketHandle);
        }

        void ServerNetworkManager::SendInGame(NetMessage& p_message, Connection* p_connection)
        {
            DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();

            // std::cout << "Sending snapshot." << std::endl; // TODOCM logg instead
            // If we're a new connection we send a initialise snapshot, might need this later
            if(p_connection->NewConnection)
            {
                p_message.MessageID = MessageID::INIT_SNAPSHOT;
                p_connection->NewConnection = false;
            }
            else
            {
                counter++; // TODOCM remove test
                int checkValue;

                if(counter < 2)
                {
                    checkValue = 100;
                }
                else
                {
                    counter = -1;
                    checkValue = -1;
                }

                // TODOCM remove, packet loss experiment
                int a = rand() % 100;
                if(a < checkValue)
                {
                    /*std::cout << "Dropping packages..." << std::endl;
                    return;*/
                }
            }

            // Send message
            if(!NetworkModule.SendReliableData(&p_message, sizeof(p_message), p_connection->ReliableSocketHandle))
            {
                cout << "Failed to send" << endl;
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
                            break;
                        }
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