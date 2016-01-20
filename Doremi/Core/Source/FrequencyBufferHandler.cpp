#include <FrequencyBufferHandler.hpp>
#include <Manager/Network/BitStreamer.h>
#include <SequenceMath.hpp>
#include <AudioHandler.hpp>
#include <iostream>

namespace Doremi
{
    namespace Core
    {
        FrequencyBufferHandler::FrequencyBufferHandler() : m_bufferDelay(10)
        {
            testfloat = 0.0f;
            first = true;
        }

        FrequencyBufferHandler::~FrequencyBufferHandler() {}

        float FrequencyBufferHandler::GetFrequencyForFrame()
        {
            m_realSequence++;

            // Current Sequence saved
            uint8_t CurrentSequence = m_nextSequence - m_bufferedFrequencies.size();

            float returnFrequency = 0.0f;

            // If we got stuff to get from, should only be a initialize problem
            if(CurrentSequence == m_realSequence)
            {
                if(m_bufferedFrequencies.size())
                {
                    returnFrequency = (*m_bufferedFrequencies.begin()).frequence;
                    m_bufferedFrequencies.pop_front();
                }
            }

            return returnFrequency;
        }

        void FrequencyBufferHandler::BufferFrequency(float p_frequency)
        {
            m_bufferedFrequencies.push_back(DebufFrequence(p_frequency, m_nextSequence));
        }

        void FrequencyBufferHandler::UpdateBufferFromSequence(uint8_t p_sequence)
        {
            uint32_t numOfRemoves = sequence_difference(p_sequence, m_nextSequence, 255);

            // Check if we can remove (error check), then remove the number of accs in difference
            if(numOfRemoves <= m_bufferedFrequencies.size())
            {
                // TODOCM check if you can remove this in one call
                for(size_t i = 0; i < numOfRemoves; i++)
                {
                    m_bufferedFrequencies.pop_front();
                    m_nextSequence++;
                }
            }
        }

        void FrequencyBufferHandler::ReadNewFrequencies(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesRead)
        {
            // Read how many frequencies we got buffered
            uint8_t NumOfSequences = p_streamer.ReadUnsignedInt8();
            uint8_t NumOfSequencesLeft = NumOfSequences;

            // Read the starting sequence
            uint8_t FirstSequence = p_streamer.ReadUnsignedInt8();

            // TODOCM remove...
            if(first)
            {
                first = false;
                SetStartingSequence(FirstSequence);
            }

            // Add to offset
            op_BytesRead += sizeof(uint8_t) * 2;

            if(NumOfSequences == 0)
            {
                return;
            }

            // While we're lower or
            while(NumOfSequencesLeft && sequence_more_recent(m_nextSequence, FirstSequence, 255))
            {
                FirstSequence++;
                NumOfSequencesLeft--;
            }

            // Move read pointer forward
            op_BytesRead += sizeof(float) * (NumOfSequences - NumOfSequencesLeft);
            p_streamer.SetReadWritePosition(op_BytesRead);

            // Increase the last one we will use
            m_nextSequence += NumOfSequencesLeft;

            // For the remaining sequences
            while(NumOfSequencesLeft)
            {
                // Set read point
                // p_streamer.SetReadWritePosition(op_BytesRead);

                // Read frequency
                float frequency = p_streamer.ReadFloat();

                // Add frequency to buffer
                m_bufferedFrequencies.push_back(DebufFrequence(frequency, FirstSequence));
                FirstSequence++;

                // Add to offset
                op_BytesRead += sizeof(float);
                NumOfSequencesLeft--;
            }

            // Set read point for next user
            p_streamer.SetReadWritePosition(op_BytesRead);
        }

        void FrequencyBufferHandler::WriteFrequencies(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten)
        {
            // Check how many bytes we have left to write
            uint32_t bytesLeftToWrite = p_bufferSize - op_BytesWritten;

            // If we can't even start header
            if(bytesLeftToWrite < (sizeof(uint8_t) * 2))
            {
                return;
            }

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
            std::list<DebufFrequence>::iterator iter = m_bufferedFrequencies.begin();

            for(; iter != m_bufferedFrequencies.end(); ++iter)
            {
                uint32_t bytesToWrite = sizeof(float);

                // Check if there is space left to write
                if(bytesLeftToWrite < bytesToWrite)
                {
                    break;
                }

                // Remove from our bytes left to write
                bytesLeftToWrite -= bytesToWrite;

                // Write frequency
                p_streamer.WriteFloat((*iter).frequence);

                // Increment how many bytes we've written
                op_BytesWritten += bytesToWrite;

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

        void FrequencyBufferHandler::SetStartingSequence(uint8_t p_sequence)
        {
            m_realSequence = p_sequence - m_bufferDelay;
            m_nextSequence = p_sequence;
        }

        uint8_t FrequencyBufferHandler::GetNextSequenceUsed() { return m_nextSequence; }
    }
}