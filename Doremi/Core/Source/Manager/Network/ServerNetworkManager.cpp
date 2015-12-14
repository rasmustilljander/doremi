// Project specific
#include <Manager/Network/ServerNetworkManager.hpp>
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <Manager/Network/NetMessage.hpp>
#include <Manager/Network/Connection.hpp>
#include <iostream> // TODOCM remove after test
#include <vector>

namespace Doremi
{
    namespace Core
    {
        ServerNetworkManager::ServerNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext), m_nextUpdateTimer(0.0f), m_updateInterval(0.017f), m_timeoutInterval(5.0f), m_maxConnection(16)
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
            }

            delete IncommingAdress;
        }

        void ServerNetworkManager::RecieveReliableMessages()
        {
            DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();

            // For each connection
            for(std::map<DoremiEngine::Network::Adress*, Connection*>::iterator iter = m_connections.begin(); iter != m_connections.end(); ++iter)
            {
                if(iter->second->ConnectionState == ConnectionState::CONNECTED)
                {
                    NetMessage Message = NetMessage();

                    // Check if we got any data
                    // TODOCM maybe we want to loop to get all data? or not..
                    if(NetworkModule.RecieveReliableData(&Message, sizeof(Message), iter->second->ReliableSocketHandle))
                    {
                        std::cout << "Recieved reliable messsage." << std::endl;
                        ; // TODOCM logg instead

                        // TODOCM interpet data
                        iter->second->LastResponse = 0;
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
                    SendDisconnect(m_adress);
                }
            }
            else // If connection is new, we create a new connection
            {
                // Create a new connection
                Connection* newConnection = new Connection();
                newConnection->LastResponse = 0;
                newConnection->ConnectionState = ConnectionState::CONNECTING;

                // Create a copy of the adress and save it with connection to our map
                DoremiEngine::Network::Adress* NewAdress = NetworkModule.CreateAdress(m_adress);
                m_connections[NewAdress] = newConnection;

                // Send version check message
                SendVersionCheck(m_adress);
            }
        }

        void ServerNetworkManager::RecieveVersionCheck(const NetMessage& m_message, const DoremiEngine::Network::Adress& m_adress)
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

                    // Send Connected Message
                    SendConnect(m_adress);
                }
            }
            else
            {
                // Don't have adress, send disconnect?
                // TODOCM maybe dont send anything, because of DDOS
                SendDisconnect(m_adress);
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

        void ServerNetworkManager::SendMessages(double p_dt)
        {
            // Update timer to send
            m_nextUpdateTimer += p_dt;

            // If we exceed timer
            if(m_nextUpdateTimer >= m_updateInterval)
            {
                // Remove time
                m_nextUpdateTimer -= m_updateInterval;

                DoremiEngine::Network::NetworkModule& NetworkModule = m_sharedContext.GetNetworkModule();

                // For all connected clients we send messages
                for(std::map<DoremiEngine::Network::Adress*, Connection*>::iterator iter = m_connections.begin(); iter != m_connections.end(); ++iter)
                {
                    if(iter->second->ConnectionState == ConnectionState::CONNECTED)
                    {
                        std::cout << "Sending snapshot." << std::endl;
                        ; // TODOCM logg instead

                        // Create message
                        NetMessage Message = NetMessage();
                        Message.MessageID = MessageID::SNAPSHOT;

                        // TODOCM add snapshot info here

                        // Send message
                        NetworkModule.SendReliableData(&Message, sizeof(Message), iter->second->ReliableSocketHandle);
                    }
                }
            }
        }

        void ServerNetworkManager::SendDisconnect(const DoremiEngine::Network::Adress& m_adress)
        {
            std::cout << "Sending disconnect." << std::endl;
            ; // TODOCM logg instead

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

        void ServerNetworkManager::SendConnect(const DoremiEngine::Network::Adress& m_adress)
        {
            std::cout << "Sending connect." << std::endl;
            ; // TODOCM logg instead

            // Create version check message
            NetMessage NewMessage = NetMessage();
            NewMessage.MessageID = MessageID::CONNECT;

            // TODOCM add info - like port etc...

            // Send connect message
            m_sharedContext.GetNetworkModule().SendUnreliableData(&NewMessage, sizeof(NewMessage), m_unreliableSocketHandle, &m_adress);
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
                        }
                        else // If not, either wrong stage or something happened, or bot..
                        {
                            // Send disconnect message
                            SendDisconnect(*OutAdress);

                            // Set their state to disconnected
                            iter->second->ConnectionState = ConnectionState::DISCONNECTED;
                        }
                        // Break loop
                        // TODOCM maybe send disconnect even if we dont have him in list?
                        break;
                    }
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
                    SendDisconnect(*iter->first);

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