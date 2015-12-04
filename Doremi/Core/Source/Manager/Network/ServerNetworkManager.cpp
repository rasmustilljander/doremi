// Project specific
#include <Manager/Network/ServerNetworkManager.hpp>
#include <DoremiEngine/Network/Include/NetworkModule.hpp>
#include <Manager/Network/NetMessage.hpp>
#include <iostream> // TODOCM remove after test

namespace Doremi
{
    namespace Core
    {
        ServerNetworkManager::ServerNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
        : Manager(p_sharedContext)
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