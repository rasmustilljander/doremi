#include <Doremi/Core/Include/NetworkEventReceiver.hpp>
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>
#include <Doremi/Core/Include/SequenceMath.hpp>

// Events
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>

#include <Doremi/Core/Include/EventInterpeter.hpp>

#include <iostream>

namespace Doremi
{
    namespace Core
    {
        NetworkEventReceiver::NetworkEventReceiver() : m_currentSequence(0) {}

        NetworkEventReceiver::~NetworkEventReceiver() {}

        void NetworkEventReceiver::InterpetEventAndThrow(NetworkStreamer& p_streamer, uint32_t& op_bitsRead)
        {
            // Read number of new items
            uint8_t NumOfNewItems = p_streamer.ReadUnsignedInt8();
            op_bitsRead += sizeof(uint8_t) * 8;

            // Read the items
            for(size_t i = 0; i < NumOfNewItems; i++)
            {
                Event* t_tempEvent = InterpetEvent(p_streamer, op_bitsRead);

                delete t_tempEvent;
            }
        }

        void NetworkEventReceiver::InterpetEventAndQueue(NetworkStreamer& p_streamer, uint32_t& op_bitsRead)
        {
            // Read number of new items
            uint8_t NumOfNewItems = p_streamer.ReadUnsignedInt8();
            op_bitsRead += sizeof(uint8_t) * 8;

            // Read the items
            for(size_t i = 0; i < NumOfNewItems; i++)
            {
                Event* t_tempEvent = InterpetEvent(p_streamer, op_bitsRead);

                // static int eventAdded = 0;
                // std::cout << eventAdded << " with type: " << (uint32_t)(t_tempEvent->eventType) << std::endl;
                // eventAdded++;

                // Queue events
                m_frameQueuedEvents.push_back(t_tempEvent);
            }
        }

        void NetworkEventReceiver::ReadEvents(NetworkStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesRead)
        {
            // Get header size
            uint32_t t_headerSize = sizeof(uint8_t) * 2;

            // Check how many bytes we have left to write
            uint32_t bytesLeftToRead = p_bufferSize - op_BytesRead;

            // Check if we can read headers
            if(bytesLeftToRead < t_headerSize)
            {
                return;
            }

            // Read number of sequences packets
            uint8_t NumOfSequencesLeft = p_streamer.ReadUnsignedInt8();
            bytesLeftToRead -= sizeof(uint8_t);
            op_BytesRead += sizeof(uint8_t);

            // Read starting sequence
            uint8_t CurrentSequence = p_streamer.ReadUnsignedInt8();
            bytesLeftToRead -= sizeof(uint8_t);
            op_BytesRead += sizeof(uint8_t);

            // While we still got sequences to read, and already have them, we skip to use
            while(NumOfSequencesLeft && sequence_more_recent(m_currentSequence, CurrentSequence, 255))
            {
                // Increment how many we read, and how many that are left
                CurrentSequence++;
                NumOfSequencesLeft--;

                uint32_t t_bitsRead = 0;

                // If we don't have enough memory to read next line
                if(bytesLeftToRead < sizeof(uint8_t))
                {
                    return;
                }

                // Interpet events and throw
                InterpetEventAndThrow(p_streamer, t_bitsRead);

                // Get full bytes read
                uint32_t t_FullBytesRead = ceil((float)t_bitsRead / 8.0f);
                bytesLeftToRead -= t_FullBytesRead;
                op_BytesRead += t_FullBytesRead;

                // Set read point
                p_streamer.SetReadWritePosition(op_BytesRead);
            }

            // Increment by how many we should read, could move this for each we read instead
            m_currentSequence += NumOfSequencesLeft;

            // While we still have sequences left to read
            while(NumOfSequencesLeft)
            {
                uint32_t t_bitsRead = 0;

                // If we don't have enough memory to read next line
                if(bytesLeftToRead < sizeof(uint8_t))
                {
                    return;
                }

                // Interpet events and save
                InterpetEventAndQueue(p_streamer, t_bitsRead);

                // Get full bytes read
                uint32_t t_FullBytesRead = ceil((float)t_bitsRead / 8.0f);
                bytesLeftToRead -= t_FullBytesRead;
                op_BytesRead += t_FullBytesRead;

                // Set read point
                p_streamer.SetReadWritePosition(op_BytesRead);

                // Decrement counter
                NumOfSequencesLeft--;
            }
        }

        uint8_t NetworkEventReceiver::GetNextSequenceUsed() { return m_currentSequence; }

        std::list<Event*> NetworkEventReceiver::GetEventsReceivedFromServer()
        {
            std::list<Event*> r_eventList = m_frameQueuedEvents;

            m_frameQueuedEvents = std::list<Event*>();

            return r_eventList;
        }

        // void NetworkEventSender::CheckNewAddRemoves(NetworkStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesRead)
        //{
        //    // Read AddRemove items
        //    uint8_t NumOfSequences = p_streamer.ReadUnsignedInt8();

        //    uint8_t FirstSequence = p_streamer.ReadUnsignedInt8();

        //    op_BytesRead += sizeof(uint8_t) * 2;

        //    if (NumOfSequences == 0)
        //    {
        //        return;
        //    }

        //    // While we're lower or
        //    while (NumOfSequences && sequence_more_recent(m_nextSequence, FirstSequence, 255))
        //    {
        //        FirstSequence++;
        //        NumOfSequences--;

        //        // Read number of new items
        //        uint8_t NumOfNewItems = p_streamer.ReadUnsignedInt8();

        //        // Add offset to next read, head + numItems*(idOrblueprint + position) + thie ceileing of how many bits for each item
        //        op_BytesRead += sizeof(uint8_t) + NumOfNewItems * (sizeof(uint32_t) + sizeof(float) * 3) + ceil((float)NumOfNewItems / 8.0f);
        //        p_streamer.SetReadWritePosition(op_BytesRead);
        //    }

        //    // Increase the last one we will use
        //    m_nextSequence += NumOfSequences;

        //    // For the remaining sequences
        //    while (NumOfSequences)
        //    {
        //        // Set read point
        //        p_streamer.SetReadWritePosition(op_BytesRead);

        //        // Read number of new items
        //        uint8_t NumOfNewItems = p_streamer.ReadUnsignedInt8();

        //        for (size_t i = 0; i < NumOfNewItems; i++)
        //        {
        //            // Read data
        //            bool ShouldAdd = p_streamer.ReadBool();
        //            uint32_t idOrBlueprint = p_streamer.ReadUnsignedInt32();
        //            DirectX::XMFLOAT3 position = p_streamer.ReadFloat3();

        //            // Add to list
        //            if (ShouldAdd)
        //            {
        //                //QueueAddObject(idOrBlueprint, position);
        //            }
        //            else
        //            {
        //                //QueueRemoveObject(idOrBlueprint);
        //            }
        //        }

        //        // Add to offset
        //        op_BytesRead += sizeof(uint8_t) + NumOfNewItems * (sizeof(uint32_t) + sizeof(float) * 3) + ceil((float)NumOfNewItems / 8.0f);
        //        NumOfSequences--;
        //    }

        //    // Set read point for next user
        //    p_streamer.SetReadWritePosition(op_BytesRead);
        //}
    }
}
