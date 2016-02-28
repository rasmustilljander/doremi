#include <Doremi/Core/Include/Network/NetworkConnectionsServer.hpp>

// Engine
#include <DoremiEngine/Core/Include/SharedContext.hpp>

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

            // Create adress for ALL incomming IP and port 5050
            DoremiEngine::Network::Adress* UnreliableAdress = t_networkModule.CreateAdress(5050);

            // Create adress for ALL incomming IP and port 4050
            DoremiEngine::Network::Adress* ReliableAdress = t_networkModule.CreateAdress(4050);

            // Create socket for unrealiable
            m_connectingSocketHandle = t_networkModule.CreateUnreliableWaitingSocket(UnreliableAdress);

            // Create socket for relialbe
            m_connectedSocketHandle = t_networkModule.CreateReliableConnection(ReliableAdress, m_maxConnections);
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