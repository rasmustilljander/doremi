#include <Doremi/Core/Include/Network/NetworkConnectionsServer.hpp>

// Engine
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Configuration/Include/ConfigurationModule.hpp>

// Network
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <DoremiEngine/Network/Include/Adress.hpp>

namespace Doremi
{
    namespace Core
    {
        NetworkConnectionsServer* NetworkConnectionsServer::m_singleton = nullptr;

        NetworkConnectionsServer* NetworkConnectionsServer::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("GetInstance was called before StartupConnectionsServer.");
            }
            return m_singleton;
        }

        void NetworkConnectionsServer::StartupNetworkConnectionsServer(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("StartupConnectionsServer was called multiple times.");
            }
            m_singleton = new NetworkConnectionsServer(p_sharedContext);
        }

        NetworkConnectionsServer::NetworkConnectionsServer(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext), m_maxConnections(16)
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = p_sharedContext.GetNetworkModule();
            const DoremiEngine::Configuration::ConfiguartionInfo& t_config = p_sharedContext.GetConfigurationModule().GetAllConfigurationValues();

            m_portConnecting = t_config.PortServerConnecting;
            m_portConnected = t_config.PortServerConnected;

            // Create adress for ALL incomming
            DoremiEngine::Network::Adress* UnreliableAdress = t_networkModule.CreateAdress(m_portConnecting);

            // Create adress for ALL incomming
            DoremiEngine::Network::Adress* ReliableAdress = t_networkModule.CreateAdress(m_portConnected);

            // Create socket for unrealiable
            m_connectingSocketHandle = t_networkModule.CreateUnreliableWaitingSocket(UnreliableAdress);

            // Create socket for relialbe
            m_connectedSocketHandle = t_networkModule.CreateReliableConnection(ReliableAdress, m_maxConnections);

            // Load master ip and port
            LoadMasterFromConfigFile(p_sharedContext);
        }

        void NetworkConnectionsServer::LoadMasterFromConfigFile(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = p_sharedContext.GetNetworkModule();
            DoremiEngine::Configuration::ConfiguartionInfo t_configInfo = p_sharedContext.GetConfigurationModule().GetAllConfigurationValues();

            // Get IP from config file
            std::string t_IPString = t_configInfo.IPToMaster;

            // Create pointer to use
            char* t_IPCharPointer = new char[t_IPString.size() + 1];

            // Copy string to pointer
            t_IPString.copy(t_IPCharPointer, t_IPString.size());

            // Create int array to hold IP values
            int* t_IPArray = new int[4];

            // Pointer to part of string to check
            char* t_ToCheck;
            t_ToCheck = strtok(t_IPCharPointer, ".");

            size_t t_counter = 0;
            while(t_ToCheck != NULL && t_counter < 4)
            {
                // Save string part as int to array
                t_IPArray[t_counter] = std::stoi(std::string(t_ToCheck));
                t_ToCheck = strtok(NULL, ".");
                t_counter++;
            }

            // If we got values for all int slots we got an actual IP
            if(t_counter == 4)
            {
                // TODOCM Read port ? as well, change to master as well probobly
                m_masterConnection.Adress = t_networkModule.CreateAdress(t_IPArray[0], t_IPArray[1], t_IPArray[2], t_IPArray[3],
                                                                         t_configInfo.PortMasterServer); // TODOCM remove test code
            }
            else
            {
                m_masterConnection.Adress = t_networkModule.CreateAdress(127, 0, 0, 1, t_configInfo.PortMasterServer);
            }

            // Create new socket for unreliable
            m_masterConnection.SocketHandle = t_networkModule.CreateUnreliableSocket();

            delete t_IPCharPointer;
            delete t_IPArray;
        }

        NetworkConnectionsServer::~NetworkConnectionsServer() {}

        void NetworkConnectionsServer::CreateNewConnecting(const DoremiEngine::Network::Adress& p_adress)
        {
            // Create new connection and initialize values( could do this in constructor but nice to see)
            ClientConnectionFromServer* t_newConnection = new ClientConnectionFromServer();

            // First thing we do is version checking
            t_newConnection->ConnectionState = ClientConnectionStateFromServer::VERSION_CHECK;
            t_newConnection->LastResponse = 0;
            t_newConnection->LastSequenceUpdate = SEQUENCE_TIMER_START; // High because we want update :)

            // Copy adress
            DoremiEngine::Network::Adress* t_newAdress = m_sharedContext.GetNetworkModule().CreateAdress(p_adress);

            // Add connection to map
            m_connectingClientConnections[t_newAdress] = t_newConnection;
        }

        void NetworkConnectionsServer::SetConnecting(const std::pair<DoremiEngine::Network::Adress*, ClientConnectionFromServer*>& p_connection)
        {
            // Remove from connecting and add to connected
            m_connectedClientConnections[p_connection.first] = p_connection.second;
            m_connectingClientConnections.erase(p_connection.first);
        }


        void NetworkConnectionsServer::RemoveConnection(DoremiEngine::Network::Adress& p_adress)
        {
            // Try to remove from both maps
            m_connectedClientConnections.erase(&p_adress);
            m_connectingClientConnections.erase(&p_adress);
        }

        bool NetworkConnectionsServer::AdressWithPortExist(const DoremiEngine::Network::Adress& p_adress, ClientConnectionFromServer*& o_connection)
        {
            // Check all connectd connections
            for(auto& t_connection : m_connectedClientConnections)
            {
                // Custom check cause of adress interface ( check AdressImplementation )
                // Check if we have same IP and Port
                if(*(t_connection.first) == p_adress)
                {
                    o_connection = t_connection.second;
                    return true;
                }
            }

            // TODOCM evaluate if we need to check for connected connections
            // Check all connecting connections
            for(auto& t_connection : m_connectingClientConnections)
            {
                // Custom check cause of adress interface ( check AdressImplementation )
                // Check if we have same IP and Port
                if(*(t_connection.first) == p_adress)
                {
                    o_connection = t_connection.second;
                    return true;
                }
            }

            return false;
        }

        bool NetworkConnectionsServer::AdressExistInConnecting(const DoremiEngine::Network::Adress& p_adress,
                                                               std::pair<DoremiEngine::Network::Adress*, ClientConnectionFromServer*>& o_connection)
        {
            // TODOCM evaluate if we need to check for connected connections
            // Check all connecting connections
            for(auto& t_connection : m_connectingClientConnections)
            {
                // Custom check cause of adress interface ( check AdressImplementation )
                // Check if we have same IP
                if(*(t_connection.first) *= p_adress)
                {
                    o_connection = t_connection;
                    return true;
                }
            }

            return false;
        }
    }
}