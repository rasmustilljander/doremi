#include <Doremi/Core/Include/Network/NetworkConnectionsClient.hpp>
#include <DoremiEngine\Network\Include\NetworkModule.hpp>

#include <DoremiEngine/Core/Include/SharedContext.hpp>

namespace Doremi
{
    namespace Core
    {
        NetworkConnectionsClient* NetworkConnectionsClient::m_singleton = nullptr;

        NetworkConnectionsClient* NetworkConnectionsClient::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("Called GetInstance before StartupNetworkConnectionsClient.");
            }
            return m_singleton;
        }

        void NetworkConnectionsClient::StartupNetworkConnectionsClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("Called StartupNetworkConnectionsClient multiple times.");
            }
            m_singleton = new NetworkConnectionsClient(p_sharedContext);
        }

        NetworkConnectionsClient::NetworkConnectionsClient(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext)
        {
            DoremiEngine::Network::NetworkModule& t_networkModule = p_sharedContext.GetNetworkModule();

            // Create adress and socket for master
            m_masterConnection.Adress = t_networkModule.CreateAdress(127, 0, 0, 1, 3200);
            m_masterConnection.SocketHandle = t_networkModule.CreateUnreliableSocket();
        }

        NetworkConnectionsClient::~NetworkConnectionsClient() {}
    }
}