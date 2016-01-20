#include <Doremi/Core/Include/AddRemoveSyncHandler.hpp>
#include <Doremi/Core/Include/Manager/Network/NetMessage.hpp>
#include <Doremi/Core/Include/Manager/Network/BitStreamer.h>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/SequenceMath.hpp>
#include <iostream>

namespace Doremi
{
    namespace Core
    {
        AddRemoveSyncHandler::AddRemoveSyncHandler() : m_nextSequence(0) {}


        AddRemoveSyncHandler::~AddRemoveSyncHandler() {}

        void AddRemoveSyncHandler::AddRemoveQueuedObjects()
        {
            EntityHandler& entityHandler = EntityHandler::GetInstance();

            std::list<ObjectToAddOrRemove>::iterator iter = m_frameQueuedObjects.begin();

            // For each ququed object we add or remove
            for(; iter != m_frameQueuedObjects.end(); ++iter)
            {
                if(iter->AddObject)
                {
                    entityHandler.CreateEntity((Blueprints)iter->BluePrintOrEntityID, iter->Position);
                }
                else
                {
                    entityHandler.RemoveEntity(iter->BluePrintOrEntityID);
                }
            }

            m_frameQueuedObjects.clear();
        }

        void AddRemoveSyncHandler::UpdateQueueWithSequence(uint8_t p_sequence)
        {
            uint32_t numOfRemoves = sequence_difference(p_sequence, m_nextSequence, 255);

            // Check if we can remove (error check), then remove the number of accs in difference
            if(numOfRemoves <= m_BufferedAddRemoveObjects.size())
            {
                // TODOCM check if you can remove this in one call
                for(size_t i = 0; i < numOfRemoves; i++)
                {
                    m_BufferedAddRemoveObjects.pop_front();
                    m_nextSequence++;
                }
            }
        }

        void AddRemoveSyncHandler::QueueObjectsFromFrame()
        {
            // If we got any add or removes, we queue them
            if(m_frameQueuedObjects.size())
            {
                m_BufferedAddRemoveObjects.push_back(m_frameQueuedObjects);

                // TODOCM check if it becomes reference or not => could do a pointer instead and move it
                m_frameQueuedObjects.clear();
            }
        }

        void AddRemoveSyncHandler::QueueAddObject(uint32_t p_blueprint, DirectX::XMFLOAT3 p_position)
        {
            ObjectToAddOrRemove obj = ObjectToAddOrRemove();
            obj.AddObject = true;
            obj.BluePrintOrEntityID = p_blueprint;
            obj.Position = p_position;

            m_frameQueuedObjects.push_front(obj);
        }

        void AddRemoveSyncHandler::QueueRemoveObject(uint32_t p_id)
        {
            ObjectToAddOrRemove obj = ObjectToAddOrRemove();
            obj.AddObject = false;
            obj.BluePrintOrEntityID = p_id;

            m_frameQueuedObjects.push_front(obj);
        }

        void AddRemoveSyncHandler::CheckNewAddRemoves(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesRead)
        {
            // Read AddRemove items
            uint8_t NumOfSequences = p_streamer.ReadUnsignedInt8();

            uint8_t FirstSequence = p_streamer.ReadUnsignedInt8();

            op_BytesRead += sizeof(uint8_t) * 2;

            if(NumOfSequences == 0)
            {
                return;
            }

            // While we're lower or
            while(NumOfSequences && sequence_more_recent(m_nextSequence, FirstSequence, 255))
            {
                FirstSequence++;
                NumOfSequences--;

                // Read number of new items
                uint8_t NumOfNewItems = p_streamer.ReadUnsignedInt8();

                // Add offset to next read, head + numItems*(idOrblueprint + position) + thie ceileing of how many bits for each item
                op_BytesRead += sizeof(uint8_t) + NumOfNewItems * (sizeof(uint32_t) + sizeof(float) * 3) + ceil((float)NumOfNewItems / 8.0f);
                p_streamer.SetReadWritePosition(op_BytesRead);
            }

            // Increase the last one we will use
            m_nextSequence += NumOfSequences;

            std::cout << "Added " << (uint32_t)NumOfSequences << std::endl;

            // For the remaining sequences
            while(NumOfSequences)
            {
                // Set read point
                p_streamer.SetReadWritePosition(op_BytesRead);

                // Read number of new items
                uint8_t NumOfNewItems = p_streamer.ReadUnsignedInt8();

                for(size_t i = 0; i < NumOfNewItems; i++)
                {
                    // Read data
                    bool ShouldAdd = p_streamer.ReadBool();
                    uint32_t idOrBlueprint = p_streamer.ReadUnsignedInt32();
                    DirectX::XMFLOAT3 position = p_streamer.ReadFloat3();

                    // Add to list
                    if(ShouldAdd)
                    {
                        QueueAddObject(idOrBlueprint, position);
                    }
                    else
                    {
                        QueueRemoveObject(idOrBlueprint);
                    }
                }

                // Add to offset
                op_BytesRead += sizeof(uint8_t) + NumOfNewItems * (sizeof(uint32_t) + sizeof(float) * 3) + ceil((float)NumOfNewItems / 8.0f);
                NumOfSequences--;
            }

            // Set read point for next user
            p_streamer.SetReadWritePosition(op_BytesRead);
        }

