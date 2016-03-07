#pragma once

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
        struct NetMessageServerClientConnectingFromServer;
        struct NetMessageServerClientConnectedFromServer;
        struct NetMessageMasterClientFromMaster;

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
            void ReceiveVersionCheck(NetMessageServerClientConnectingFromServer& p_message);

            /**
                Receive a connect message
            */
            void ReceiveConnect(NetMessageServerClientConnectingFromServer& p_message);

            /**
                Receive a disconnect messages
            */
            void ReceiveDisconnect(NetMessageServerClientConnectingFromServer& p_message);

            /**
                == Receive Messages Connected ==
            */

            /**
                Receive connected message
            */
            void ReceiveConnected(NetMessageServerClientConnectedFromServer& p_message);

            /**
                Receive load world message
            */
            void ReceiveLoadWorld(NetMessageServerClientConnectedFromServer& p_message);

            /**
                Receive in game message
            */
            void ReceiveInGame(NetMessageServerClientConnectedFromServer& p_message);

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

            void ReceiveConnectedMaster(NetMessageMasterClientFromMaster& p_message);

            void ReceiveDisconnectMaster(NetMessageMasterClientFromMaster& p_message);

            void SendConnectionRequestMaster();

            void SendConnectedMaster();

            void SendDisconnectMaster();

        private:
            explicit NetworkMessagesClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~NetworkMessagesClient();

            static NetworkMessagesClient* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}