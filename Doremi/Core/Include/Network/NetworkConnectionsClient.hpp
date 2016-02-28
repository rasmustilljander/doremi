#pragma once
#include <Doremi/Core/Include/Network/Connections.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
    namespace Network
    {
        class Adress;
    }
}

namespace Doremi
{
    namespace Core
    {
        class NetworkConnectionsClient
        {
        public:
            static NetworkConnectionsClient* GetInstance();

            static void StartupNetworkConnectionsClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ServerConnectionFromClient m_serverConnection;

            MasterConnectionFromClient m_masterConnection;

        private:
            NetworkConnectionsClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void LoadMasterFromConfigFile(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void LoadServerFromConfigFile(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~NetworkConnectionsClient();

            static NetworkConnectionsClient* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}