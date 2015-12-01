// Project specific
#include <DoremiEngine/Network/Include/Socket.hpp>

// Standard libraries
#include <iostream>

// TODOCM Fix GetError from failed calls, example: socket(AF_INET, ....)

namespace DoremiEngine
{
    namespace Network
    {
        Socket::Socket()
        {
        }

        Socket::Socket(SOCKET p_socketHandle) : m_socketHandle(p_socketHandle)
        {
        }

        Socket::~Socket()
        {
        }

        void Socket::CreateTCPSocket()
        {
            m_socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            // If failed throw exception
            if(m_socketHandle == INVALID_SOCKET)
            {
                throw std::runtime_error("Failed creating TCP socket.");
            }
        }

        void Socket::CreateWaitingTCPSocket(const Adress& p_myAdress, const uint8_t& p_maxConnections)
        {
            // Create Socket
            CreateTCPSocket();

            // Set adress and bind socket to port
            BindSocket(p_myAdress);

            // Set socket to listening with max number of connections to port
            listen(m_socketHandle, p_maxConnections);
        }

        void Socket::CreateAndConnectTCPSocket(const Adress& p_connectAdress)
        {
            // Create Socket
            CreateTCPSocket();

            // Connect to adress
            ConnectSocket(p_connectAdress);
        }

        Socket Socket::AcceptTCPConnection(Adress& p_inAdress)
        {
            SOCKADDR_IN Adress = { 0 };

            // Accept a incomming connection, returns a socket used to send to later
            SOCKET OutSocketHandle = accept(m_socketHandle, (SOCKADDR*)&Adress, nullptr);

            // Set the incomming connection adress to a variable for later use
            p_inAdress.SetAdress(Adress);

            // If failed, throw exception
            if(OutSocketHandle == INVALID_SOCKET)
            {
                std::string Out = "Failed to accept connection with IP: " + p_inAdress.GetIPToString() +
                                  " To port: " + std::to_string(p_inAdress.GetPort());
                throw std::runtime_error(Out.c_str());
            }

            // Return a socket used to send to later
            return Socket(OutSocketHandle);
        }

        void Socket::CreateUDPSocket()
        {
        }

        void Socket::Send()
        {
        }

        void Socket::Recieve()
        {
        }

        void Socket::BindSocket(const Adress& p_myAdress)
        {
            // Bind socket to incomming connection to a specific port and "allowed" IP
            int Result = bind(m_socketHandle, (SOCKADDR*)&p_myAdress.GetAdress(), sizeof(SOCKADDR));

            // If failed, throw exception
            if(Result == SOCKET_ERROR)
            {
                std::string Out = "Failed to bind socket with IP: " + p_myAdress.GetIPToString() +
                                  " To port: " + std::to_string(p_myAdress.GetPort());
                throw std::runtime_error(Out.c_str());
            }
        }

        void Socket::ConnectSocket(const Adress& p_connectAdress)
        {
            // Attempt to connect to another socket with the IP and port specified
            int Result = connect(m_socketHandle, (SOCKADDR*)&p_connectAdress.GetAdress(), sizeof(SOCKADDR));

            // If failed, throw exception
            if(Result == SOCKET_ERROR)
            {
                DWORD errorCode = WSAGetLastError();
                std::string Out = "Failed to connect to socket with IP: " + p_connectAdress.GetIPToString() +
                                  " To port: " + std::to_string(p_connectAdress.GetPort());
                throw std::runtime_error(Out.c_str());
            }
        }
    }
}