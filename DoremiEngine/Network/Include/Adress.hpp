#pragma once
#include <cstdint>

namespace DoremiEngine
{
    namespace Network
    {
        class Adress
        {
            public:
            /**
                TODOCM doc
            */
            Adress();

            /**
                TODOCM doc
            */
            ~Adress();

            private:
            /**
                TODOCM doc
            */
            uint32_t m_IP;

            /**
                TODOCM doc
            */
            uint16_t m_port;
        };
    }
}