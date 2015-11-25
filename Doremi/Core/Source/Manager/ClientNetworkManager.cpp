#include <Manager/ClientNetworkManager.hpp>
#include <DoremiEngine/Network/Include/NetMessage.hpp>
#include <DoremiEngine/Network/Include/Connection.hpp>

namespace Doremi
{
	namespace Core
	{
		ClientNetworkManager::ClientNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
			:Manager(p_sharedContext), m_masterConnectionState(ConnectionState::DISCONNECTED), m_serverConnectionState(ConnectionState::DISCONNECTED),
			m_nextUpdateTimer(0.0f), m_updateInterval(1000.0f)
		{
            //TODOCM remove test code
            p_sharedContext.GetNetworkModule().Startup();
		}

		ClientNetworkManager::~ClientNetworkManager()
		{

		}

		void ClientNetworkManager::Update(double p_dt)
		{
            m_nextUpdateTimer += p_dt;

			if (m_nextUpdateTimer >= m_updateInterval)
			{
				m_nextUpdateTimer -= m_updateInterval;
			}
		}
	}
}