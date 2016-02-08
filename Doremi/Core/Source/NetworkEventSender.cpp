#include <Doremi/Core/Include/NetworkEventSender.hpp>
#include <Doremi/Core/Include/Manager/Network/NetMessage.hpp>
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/SequenceMath.hpp>
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <iostream>

namespace Doremi
{
    namespace Core
    {
        NetworkEventSender::NetworkEventSender() : m_nextSequence(0) {}

        NetworkEventSender::~NetworkEventSender() {}

        void NetworkEventSender::UpdateBufferWithRecievedClientSequenceAcc(uint8_t p_sequence)
        {
            int16_t numOfRemoves = sequence_difference(p_sequence, m_nextSequence, 255);

            if(numOfRemoves <= 0)
            {
                return;
            }

            if(numOfRemoves > m_bufferedQueuesOfEvent.size())
            {
                cout << "Error in acc, more exist then pattern provides, shouldn't be possible in the real game" << endl;
            }

            // Check if we can remove (error check), then remove the number of accs in difference
            numOfRemoves = min(numOfRemoves, m_bufferedQueuesOfEvent.size());


            // TODOCM check if you can remove this in one call
            for(size_t i = 0; i < numOfRemoves; i++)
            {
                m_bufferedQueuesOfEvent.pop_front();
                m_nextSequence++;
            }
        }

        void NetworkEventSender::QueueEventToFrame(Event* p_frameEvent) { m_frameQueuedEvents.push_back(p_frameEvent); }

        void NetworkEventSender::AddFrameQueuedObjectsToBuffer()
        {
            // If we got any add or removes, we queue them
            if(m_frameQueuedEvents.size())
            {
                m_bufferedQueuesOfEvent.push_back(m_frameQueuedEvents);

                // TODOCM check if it becomes reference or not => could do a pointer instead and move it
                m_frameQueuedEvents.clear();
            }
        }

