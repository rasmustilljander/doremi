#pragma once

// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>

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
        class NetworkManagerClient : public Manager
        {
        public:
            /**
                TODOCM doc
            */
            NetworkManagerClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOCM doc
            */
            virtual ~NetworkManagerClient();

            /**
                Load IP to server and playerID
            */
            void LoadConfigFile(const DoremiEngine::Core::SharedContext& p_sharedContext);


            /**
                TODOCM doc
            */
            void SetServerIP(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

            /**
                TODOCM doc
            */
            void Update(double p_dt) override;


        private:
            /**
                Receives connecting and connected messages
            */
            void ReceiveMessages();

            /**
                Recieves connecting messages from server
            */
            void ReceiveConnectingMessages();

            /**
                Recieves connected messages from server
            */
            void ReceiveConnectedMessages();

            /**
                Sends reliable & unreliable messages
            */
            void SendMessages(double p_dt);

            /**
                TODOCM doc
            */
            void SendConnectingMessages(double p_dt);

            /**
                TODOCM doc
            */
            void SendConnectedMessages();


            /**
                Update timeout to server and disconnects if too long
            */
            void UpdateTimeouts(double p_dt);


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

            /**
                TODOCM doc
            */
            uint32_t m_numJoinEvents;

            /**
                TODOCM doc
            */
            uint8_t m_maxConnectingMessagesPerFrame;

            uint8_t m_maxConnectedMessagesPerFrame;
        };
    }
}
