#pragma once
// Project specific
#include <Adress.hpp>

// Standard libraries

#include <WinSock2.h>

namespace DoremiEngine
{
    namespace Network
    {
        /**
            Adress struct for comosing and holding adress information
        */
        class AdressImplementation : public Adress
        {
            public:
            /**
                Constructs Adress
            */
            AdressImplementation();

            /**
                Construct Adress with any input IP, and set port
            */
            AdressImplementation(uint16_t p_port);

            /**
               Constructs Adress with IP in format "p_a.p_b.p_c.p_d" as in "255.255.255.255" with
                port
            */
            AdressImplementation(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port);

            /**
                TODOCM doc
            */
            ~AdressImplementation();

            /**
                TODOCM doct
            */
            uint16_t GetPort() const override
            {
                return m_port;
            };

            /**
                Returns IP
            */
            uint32_t GetIP() const override
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
            void SetIP(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d) override;

            /**
                Set the port, use ComposeAdress to be able to use GetAdress after this
            */
            void SetNetPort(uint16_t p_port) override;

            /**
                Set the actuall composed adress, warning may not use IP and PORT in a good way now
                TODOCM extract the port and IP if possible
                TODOCM move to firend class
            */
            void SetAdress(SOCKADDR_IN p_adress);

            /**
               Compose the adress used for socket binding when the functions SetIP or/and
                SetNetPort was used
            */
            void ComposeAdress() override;

            /**
                Returns the IP as a string
            */
            std::string GetIPToString() const override
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