        void NetworkEventSender::WriteEvents(NetworkStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten, bool& p_finished)
        {
            // Add events from current frame
            AddFrameQueuedObjectsToBuffer();

            // Check how many bytes we have left to write
            uint32_t bytesLeftToWrite = p_bufferSize - op_BytesWritten;

            // Get header size
            uint32_t t_headerSize = sizeof(uint8_t) * 2;

            // Check if we have room for header
            if(bytesLeftToWrite < t_headerSize)
            {
                p_finished = false;
                return;
            }

            // Save position for later writing
            uint32_t WritePositionForNumSequences = op_BytesWritten;

            // Jump over the write of number of sequences in packet, we get back here later
            p_streamer.SetReadWritePosition(WritePositionForNumSequences + sizeof(uint8_t));
            bytesLeftToWrite -= sizeof(uint8_t);
            op_BytesWritten += sizeof(uint8_t);

            // Write the most oldest sequence client doesn't have
            p_streamer.WriteUnsignedInt8(m_nextSequence);
            bytesLeftToWrite -= sizeof(uint8_t);
            op_BytesWritten += sizeof(uint8_t);

            // Number of sequences counter, used to write later
            uint8_t numOfSequences = 0;

            // Start going through the buffered queues with oldest first, and after each queue write move one byte ahead, and check if next queu will
            // fit into
            // the message, if not we have to split it into smaller ones
            std::list<list<Event*>>::iterator iter = m_bufferedQueuesOfEvent.begin();

            // Set to pre value that we will finish, if false we can't continue
            p_finished = true;

            for(; iter != m_bufferedQueuesOfEvent.end() && p_finished; ++iter)
            {
                // Counter for items
                uint32_t numOfItems = 0;
                // uint32_t bytesToWrite = sizeof(uint8_t) + numOfItems * (sizeof(uint32_t) + sizeof(float) * 3) + ceil((float)numOfItems / 8.0f);

                // If we don't have enough memory to write header
                if(bytesLeftToWrite < sizeof(uint8_t))
                {
                    p_finished = false;
                    break;
                }

                // Save position to write number of items
                uint32_t WritePositionForNumEvents = op_BytesWritten;
                p_streamer.SetReadWritePosition(WritePositionForNumEvents + sizeof(uint8_t)); // If we have small events we could possibly have more
                // then 255 TODOCM maybe revalue this
                bytesLeftToWrite -= sizeof(uint8_t);
                op_BytesWritten += sizeof(uint8_t);

                // Need a variable to check bits written
                uint32_t t_totalBitsWritten = 0;

                int32_t t_bitsLeftToWrite = bytesLeftToWrite * 8;

                // Go through all events and write them to the message
                list<Event*>::iterator iterEvents = iter->begin();
                for(; iterEvents != iter->end() && t_bitsLeftToWrite > 0 && numOfItems < 255; ++iterEvents)
                {
                    uint32_t t_bitsWrittenPerObject = 0;

                    // Write event id
                    p_streamer.WriteUnsignedInt32(static_cast<uint32_t>((*iterEvents)->eventType));
                    t_bitsWrittenPerObject += sizeof(uint32_t) * 8;

                    // If we read more then we can, we need to exclude the one who didn't get to write whole
                    (*iterEvents)->Write(&p_streamer, t_bitsWrittenPerObject);

                    // Get increment for rollback
                    t_bitsLeftToWrite -= t_bitsWrittenPerObject;
                    t_totalBitsWritten += t_bitsWrittenPerObject;

                    numOfItems++;
                }

                // If we went too far, also need to move back the memory pointer
                if(t_bitsLeftToWrite < 0)
                {
                    // Should cause us to go into next if statement
                    --iterEvents;

                    // Rollback the memory that is invalid
                    t_totalBitsWritten += t_bitsLeftToWrite;

                    // Set to not finished
                    p_finished = false;
                }

                // If we didn't reach the last one, we push the items left in front of the next
                if(iterEvents != iter->end())
                {
                    // Direct copy
                    std::list<Event*> newList;

                    newList.splice(newList.begin(), *iter, iterEvents, iter->end());

                    // Insert new list in between
                    std::list<list<Event*>>::iterator tempIter = iter;
                    std::advance(tempIter, 1);
                    m_bufferedQueuesOfEvent.insert(tempIter, newList);
                }


                // Write number of items to the saved position
                p_streamer.SetReadWritePosition(WritePositionForNumEvents);
                p_streamer.WriteUnsignedInt8(numOfItems);

                // Increments written by full byte
                uint32_t fullBytesWritten = ceil((float)t_totalBitsWritten / 8.0f);
                op_BytesWritten += fullBytesWritten;
                bytesLeftToWrite -= fullBytesWritten;

                // Move our position to whole new byte
                p_streamer.SetReadWritePosition(op_BytesWritten);

                // Increment buffered queues
                numOfSequences++;

                // If there are too many sequences, we cannot go over 126, cause then sequence will become a problem
                if(numOfSequences > 126)
                {
                    // Set to not fininsh
                    p_finished = false;
                }
            }

            // Write how many parts we send
            p_streamer.SetReadWritePosition(WritePositionForNumSequences);
            p_streamer.WriteUnsignedInt8(numOfSequences);

            // set back streamer
            p_streamer.SetReadWritePosition(op_BytesWritten);
        }

        //    void NetworkEventSender::WriteAddRemoves(NetworkStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten)
        //    {
        //        // Queue new objects
        //        AddFrameQueuedObjectsToBuffer();

        //        // Check how many bytes we have left to write
        //        uint32_t bytesLeftToWrite = p_bufferSize - op_BytesWritten;

        //        // If we can't even start header
        //        if(bytesLeftToWrite < (sizeof(uint8_t) * 2))
        //        {
        //            return;
        //        }

        //        // Either when we save the latest sequence client got we remove the redudant there, or we do it now

        //        // Save position for later writing
        //        // TODOCM could do without this if bytesWRitten doesn't change
        //        uint32_t WritePositionForNumSequences = op_BytesWritten;

        //        // Skip for number of sequences in packet
        //        p_streamer.SetReadWritePosition(WritePositionForNumSequences + sizeof(uint8_t));

        //        // Write oldest sequence
        //        p_streamer.WriteUnsignedInt8(m_nextSequence);

        //        // Remove from
        //        bytesLeftToWrite -= sizeof(uint8_t) * 2;
        //        op_BytesWritten += sizeof(uint8_t) * 2;

        //        // Number of sequences counter
        //        uint8_t numOfSequences = 0;

