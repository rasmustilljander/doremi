#include <Doremi/Core/Include/Network/NetworkConnectionsMaster.hpp>

// Engine
#include <DoremiEngine/Core/Include/SharedContext.hpp>

// Network
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <DoremiEngine/Network/Include/Adress.hpp>

namespace Doremi
{
    namespace Core
    {
        NetworkConnectionsMaster* NetworkConnectionsMaster::m_singleton = nullptr;

        NetworkConnectionsMaster* NetworkConnectionsMaster::GetInstance()
        {
            if (m_singleton == nullptr)
            {
                std::runtime_error("GetInstance was called before StartupConnectionsServer.");
            }
            return m_singleton;
        }

        void NetworkConnectionsMaster::StartupNetworkConnectionsMaster(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if (m_singleton != nullptr)
            {
                std::runtime_error("StartupConnectionsServer was called multiple times.");
            }
            m_singleton = new NetworkConnectionsMaster(p_sharedContext);
        }

        NetworkConnectionsMaster::NetworkConnectionsMaster(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext)
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = p_sharedContext.GetNetworkModule();

            // Create adress for ALL incomming IP and port 3200 clients
            DoremiEngine::Network::Adress* AdressClients = t_networkModule.CreateAdress(3200);

            // Create adress for ALL incomming IP and port 3201 servers
            DoremiEngine::Network::Adress* AdressServers = t_networkModule.CreateAdress(3201);

            // Create socket for clients
            m_socketHandleClients = t_networkModule.CreateUnreliableWaitingSocket(AdressClients);

            // Create sockets for servers
            m_socketHandleServers = t_networkModule.CreateUnreliableWaitingSocket(AdressServers);
        }

        NetworkConnectionsMaster::~NetworkConnectionsMaster() {}


        bool NetworkConnectionsMaster::AdressWithPortExistClients(const DoremiEngine::Network::Adress& p_adress, ClientConnectionFromMaster*& o_connection)
        {
            // Check all connectd connections
            for (auto& t_connection : m_clientConnections)
            {
                // Custom check cause of adress interface ( check AdressImplementation )
                // Check if we have same IP and Port
                if (*(t_connection.first) == p_adress)
                {
                    o_connection = t_connection.second;
                    return true;
                }
            }

            return false;
        }

        bool NetworkConnectionsMaster::AdressWithPortExistServers(const DoremiEngine::Network::Adress& p_adress, ServerConnectionFromMaster*& o_connection)
        {
            // Check all connectd connections
            for (auto& t_connection : m_serverConnections)
            {
                // Custom check cause of adress interface ( check AdressImplementation )
                // Check if we have same IP and Port
                if (*(t_connection.first) == p_adress)
                {
                    o_connection = t_connection.second;
                    return true;
                }
            }

            return false;
        }

        void NetworkConnectionsMaster::CreateNewConnectionClients(const DoremiEngine::Network::Adress& p_adress)
        {
            // Create new connection and initialize values( could do this in constructor but nice to see)
            ClientConnectionFromMaster* t_newConnection = new ClientConnectionFromMaster();

            // First thing we do is version checking
            t_newConnection->ConnectionState = ClientConnectionStateFromMaster::CONNECTED;
            t_newConnection->LastResponse = 0;

            // Copy adress
            DoremiEngine::Network::Adress* t_newAdress = m_sharedContext.GetNetworkModule().CreateAdress(p_adress);

            // Add connection to map
            m_clientConnections[t_newAdress] = t_newConnection;
        }

        void NetworkConnectionsMaster::CreateNewConnectionServers(const DoremiEngine::Network::Adress& p_adress)
        {
            // Create new connection and initialize values( could do this in constructor but nice to see)
            ServerConnectionFromMaster* t_newConnection = new ServerConnectionFromMaster();

            // First thing we do is version checking
            t_newConnection->ConnectionState = ServerConnectionStateFromMaster::CONNECTED;
            t_newConnection->LastResponse = 0;

            // Copy adress
            DoremiEngine::Network::Adress* t_newAdress = m_sharedContext.GetNetworkModule().CreateAdress(p_adress);

            // Add connection to map
            m_serverConnections[t_newAdress] = t_newConnection;
        }
    }
}