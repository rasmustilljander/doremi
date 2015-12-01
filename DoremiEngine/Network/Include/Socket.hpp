#pragma once

// Project specific
#include <Adress.hpp>

// Standard libraries
#include <string>

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
            Socket(SOCKET p_socketHandle);

            /**
                TODOCM log
            */
            ~Socket();

            /**
                Creates a TCP socket used for incomming connections
            */
            void CreateWaitingTCPSocket(const Adress& p_myAdress, const uint8_t& p_maxConnections);

            /**
                Create a TCP socket and connect to an adress
            */
            void CreateAndConnectTCPSocket(const Adress& p_connectAdress);

            /**
                Accept a TCP connection if CreateWaitingTCPSocket is called
            */
            Socket AcceptTCPConnection(Adress& p_inAdress);

            /**
                TODOCM log
            */
            void CreateUDPSocket();

            /**
                Send data to socket
            */
            bool Send(void* t_data, const uint32_t &t_dataSize);

            /**
                Recieve data from socket
            */
            bool Recieve(void* t_data, const uint32_t &t_dataSize);

            private:
            /**
                Creates a TCP socket used for binding or connecting
            */
            void CreateTCPSocket();

            /**
                Bind TCP socket to incomming connections
            */
            void BindSocket(const Adress& p_adress);

            /**
                Connect socket to adress
            */
            void ConnectSocket(const Adress& p_connectAdress);

            /**
                Socket handle used for API calls
            */
            SOCKET m_socketHandle;

            /**
                Max Size of package, TCP shoudln't care, TODOCM check if true
            */
            uint32_t m_messageSize;
        };
    }
}