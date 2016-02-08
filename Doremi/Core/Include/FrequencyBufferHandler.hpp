#pragma once
#include <stdint.h>
#include <list>


namespace Doremi
{
    namespace Core
    {
        class NetworkStreamer;

        /**
            Used to buffer frequency for send, and recieve
        */
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

            void Update();

            /**
                Read incomming frequencies from a buffer
            */
            void ReadNewFrequencies(NetworkStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesRead);

            /**
                Write outgoing frequencies to a buffer
            */
            void WriteFrequencies(NetworkStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten);

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
