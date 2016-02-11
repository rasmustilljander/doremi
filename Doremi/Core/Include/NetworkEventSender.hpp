#pragma once
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <DirectXMath.h>
#include <list>

namespace Doremi
{
    namespace Core
    {
        class NetworkStreamer;
        struct Event;
        struct NetMessage;

        /**
            TODOCM doc
        */
        struct ObjectToAddOrRemove
        {
            bool AddObject;
            uint32_t BluePrintOrEntityID;
            DirectX::XMFLOAT3 Position;
        };

        /**
            TODOCM doc
        */
        class NetworkEventSender
        {
        public:
            NetworkEventSender();

            ~NetworkEventSender();

            /**
                Remove buffered events with a acc sequence input
            */
            void UpdateBufferWithRecievedClientSequenceAcc(uint8_t p_sequence);

            /**
                Queue a event to a frame queue to later be sent
            */
            void QueueEventToFrame(Event* p_frameEvent);

            /**
                Take the frame queu and add it to big buffer
            */
            void AddFrameQueuedObjectsToBuffer();

            /**
                Write events to a streamer
            */
            void WriteEvents(NetworkStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten, bool& p_finished);

            /**
                Get the latest sequence we're useing
            */
            uint8_t GetNextSequenceUsed();

        private:
            /**
                Queued events for the current frame
            */
            std::list<Event*> m_frameQueuedEvents;

            /**
                The sequence we're using
            */
            uint8_t m_nextSequence;

            /**
                Buffer of framequeued events
            */
            std::list<std::list<Event*>> m_bufferedQueuesOfEvent;
        };
    }
}