        void AddRemoveSyncHandler::WriteAddRemoves(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten)
        {
            // Queue new objects
            QueueObjectsFromFrame();

            // Check how many bytes we have left to write
            uint32_t bytesLeftToWrite = p_bufferSize - op_BytesWritten;

            // If we can't even start header
            if(bytesLeftToWrite < (sizeof(uint8_t) * 2))
            {
                return;
            }

            // Either when we save the latest sequence client got we remove the redudant there, or we do it now

            // Save position for later writing
            // TODOCM could do without this if bytesWRitten doesn't change
            uint32_t WritePositionForNumSequences = op_BytesWritten;

            // Skip for number of sequences in packet
            p_streamer.SetReadWritePosition(WritePositionForNumSequences + sizeof(uint8_t));

            // Write oldest sequence
            p_streamer.WriteUnsignedInt8(m_nextSequence);

            // Remove from
            bytesLeftToWrite -= sizeof(uint8_t) * 2;
            op_BytesWritten += sizeof(uint8_t) * 2;

            // Number of sequences counter
            uint8_t numOfSequences = 0;

            // Start going through the buffered one with oldest first, and after each write move one byte ahead, and check if next one will fir into
            // the message, if not we have to split it into smaller ones
            std::list<list<ObjectToAddOrRemove>>::iterator iter = m_BufferedAddRemoveObjects.begin();

            for(; iter != m_BufferedAddRemoveObjects.end(); ++iter)
            {
                // Check if we have enough space left
                uint32_t numOfItems = iter->size();
                uint32_t bytesToWrite = sizeof(uint8_t) + numOfItems * (sizeof(uint32_t) + sizeof(float) * 3) + ceil((float)numOfItems / 8.0f);

                if(bytesLeftToWrite < bytesToWrite)
                {
                    // Skip over the number of objects for now
                    p_streamer.SetReadWritePosition(op_BytesWritten + sizeof(uint8_t));
                    bytesLeftToWrite -= sizeof(uint8_t);

                    // We change how many bytes we write, excluding bits
                    bytesToWrite = sizeof(uint32_t) + sizeof(float) * 3;

                    // Counter for bits
                    uint8_t nextItemToBeWritten = 1;

                    // We add items manually
                    list<ObjectToAddOrRemove>::iterator iterItems = iter->begin();
                    for(; iterItems != iter->end() && bytesLeftToWrite >= (bytesToWrite + ceil((float)nextItemToBeWritten / 8.0f)); ++iterItems)
                    {
                        bytesLeftToWrite -= bytesToWrite;
                        nextItemToBeWritten++;

                        p_streamer.WriteBool(iterItems->AddObject);
                        p_streamer.WriteUnsignedInt32(iterItems->BluePrintOrEntityID);
                        p_streamer.WriteFloat3(iterItems->Position);
                    }

                    // from the iterator to the end we split into a new sequence and put it between the big iterator

                    //// Create new list from leftovers
                    // std::list<ObjectToAddOrRemove> newList = std::list<ObjectToAddOrRemove>(iterItems, iter->end());

                    //// Remove from list
                    // iter->erase(iterItems, iter->end());

                    // Direct copy
                    std::list<ObjectToAddOrRemove> newList;
                    newList.splice(newList.begin(), *iter, iterItems, iter->end());

                    // Insert new list in between
                    m_BufferedAddRemoveObjects.insert(iter, newList);

                    // Set writing position to valid
                    numOfSequences++;

                    // Step back in buffer and write the number of items we added
                    p_streamer.SetReadWritePosition(op_BytesWritten);
                    p_streamer.WriteUnsignedInt8(nextItemToBeWritten - 1);

                    // Add how many bytes we've written
                    op_BytesWritten += bytesToWrite + ceil((float)(nextItemToBeWritten - 1) / 8.0f);

                    break;
                }

                // Remove from our bytes left to write
                bytesLeftToWrite -= bytesToWrite;

                // Write number of items
                p_streamer.WriteUnsignedInt8(numOfItems);

                // Go through all items and write them to message
                list<ObjectToAddOrRemove>::iterator iterItems = iter->begin();
                for(; iterItems != iter->end(); ++iterItems)
                {
                    p_streamer.WriteBool(iterItems->AddObject);
                    p_streamer.WriteUnsignedInt32(iterItems->BluePrintOrEntityID);
                    p_streamer.WriteFloat3(iterItems->Position);
                }

                // Increment how many bytes we've written
                op_BytesWritten += sizeof(uint8_t) + numOfItems * (sizeof(uint32_t) + sizeof(float) * 3) + ceil((float)numOfItems / 8.0f);

                // Move our position to whole new byte
                p_streamer.SetReadWritePosition(op_BytesWritten);

                numOfSequences++;
            }

            // Write number of sequences
            p_streamer.SetReadWritePosition(WritePositionForNumSequences);
            p_streamer.WriteUnsignedInt8(numOfSequences);

            // Set writing position to right position
            p_streamer.SetReadWritePosition(op_BytesWritten);
        }

        uint8_t AddRemoveSyncHandler::GetNextSequenceUsed() { return m_nextSequence; }
    }
}