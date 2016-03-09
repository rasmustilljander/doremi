#include <FrequencyBufferHandler.hpp>
#include <Doremi/Core/Include/Streamers/NetworkStreamer.hpp>
#include <SequenceMath.hpp>
#include <AudioHandler.hpp>
#include <algorithm>
#include <iostream>


namespace Doremi
{
    namespace Core
    {
        FrequencyBufferHandler::FrequencyBufferHandler() : m_bufferDelay(3) {}

        FrequencyBufferHandler::~FrequencyBufferHandler() {}

        void FrequencyBufferHandler::Update()
        {
            // Increment realsequence
            m_realSequence++;

            //// Holder for new input
            // uint32_t newMaskWithInput = m_maskWithInput;

            // If we have any inputs
            std::list<FrequenceItem>::reverse_iterator riter = m_bufferedFrequencies.rbegin();

            // For all
            for(; riter != m_bufferedFrequencies.rend(); ++riter)
            {
                // If we're more recent
                if(sequence_more_recent(riter->Sequence + 1, m_realSequence, 255))
                {
                    break;
                }
            }

            // Remove those we've skipped, shouldn't skip but ye
            m_bufferedFrequencies.erase(riter.base(), m_bufferedFrequencies.end());
            riter = m_bufferedFrequencies.rbegin();

            // If we have anything to update with left
            if(m_bufferedFrequencies.size())
            {
                // Check if realsequence is same as last queued
                if(riter->Sequence == m_realSequence)
                {
                    // Set frequence
                    m_frequence = riter->Frequence;

                    // Remove sequence
                    m_bufferedFrequencies.pop_back();

                    m_sequenceByLastFrequence = m_realSequence;
                }
            }
        }

        void FrequencyBufferHandler::QueueFrequency(float p_frequency, uint8_t p_sequence)
        {
            // Check if our sequence hasn't passed
            if(sequence_more_recent(p_sequence, m_realSequence, 255))
            {
                // Try to fill it
                std::list<FrequenceItem>::iterator iter = m_bufferedFrequencies.begin();

                for(; iter != m_bufferedFrequencies.end(); ++iter)
                {
                    // If we're more recent we put it here
                    if(sequence_more_recent(p_sequence, iter->Sequence, 255))
                    {
                        break;
                    }
                }

                // Insert it
                m_bufferedFrequencies.insert(iter, FrequenceItem(p_frequency, p_sequence));
            }
            // TODOCM remove debugg
            if(m_bufferedFrequencies.size() > 30)
            {
                std::cout << "Got more then 30 frequences buffered will clear, real num: " << m_bufferedFrequencies.size() << std::endl;
                m_bufferedFrequencies.clear();
            }
        }

        float FrequencyBufferHandler::GetFrequencyForFrame() { return m_frequence; }

        void FrequencyBufferHandler::SetSequence(uint8_t p_sequence) { m_realSequence = p_sequence - m_bufferDelay; }

        void FrequencyBufferHandler::Reset()
        {
            m_bufferedFrequencies.clear();
            m_realSequence = 0;
            m_frequence = 0;
            m_sequenceByLastFrequence = 0;
        }
    }
}