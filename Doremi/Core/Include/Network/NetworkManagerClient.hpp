#pragma once

// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>

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
        class NetworkManagerClient : public Manager, public Subscriber
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

            /**
                TODOCM doc
            */
            void OnEvent(Event* p_event);

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
                Time to disconnect from server
            */
            double m_timeoutIntervalConnecting;

            /**
                TODOCM doc
            */
            double m_timeoutIntervalConnected;

            /**
                TODOCM doc
            */
            uint8_t m_maxConnectingMessagesPerFrame;

            /**
                TODOCM doc
            */
            uint8_t m_maxConnectedMessagesPerFrame;
        };
    }
}
