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
                Constructs Adress with local IP
            */
            Adress();

            /**
                Constructs Adress with IP in format "p_a.p_b.p_c.p_d" as in "255.255.255.255" with
               port
            */
            Adress(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port);

            /**
                TODOCM doc
            */
            ~Adress();

            /**
                Returns port
            */
            uint16_t GetPort() const
            {
                return m_port;
            };

            /**
                Returns IP
            */
            uint32_t GetIP() const
            {
                return m_IP;
            };

            /**
                Returns composed Adress used for socket binding
            */
            SOCKADDR_IN GetAdress() const
            {
                return m_Adress;
            };

            /**
                Set the IP in format "p_a.p_b.p_c.p_d" as in "255.255.255.255", use ComposeAdress to
               be able to use GetAdress after this
            */
            void SetIP(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d);

            /**
                Set the port, use ComposeAdress to be able to use GetAdress after this
            */
            void SetNetPort(uint16_t p_port);

            /**
                Set the actuall composed adress, warning may not use IP and PORT in a good way now
                TODOCM extract the port and IP if possible
            */
            void SetAdress(SOCKADDR_IN p_adress);

            /**
                Compose the adress used for socket binding when the functions SetIP or/and
               SetNetPort was used
            */
            void ComposeAdress();

            /**
                Returns the IP as a string
            */
            std::string GetIPToString() const
            {
                return m_IPString;
            };

            private:
            /**
                IP in form of Big Endian
            */
            uint32_t m_IP;

            /**
                Port for either in/out
            */
            uint16_t m_port;

            /**
                Composed adress used for socket binding
            */
            SOCKADDR_IN m_Adress;

            /**
                string of IP used for debug
            */
            std::string m_IPString;
        };
    }
}