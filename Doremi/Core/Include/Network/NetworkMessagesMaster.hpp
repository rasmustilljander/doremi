#pragma once
#include <string>


namespace DoremiEngine
{
    namespace Network
    {
        class Adress;
    }
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {

        class NetworkMessagesMaster
        {
        public:
            static NetworkMessagesMaster* GetInstance();

            static void StartupNetworkMessagesMaster(const DoremiEngine::Core::SharedContext& p_sharedContext);

        private:
            NetworkMessagesMaster(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~NetworkMessagesMaster();

            static NetworkMessagesMaster* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}