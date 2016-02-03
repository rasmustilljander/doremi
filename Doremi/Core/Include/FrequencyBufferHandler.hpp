#pragma once
#include <stdint.h>
#include <list>


namespace Doremi
{
    namespace Core
    {
        class BitStreamer;

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
                Returns the frequency for the current frame, if none is buffered 0 is returned
            */
            float GetFrequencyForFrame();

            /**
                Buffer a frequency used for send
            */
            void BufferFrequency(float p_frequency);

            /**
                Update buffer queue from acced sequence
            */
            void UpdateBufferFromSequence(uint8_t p_sequence);

            /**
                Read incomming frequencies from a buffer
            */
            void ReadNewFrequencies(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesRead);

            /**
                Write outgoing frequencies to a buffer
            */
            void WriteFrequencies(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten);

            /**
                Initialize the sequence
            */
            void SetStartingSequence(uint8_t p_sequence);

            /**
                Returns the sequence used in next update
            */
            uint8_t GetNextSequenceUsed();

        private:
            /**
                Buffered frequencies, both for incomming and sending
            */
            std::list<float> m_bufferedFrequencies;

            /**
                The next one we will recieve
            */
            uint8_t m_nextSequence;

            /**
                The sequence we are using now
            */
            uint8_t m_realSequence;

            /**
                How many frequences we buffer
            */
            uint8_t m_bufferDelay;

            /**
                A hotfix to initialize sequencce
                TODOCM remove later
            */
            bool m_first;
        };
    }
}
