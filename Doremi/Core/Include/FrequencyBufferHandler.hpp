#pragma once
#include <stdint.h>
#include <list>


namespace Doremi
{
    namespace Core
    {
        class BitStreamer;

        struct DebufFrequence
        {
            DebufFrequence(float p_frequence, uint8_t p_sequence) : frequence(p_frequence), sequence(p_sequence) {}
            float frequence;
            uint8_t sequence;
        };

        class FrequencyBufferHandler
        {
        public:
            /**
                TODOCM doc
            */
            FrequencyBufferHandler();

            /**
                TODOCM doc
            */
            ~FrequencyBufferHandler();

            /**
                TODOCM doc
            */
            float GetFrequencyForFrame();

            /**
                TODOCM doc
            */
            void BufferFrequency(float p_frequency);

            /**
                Update buffer queue from acced sequence
            */
            void UpdateBufferFromSequence(uint8_t p_sequence);

            /**
                TODOCM doc
            */
            void ReadNewFrequencies(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesRead);

            /**
                TODOCM doc
            */
            void WriteFrequencies(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten);

            /**
                TODOCM doc
            */
            void SetStartingSequence(uint8_t p_sequence);

            /**
                TODOCM doc
            */
            uint8_t GetNextSequenceUsed();

        private:
            /**
                TODOCM doc
            */
            std::list<DebufFrequence> m_bufferedFrequencies;

            /**
                The next one we will recieve
            */
            uint8_t m_nextSequence;

            /**
                TODOCM doc
            */
            uint8_t m_realSequence;

            /**
                TODOCM doc
            */
            uint8_t m_bufferDelay;

            float testfloat;
            bool first;
        };
    }
}
