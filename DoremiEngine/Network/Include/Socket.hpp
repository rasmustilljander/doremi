#pragma once

// Project specific
#include <AdressImplementation.hpp>

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
            explicit Socket(SOCKET p_socketHandle);

            /**
                TODOCM log
            */
            ~Socket();

            /**
                Creates a TCP socket used for incomming connections
            */
            void CreateWaitingTCPSocket(const AdressImplementation& p_myAdress, const uint8_t& p_maxConnections);

            /**
                Create a TCP socket and connect to an adress
            */
            bool CreateAndConnectTCPSocket(const AdressImplementation& p_connectAdress);

            /**
                Accept a TCP connection if CreateWaitingTCPSocket is called
            */
            bool AcceptTCPConnection(SOCKET& p_socketHandle, AdressImplementation& p_adress);

            /**
                Creates a UDP socket needed to use SendTo function first to function
            */
            void CreateUDPSocketToSendAndRecieve();

            /**
                Creates a UDP socket used for incomming incomming messages
            */
            void CreateAndBindUDPSocket(const AdressImplementation& p_myAdress);

            /**
                Send data using UDP
            */
            bool SendUDP(const AdressImplementation& p_Adress, void* p_data, const uint32_t& p_dataSize);

            /**
                Recieve data using UDP and fetch adress
            */
            bool RecieveUDP(AdressImplementation& p_Adress, void* p_data, const uint32_t& p_dataSize, uint32_t& p_dataSizeReceived);

            /**
                Recieve data using UDP to bound socket
            */
            bool RecieveUDP(void* p_data, const uint32_t& p_dataSize, uint32_t& p_dataSizeReceived);

            /**
                Send data to socket
            */
            bool SendTCP(void* p_data, const uint32_t& p_dataSize);

            /**
                Recieve data from socket
            */
            bool RecieveTCP(void* p_data, const uint32_t& p_dataSize, uint32_t& p_dataSizeReceived);

        private:
            /**
                Creates a TCP socket used for binding or connecting
            */
            void CreateTCPSocket();

            /**
                Creates a UDP socket used for binding or send
            */
            void CreateUDPSocket();

            /**
                Bind TCP socket to incomming connections
            */
            void BindSocket(const AdressImplementation& p_adress);

            /**
                Set Socket to not Block on Recieve calls
            */
            void SetNonBlocking();

            /**
                Connect socket to adress
            */
            bool ConnectSocket(const AdressImplementation& p_connectAdress);

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