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
            NetworkEventReceiver();

            ~NetworkEventReceiver();

            /**
                Read events from a streamer and buffer them to a frame
            */
            void ReadEvents(NetworkStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesRead);

            /**
                Get the next sequence
            */
            uint8_t GetNextSequenceUsed();

            /**
                Return all eventes queued on server and
            */
            std::list<Event*> GetEventsReceivedFromServer();

        private:
            /**
                Interpet events and remove them, used when we already have used the events
            */
            void InterpetEventAndThrow(NetworkStreamer& p_streamer, uint32_t& op_bitsRead);

            /**
                Interpet events and queue them to our frame queue
            */
            void InterpetEventAndQueue(NetworkStreamer& p_streamer, uint32_t& op_bitsRead);

            /**
                Our queued events for the frame
            */
            std::list<Event*> m_frameQueuedEvents;

            /**
                The sequence we've currently used
            */
            uint8_t m_currentSequence;
        };
    }
}
