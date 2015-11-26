#include <Manager/ServerNetworkManager.hpp>
#include <DoremiEngine/Network/Include/NetMessage.hpp>

namespace Doremi
{
    namespace Core
    {
        ServerNetworkManager::ServerNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
        : Manager(p_sharedContext)
        {
            // TODOCM Remove test code
            size_t sock = p_sharedContext.GetNetworkModule().CreateReliableConnection(127, 0, 0, 1, 3500);

            // TODOCM Remove test code
            p_sharedContext.GetNetworkModule().AcceptConnection(sock);
        }

        ServerNetworkManager::~ServerNetworkManager()
        {
        }

        void ServerNetworkManager::Update(double p_dt)
        {
        }
    }
}