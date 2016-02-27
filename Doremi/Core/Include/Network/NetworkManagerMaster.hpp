#pragma once

// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>

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
        class NetworkManagerMaster : public Manager
        {
        public:
            /**
            TODOCM doc
            */
            NetworkManagerMaster(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
            TODOCM doc
            */
            virtual ~NetworkManagerMaster();

            /**
            TODOCM doc
            */
            void Update(double p_dt) override;

        private:
            /**
                Recieves reliable and unreliable messages
            */
            void ReceiveMessages();

            void ReceiveMessagesClients();

            void ReceiveMessagesServers();


            /**
                Send messages to connected clients
            */
            void SendMessages();

            void SendMessagesClients();

            void SendMessagesServers();

            /**
            Update timers for clients, and check if anyone got timeouted
            */
            void UpdateTimeouts(double t_dt);

            /**
            Time interval for timeouts
            */
            double m_timeoutInterval;


            /**
            TODOCM doc
            */
            uint8_t m_maxClientMessagesPerFrame;

            uint8_t m_maxServerMessagesPerFrame;
        };
    }
}
