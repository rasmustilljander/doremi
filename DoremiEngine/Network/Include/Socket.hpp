#pragma once
#include <string>
#include <Adress.hpp>

#ifdef WIN32
#include <WinSock2.h>
#elif
#error Platform not supported
#endif

namespace DoremiEngine
{
    namespace Network
    {
        class Socket
        {
            public:
            /**
                TODOCM log
            */
            Socket();

            /**
                TODOCM log
            */
            ~Socket();

            /**
                TODOCM log
            */
            void CreateWaitingTCPSocket(const Adress &p_adress, const uint8_t &p_maxConnections);

            /**
                TODOCM log

            */
            void CreateConnectingTCPSocket();

            /**
                TODOCM log
            */
            void CreateUDPSocket();

            /**
                TODOCM log
            */
            void Send();

            /**
                TODOCM log
            */
            void Recieve();

            private:
            /**
                TODOCM log
            */
            void CreateTCPSocket();

            /**
                TODOCM log
            */
            void SetAdress(const Adress &p_adress);

            /**
                TODOCM log
            */
            SOCKET m_socketHandle;
        };
    }
}