// Project specific
#include <DoremiEngine/Network/Include/Socket.hpp>

// Standard libraries
#include <iostream>

// TODOCM Fix GetError from failed calls, example: socket(AF_INET, ....)

namespace DoremiEngine
{
    namespace Network
    {
        Socket::Socket() : m_socketHandle(SOCKET_ERROR), m_messageSize(0)
        {
        }

        Socket::Socket(SOCKET p_socketHandle) : m_socketHandle(p_socketHandle), m_messageSize(0)
        {
            // TODOCM hotfix, change to check if UDP or TCP
            m_messageSize = INT_MAX;
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

            // TODOCM see if this could be solved in any other way
            // Set TCP Socket message size to MAX
            m_messageSize = INT_MAX;

            // Set TCP send pattern to "No delay", we dont want to wait for messages to pile up befor sending
            int NoDelay = 1;
            int32_t Return = setsockopt(m_socketHandle, IPPROTO_TCP, TCP_NODELAY, (char*)&NoDelay, sizeof(NoDelay));
            if (Return == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed setting TCP to no delay.");
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

            // TODOCM TEST TEST TEST, warning might cause undefined behaviour cause of nonblocking nature
            u_long Mode = 1;
            int32_t Return = ioctlsocket(OutSocketHandle, FIONBIO, &Mode);
            if (Return == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed setting TCP to non blocking.");
            }

            // Return a socket used to send to later
            return Socket(OutSocketHandle);
        }

        void Socket::CreateUDPSocket()
        {
        }

        bool Socket::Send(void* t_data, const uint32_t &t_dataSize)
        {
            // Check if message larger then max size of the connection(TCP doesnt care)
            if (t_dataSize > m_messageSize)
            {
                // TODOCM Fix better message
                throw std::runtime_error("Attempting to send too large message.");
            }

            // Attempt to send data to socket,
            int32_t Return = send(m_socketHandle, (char*)t_data, t_dataSize, 0);

            // If some error
            if (Return == SOCKET_ERROR)
            {
                // Error cause of Socket is buissy in non-blocking mode, non-fatal error
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    // TODOCM Fix better message
                    throw std::runtime_error("Failed to send data.");
                    
                    return false;
                }
            }
            // If returned data is other size then requested, if this is a possibility we will need to fix this
            else if (Return != t_dataSize)
            {
                throw std::runtime_error("Requested sent data and sent data was not equal, confront Christian if this problem arises."); // TODOCM Solution to sent = req data sent
            }

            return true;
        }

        bool Socket::Recieve(void* t_data, const uint32_t &t_dataSize)
        {
            // Check if message larger then max size of the connection(TCP doesnt care)
            if (t_dataSize > m_messageSize)
            {
                // TODOCM Fix better message
                throw std::runtime_error("Attempting to recieve from a too large message.");
            }

            // Attempt to recieve data from socket
            int32_t Return = recv(m_socketHandle, (char*)t_data, t_dataSize, 0);

            // If some error or
            if (Return == SOCKET_ERROR )
            {
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    // TODOCM Fix better message
                    throw std::runtime_error("Failed to send data.");

                    return false;
                }
            }
            // If returned data is other size then requested, if this is a possibility we will need to fix this
            else if (Return != t_dataSize)
            {
                throw std::runtime_error("Requested sent data and sent data was not equal, confront Christian if this problem arises."); // TODOCM Solutio to recv = req data recv
            }

            return true;
        }

        void Socket::BindSocket(const Adress& p_myAdress)
        {
            // Bind socket to incomming connection to a specific port and "allowed" IP
            int32_t Result = bind(m_socketHandle, (SOCKADDR*)&p_myAdress.GetAdress(), sizeof(SOCKADDR));

            // If failed, throw exception
            if(Result == SOCKET_ERROR)
            {
                std::string Out = "Failed to bind socket with IP: " + p_myAdress.GetIPToString() +
                                  " To port: " + std::to_string(p_myAdress.GetPort());
                throw std::runtime_error(Out.c_str());
            }

            // TODOCM TEST TEST TEST, warning might cause undefined behaviour cause of nonblocking nature
            u_long Mode = 1;
            Result = ioctlsocket(m_socketHandle, FIONBIO, &Mode);
            if (Result == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed setting TCP to non blocking.");
            }
        }

        void Socket::ConnectSocket(const Adress& p_connectAdress)
        {
            // Attempt to connect to another socket with the IP and port specified
            int32_t Result = connect(m_socketHandle, (SOCKADDR*)&p_connectAdress.GetAdress(), sizeof(SOCKADDR));

            // If failed, throw exception
            if(Result == SOCKET_ERROR)
            {
                DWORD errorCode = WSAGetLastError();
                std::string Out = "Failed to connect to socket with IP: " + p_connectAdress.GetIPToString() +
                                  " To port: " + std::to_string(p_connectAdress.GetPort());
                throw std::runtime_error(Out.c_str());
            }

            // TODOCM TEST TEST TEST, warning might cause undefined behaviour cause of nonblocking nature
            u_long Mode = 1;
            Result = ioctlsocket(m_socketHandle, FIONBIO, &Mode);
            if (Result == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed setting TCP to non blocking.");
            }
        }
    }
}