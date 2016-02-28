#pragma once
#include <string>

namespace DoremiEngine
{
    namespace Network
    {
        class Adress
        {
        public:
            /**
                Returns port
            */
            virtual uint16_t GetPort() const = 0;

            /**
                Returns IP big endian, only usable if created yourself
            */
            virtual uint32_t GetIP() const = 0;

            /**
                Returns IP part a
            */
            virtual uint8_t GetIP_A() const = 0;

            /**
                Returns IP part b
            */
            virtual uint8_t GetIP_B() const = 0;

            /**
                Returns IP part c
            */
            virtual uint8_t GetIP_C() const = 0;

            /**
                Returns IP part d
            */
            virtual uint8_t GetIP_D() const = 0;

            /**
                Set the IP in format "p_a.p_b.p_c.p_d" as in "255.255.255.255", use ComposeAdress to
                be able to use GetAdress after this
            */
            virtual void SetIP(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d) = 0;

            /**
                Set the port, use ComposeAdress to be able to use GetAdress after this
            */
            virtual void SetNetPort(uint16_t p_port) = 0;

            /**
                Compose the adress used for socket binding when the functions SetIP or/and
                SetNetPort was used
            */
            virtual void ComposeAdress() = 0;

            /**
                Returns the IP as a string
            */
            virtual std::string GetIPToString() const = 0;

            /**
                Check if adress have same IP and Port
            */
            virtual bool Adress::operator==(const Adress& p_adress) const = 0;

            /**
                Check if adress ONLY have same IP
            */
            virtual bool Adress::operator*=(const Adress& p_adress) const = 0;
        };
    }
}