#pragma once
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <DirectXMath.h>
#include <list>

namespace Doremi
{
    namespace Core
    {
        class NetworkStreamer;
        class Event;
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

            std::list<Event*> GetEventsReceivedFromServer();

        private:
            Event* InterpetEvent(NetworkStreamer& p_streamer, uint32_t& op_BytesRead);
            void InterpetEventAndThrow(NetworkStreamer& p_streamer, uint32_t& op_bitsRead);
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
