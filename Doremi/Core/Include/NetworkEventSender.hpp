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
            /**
                TODOCM doc
            */
            NetworkEventSender();

            /**
                TODOCM doc
            */
            ~NetworkEventSender();

            void AddRemoveQueuedObjects();

            void UpdateBufferWithRecievedClientSequenceAcc(uint8_t p_sequence);


            /**
                TODOCM doc
            */
            void QueueEventToFrame(Event* p_frameEvent);


            void AddFrameQueuedObjectsToBuffer();

            /**
                TODOCM doc
            */
            void QueueRemoveObject(uint32_t p_id);

            /**
                TODOCM doc
            */
            void CheckNewAddRemoves(NetworkStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesRead);

            void WriteEvents(NetworkStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten, bool& p_finished);

            /**
                TODOCM doc
            */
            uint8_t GetNextSequenceUsed();

        private:
            /**
                TODOCM doc
            */
            std::list<Event*> m_frameQueuedEvents;

            /**
                TODOCM doc
            */
            uint8_t m_nextSequence;

            /**
                TODOCM doc
            */
            std::list<std::list<Event*>> m_bufferedQueuesOfEvent;
        };
    }
}
