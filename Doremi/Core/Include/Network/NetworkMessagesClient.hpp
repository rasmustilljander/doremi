#pragma once
#include <Doremi/Core/Include/Network/NetMessages.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        class NetworkMessagesClient
        {
        public:
            static NetworkMessagesClient* GetInstance();

            static void StartupNetworkMessagesClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                == Receive Messages Connecting ==
            */

            /**
                Receive a version check message
            */
            void ReceiveVersionCheck(NetMessageConnectingFromServer& p_message);

            /**
                Receive a connect message
            */
            void ReceiveConnect(NetMessageConnectingFromServer& p_message);

            /**
                Receive a disconnect messages
            */
            void ReceiveDisconnect(NetMessageConnectingFromServer& p_message);

            /**
                == Receive Messages Connected ==
            */

            /**
                Receive connected message
            */
            void ReceiveConnected(NetMessageConnectedFromServer& p_message);

            /**
                Receive load world message
            */
            void ReceiveLoadWorld(NetMessageConnectedFromServer& p_message);

            /**
                Receive in game message
            */
            void ReceiveInGame(NetMessageConnectedFromServer& p_message);

            /**
                == Send Messages Connecting ==
            */

            /**
                Sends a connection request message
            */
            void SendConnectionRequest();

            /**
                Sends a version check message
            */
            void SendVersionCheck();

            /**
                Sends a disconnect message
            */
            void SendDisconnect();

            /**
                == Send Messages Connecting ==
            */

            /**
                Sends a connected message
            */
            void SendConnected();

            /**
                Send a load world message
            */
            void SendLoadWorld();

            /**
                Send a in game message
            */
            void SendInGame();

        private:
            NetworkMessagesClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~NetworkMessagesClient();

            static NetworkMessagesClient* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}