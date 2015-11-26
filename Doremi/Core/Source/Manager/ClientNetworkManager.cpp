// Project specific
#include <Manager/ClientNetworkManager.hpp>
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <DoremiEngine/Network/Include/NetMessage.hpp>

namespace Doremi
{
    namespace Core
    {
        ClientNetworkManager::ClientNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
        : Manager(p_sharedContext), m_masterConnectionState(ConnectionState::DISCONNECTED),
          m_serverConnectionState(ConnectionState::DISCONNECTED), m_nextUpdateTimer(0.0f),
          m_updateInterval(1000.0f)
        {
            // TODOCM Remove test code
            p_sharedContext.GetNetworkModule().ConnectToReliable(127, 0, 0, 1, 3500);
        }

        ClientNetworkManager::~ClientNetworkManager()
        {
        }

        void ClientNetworkManager::Update(double p_dt)
        {
            m_nextUpdateTimer += p_dt;

            if(m_nextUpdateTimer >= m_updateInterval)
            {
                m_nextUpdateTimer -= m_updateInterval;
            }
        }
    }
}