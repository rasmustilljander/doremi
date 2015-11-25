#include <Manager/ServerNetworkManager.hpp>
#include <DoremiEngine/Network/Include/NetMessage.hpp>
#include <DoremiEngine/Network/Include/Connection.hpp>

namespace Doremi
{
    namespace Core
    {
        ServerNetworkManager::ServerNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            :Manager(p_sharedContext)
        {

        }

        ServerNetworkManager::~ServerNetworkManager()
        {

        }

        void ServerNetworkManager::Update(double p_dt)
        {

        }
    }
}