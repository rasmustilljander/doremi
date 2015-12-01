#pragma once

// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>


namespace Doremi
{
    namespace Core
    {
        class NetworkModule;
        /**
            Manager class for Server, contains the logic for the connection pattern
        */
        class ServerNetworkManager : public Manager
        {
            public:
            /**
            TODOCM doc
            */
            ServerNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
            TODOCM doc
            */
            virtual ~ServerNetworkManager();

            /**
            TODOCM doc
            */
            void Update(double p_dt) override;
        };
    }
}
