#pragma once

// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

// Standard libraries
#include <map>
#include <list>

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
        /**
            Manager class for Server, contains the logic for the connection pattern
        */
        class NetworkManagerServer : public Manager
        {
        public:
            /**
                TODOCM doc
            */
            NetworkManagerServer(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOCM doc
            */
            virtual ~NetworkManagerServer();

            /**
                TODOCM doc
            */
            void Update(double p_dt) override;

        private:
            /**
                Recieves reliable and unreliable messages
            */
            void ReceiveMessages();

            /**
                Check for incomming connecting messages
            */
            void ReceiveConnectingMessages();

            /**
                Check for incomming connected messages
            */
            void ReceiveConnectedMessages();

            /**
                Check incomming messages from master
            */
            void ReceiveMasterMessages();

            /**
                Send messages to connected clients
            */
            void SendConnectedMessages();

            void SendMasterMessages(double p_dt);

            /**
                Check for reliable connections to accept
            */
            void CheckForConnections();

            /**
                Update timers for clients, and check if anyone got timeouted
            */
            void UpdateTimeouts(double p_dt);

            void UpdateTimeoutsConnecting(double p_dt);

            void UpdateTimeoutsConnected(double p_dt);

            void UpdateTimeoutsMaster(double p_dt);

            /**
                Time interval for timeouts
            */
            double m_timeoutIntervalConnecting;

            /**
                Time interval for timeouts
            */
            double m_timeoutIntervalConnected;

            double m_timeoutIntervalMaster;

            double m_masterNextUpdateTimer;

            double m_masterUpdateInterval;

            /**
                TODOCM doc
            */
            uint8_t m_maxConnectingMessagesPerFrame;

            uint8_t m_maxConnectedMessagesPerFrame;

            uint8_t m_maxAcceptConnectionsPerFrame;
        };
    }
}
