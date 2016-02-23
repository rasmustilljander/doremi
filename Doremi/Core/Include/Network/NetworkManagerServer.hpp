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
                Check for reliable connections to accept
            */
            void CheckForConnections();

            /**
                Update timers for clients, and check if anyone got timeouted
            */
            void UpdateTimeouts(double t_dt);

            /**
                Timer for next send
            */
            double m_nextUpdateTimer;

            /**
                Time interval to next send
            */
            double m_updateInterval;

            /**
                Time interval for timeouts
            */
            double m_timeoutInterval;


            /**
                Max number of connections
            */
            uint8_t m_maxConnection;

            /**
                TODOCM doc
            */
            uint8_t m_nextSnapshotSequence;
            /**

                test
            */
            uint32_t counter;

            /**
                TODOCM doc
                TODOCM move to external place?
            */
            std::list<uint32_t> m_SavedPlayerIDs;

            uint8_t m_maxConnectingMessagesPerFrame;

            uint8_t m_maxConnectedMessagesPerFrame;
        };
    }
}
