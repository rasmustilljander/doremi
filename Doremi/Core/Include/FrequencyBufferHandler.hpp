#pragma once
#include <stdint.h>
#include <list>


namespace Doremi
{
    namespace Core
    {
        class NetworkStreamer;

        struct FrequenceItem
        {
            FrequenceItem(float p_frequence, uint8_t p_sequence) : Frequence(p_frequence), Sequence(p_sequence) {}

            float Frequence;
            uint8_t Sequence;
        };

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


            void Update();

            void QueueFrequency(float p_frequency, uint8_t p_sequence);

            float GetFrequencyForFrame();

            /**
                Initialize the sequence
            */
            void SetSequence(uint8_t p_sequence);

        private:
            /**
                Buffered frequencies, both for incomming and sending
            */
            std::list<FrequenceItem> m_bufferedFrequencies;

            /**
                The sequence we are using now
            */
            uint8_t m_realSequence;

            /**
                How many frequences we buffer
            */
            uint8_t m_bufferDelay;

            float m_frequence;

            uint8_t m_sequenceByLastFrequence;

            /**
                A hotfix to initialize sequencce
                TODOCM remove later
            */
            // bool m_first;
        };
    }
}
