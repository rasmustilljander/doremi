#pragma once

// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/Manager/Network/ConnectionState.hpp>

namespace DoremiEngine
{
    namespace Network
    {
        class Adress;
    }
}

namespace Doremi
{
    namespace Core
    {
        struct NetMessage;
        /**
            Manager class for Client, contains the logic for the connection pattern
        */
        class ClientNetworkManager : public Manager
        {
        public:
            /**
                TODOCM doc
            */
            ClientNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOCM doc
            */
            virtual ~ClientNetworkManager();

            /**
                TODOCM doc
            */
            void LoadIPFromConfigFile(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOCM doc
            */
            void Update(double p_dt) override;

            /**
                TODOCM doc
            */
            void SetServerIP(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

        private:
            /**
                Recieves reliable & unreliable messages
            */
            void RecieveMessages(double p_dt);

            /**
                Sends reliable & unreliable messages
            */
            void SendMessages(double p_dt);

            /**
                Sends a connection request message to server
            */
            void SendConnectRequestMessage();

            /**
                Sends a version check messag eto server
            */
            void SendVersionMessage();

            /**
                Sends a connected message to server
            */
            void SendConnectedMessage();

            /**
                TODOCM doc
            */
            void SendMapLoadingMessage();

            /**
                TODOCM doc
            */
            void SendInGameMessage();

            /**
                Sends a disconnect message to server
            */
            void SendDisconnectMessage();

            /**
                Send unreliable message dependent on connectionstate
            */
            void RecieveUnreliable(double p_dt);

            /**
                Recieves reliable data from server
            */
            void RecieveReliable(double p_dt);

            /**
                TODOCM doc
            */
            void RecieveConnected(NetMessage& p_message);

            /**
                TODOCM doc
            */
            void RecieveMapLoading(NetMessage& p_message);

            /**
                TODOCM doc
            */
            void RecieveSnapshot(NetMessage& p_message, bool p_initial);

            /**
                Update timeout to server and disconnects if too long
            */
            void UpdateTimeouts(double p_dt);

            /**
                TODOCM doc
            */
            void CreateInputMessage(NetMessage& p_message);

            /**
                Timer for next update(send data)
            */
            double m_nextUpdateTimer;

            /**
                Time until next update(send data)
            */
            double m_updateInterval;

            /**
                State of connection towards master
            */
            ConnectionState m_masterConnectionState;

            /**
                State of connection towards server (can only be connected to one server)
            */
            ConnectionState m_serverConnectionState;

            /**
                Adress to server unreliable
            */
            DoremiEngine::Network::Adress* m_unreliableServerAdress;

            /**
                TODOCM doc
            */
            DoremiEngine::Network::Adress* m_reliableServerAdress;

            /**
                Adress to server reliable
            */
            DoremiEngine::Network::Adress* m_masterServerAdress;

            /**
                Socket for unreliable data to server
            */
            size_t m_serverUnreliableSocketHandle;

            /**
                Socket for reliable data to server
            */
            size_t m_serverReliableSocketHandle;

            /**
                Socket for unreliable data to master
            */
            size_t m_masterUnreliableServerSocketHandle;

            /**
                Socket for reliable data to master
                TODOCM not sure if need this, maybe for meta data?
            */
            size_t m_masterReliableServerSocketHandle;

            /**
                Last response time in milliseconds to master server
            */
            double m_masterServerLastResponse;

            /**
                Last response time in milliseconds to server
            */
            double m_serverLastResponse;

            /**
                Time to disconnect from server
            */
            double m_timeoutInterval;

            /**
                TODOCM doc
            */
            uint32_t m_playerID;
        };
    }
}