        //        // Start going through the buffered one with oldest first, and after each write move one byte ahead, and check if next one will fir
        //        into
        //        // the message, if not we have to split it into smaller ones
        //        std::list<list<ObjectToAddOrRemove>>::iterator iter = m_BufferedAddRemoveObjects.begin();

        //        for(; iter != m_BufferedAddRemoveObjects.end(); ++iter)
        //        {
        //            // Check if we have enough space left
        //            uint32_t numOfItems = iter->size();
        //            uint32_t bytesToWrite = sizeof(uint8_t) + numOfItems * (sizeof(uint32_t) + sizeof(float) * 3) + ceil((float)numOfItems / 8.0f);

        //            if(bytesLeftToWrite < bytesToWrite)
        //            {
        //                // Skip over the number of objects for now
        //                p_streamer.SetReadWritePosition(op_BytesWritten + sizeof(uint8_t));
        //                bytesLeftToWrite -= sizeof(uint8_t);

        //                // We change how many bytes we write, excluding bits
        //                bytesToWrite = sizeof(uint32_t) + sizeof(float) * 3;

        //                // Counter for bits
        //                uint8_t nextItemToBeWritten = 1;

        //                // We add items manually
        //                list<ObjectToAddOrRemove>::iterator iterItems = iter->begin();
        //                for(; iterItems != iter->end() && bytesLeftToWrite >= (bytesToWrite + ceil((float)nextItemToBeWritten / 8.0f)); ++iterItems)
        //                {
        //                    bytesLeftToWrite -= bytesToWrite;
        //                    nextItemToBeWritten++;

        //                    p_streamer.WriteBool(iterItems->AddObject);
        //                    p_streamer.WriteUnsignedInt32(iterItems->BluePrintOrEntityID);
        //                    p_streamer.WriteFloat3(iterItems->Position);
        //                }

        //                // from the iterator to the end we split into a new sequence and put it between the big iterator

        //                //// Create new list from leftovers
        //                // std::list<ObjectToAddOrRemove> newList = std::list<ObjectToAddOrRemove>(iterItems, iter->end());

        //                //// Remove from list
        //                // iter->erase(iterItems, iter->end());

        //                // Direct copy
        //                std::list<ObjectToAddOrRemove> newList;
        //                newList.splice(newList.begin(), *iter, iterItems, iter->end());

        //                // Insert new list in between
        //                m_BufferedAddRemoveObjects.insert(iter, newList);

        //                // Set writing position to valid
        //                numOfSequences++;

        //                // Step back in buffer and write the number of items we added
        //                p_streamer.SetReadWritePosition(op_BytesWritten);
        //                p_streamer.WriteUnsignedInt8(nextItemToBeWritten - 1);

        //                // Add how many bytes we've written
        //                op_BytesWritten += bytesToWrite + ceil((float)(nextItemToBeWritten - 1) / 8.0f);

        //                break;
        //            }

        //            // Remove from our bytes left to write
        //            bytesLeftToWrite -= bytesToWrite;

        //            // Write number of items
        //            p_streamer.WriteUnsignedInt8(numOfItems);

        //            // Go through all items and write them to message
        //            list<ObjectToAddOrRemove>::iterator iterItems = iter->begin();
        //            for(; iterItems != iter->end(); ++iterItems)
        //            {
        //                p_streamer.WriteBool(iterItems->AddObject);
        //                p_streamer.WriteUnsignedInt32(iterItems->BluePrintOrEntityID);
        //                p_streamer.WriteFloat3(iterItems->Position);
        //            }

        //            // Increment how many bytes we've written
        //            op_BytesWritten += sizeof(uint8_t) + numOfItems * (sizeof(uint32_t) + sizeof(float) * 3) + ceil((float)numOfItems / 8.0f);

        //            // Move our position to whole new byte
        //            p_streamer.SetReadWritePosition(op_BytesWritten);

        //            numOfSequences++;
        //        }

        //        // Write number of sequences
        //        p_streamer.SetReadWritePosition(WritePositionForNumSequences);
        //        p_streamer.WriteUnsignedInt8(numOfSequences);

        //        // Set writing position to right position
        //        p_streamer.SetReadWritePosition(op_BytesWritten);
        //    }

        uint8_t NetworkEventSender::GetNextSequenceUsed() { return m_nextSequence; }
    }
}