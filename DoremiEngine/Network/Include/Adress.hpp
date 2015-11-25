#pragma once
#include <string>
#include <WinSock2.h>

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
            Adress(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port);

            /**
                TODOCM doc
            */
            ~Adress();

            /**
                TODOCM doc
            */
            uint16_t GetPort() const { return m_port; };

            /**
                TODOCM doc
            */
            uint32_t GetIP() const { return m_IP; };

            /**
                TODOCM doc
            */
            SOCKADDR_IN GetAdress() const { return m_Adress; };

            /**
                TODOCM doc
            */
            void SetIP(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d);

            /**
                TODOCM doc
            */
            void SetNetPort(uint16_t p_port);

            /**
                TODOCM doc
            */
            void ComposeAdress();

            /**
                TODOCM doc
            */
            std::string GetIPToString() const { return m_IPString; };

            private:
            /**
                TODOCM doc
            */
            uint32_t m_IP;

            /**
                TODOCM doc
            */
            uint16_t m_port;

            /**
                TODOCM doc
            */
            SOCKADDR_IN m_Adress;

            /**
                TODOCM doc
            */
            std::string m_IPString;
        };
    }
}