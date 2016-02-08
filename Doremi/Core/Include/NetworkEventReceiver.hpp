#pragma once
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <DirectXMath.h>
#include <list>
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>

namespace Doremi
{
    namespace Core
    {
        class NetworkStreamer;
        struct NetMessage;

        /**
        TODOCM doc
        */
        class NetworkEventReceiver
        {
        public:
            /**
                TODOCM doc
            */
            NetworkEventReceiver();

            /**
                TODOCM doc
            */
            ~NetworkEventReceiver();


            /**
                TODOCM doc
            */
            void ReadEvents(NetworkStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesRead);

            /**
                TODOCM doc
            */
            uint8_t GetNextSequenceUsed();

            /**
                TODMCM doc
            */
            std::list<Event*> GetEventsReceivedFromServer();

        private:
            /**
            TODOCM doc
            */
            Event* InterpetEvent(NetworkStreamer& p_streamer, uint32_t& op_BytesRead);

            /**
                TODOCM doc
            */
            void InterpetEventAndThrow(NetworkStreamer& p_streamer, uint32_t& op_bitsRead);

            /**
                TODOCM doc
            */
            void InterpetEventAndQueue(NetworkStreamer& p_streamer, uint32_t& op_bitsRead);

            /**
                TODOCM doc
            */
            std::list<Event*> m_frameQueuedEvents;

            /**
                TODOCM doc
            */
            uint8_t m_currentSequence;
        };
    }
}
