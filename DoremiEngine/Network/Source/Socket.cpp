// Project specific
#include <DoremiEngine/Network/Include/Socket.hpp>

// Standard libraries
#include <iostream>

// TODOCM Fix GetError from failed calls, example: socket(AF_INET, ....)

namespace DoremiEngine
{
    namespace Network
    {
        Socket::Socket() : m_socketHandle(SOCKET_ERROR), m_messageSize(0) {}

        Socket::Socket(SOCKET p_socketHandle) : m_socketHandle(p_socketHandle), m_messageSize(0)
        {
            // TODOCM hotfix, change to check if UDP or TCP
            m_messageSize = INT_MAX;
        }

        Socket::~Socket() { closesocket(m_socketHandle); }

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
            if(Return == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed setting TCP to no delay.");
            }
        }

        void Socket::CreateWaitingTCPSocket(const AdressImplementation& p_myAdress, const uint8_t& p_maxConnections)
        {
            // Create Socket
            CreateTCPSocket();

            // Set adress and bind socket to port
            BindSocket(p_myAdress);

            // Set socket to nonblocking
            SetNonBlocking();

            // Set socket to listening with max number of connections to port
            listen(m_socketHandle, p_maxConnections);
        }

        bool Socket::CreateAndConnectTCPSocket(const AdressImplementation& p_connectAdress)
        {
            // Create Socket
            CreateTCPSocket();

            // Connect to adress
            return ConnectSocket(p_connectAdress);
        }

        bool Socket::AcceptTCPConnection(SOCKET& p_socketHandle, AdressImplementation& p_adress)
        {
            SOCKADDR_IN Adress = {0};

            int SizeOfAdress = sizeof(Adress);

            // Accept a incomming connection, returns a socket used to send to later
            SOCKET OutSocketHandle = accept(m_socketHandle, (SOCKADDR*)&Adress, &SizeOfAdress);

            // If failed, throw exception
            if(OutSocketHandle == INVALID_SOCKET)
            {
                if(true)
                {
                    return false; // TODOCM check the true error check
                }
                throw std::runtime_error("Failed to accept connection");
            }

            // TODOCM TEST TEST TEST, warning might cause undefined behaviour cause of nonblocking nature
            u_long Mode = 1;
            int32_t Return = ioctlsocket(OutSocketHandle, FIONBIO, &Mode);
            if(Return == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed setting TCP to non blocking.");
            }

            // Set out socket
            p_socketHandle = OutSocketHandle;

            // Set out adress
            p_adress.SetAdress(Adress);

            // Return a socket used to send to later
            return true;
        }

        void Socket::CreateUDPSocket()
        {
            m_socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

            // If failed throw exception
            if(m_socketHandle == INVALID_SOCKET)
            {
                throw std::runtime_error("Failed creating UDP socket.");
            }

            // Get the max size of a packet to send, TODOCM see if this is really needed
            uint32_t MaxMessageSize = 0;
            int ValueSize = sizeof(int);
            int32_t Return = getsockopt(m_socketHandle, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)&MaxMessageSize, &ValueSize);

            // If error
            if(Return == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed getting the Max Message Size of UDP socket");
            }

            m_messageSize = MaxMessageSize;
        }

        void Socket::CreateUDPSocketToSendAndRecieve()
        {
            // Create Socket
            CreateUDPSocket();

            // Set to not block
            SetNonBlocking();
        }


        void Socket::CreateAndBindUDPSocket(const AdressImplementation& p_myAdress)
        {
            // Create socket
            CreateUDPSocket();

            // Bind socket to recieve incomming
            BindSocket(p_myAdress);

            // Set socket to non blocking
            SetNonBlocking();
        }


        bool Socket::SendUDP(const AdressImplementation& p_adress, void* p_data, const uint32_t& p_dataSize)
        {
            // Check if message larger then max size of the connection
            if(p_dataSize > m_messageSize)
            {
                // TODOCM Fix better message
                throw std::runtime_error("Attempting to send too large message.");
            }

            int32_t Return = sendto(m_socketHandle, (char*)p_data, p_dataSize, 0, (SOCKADDR*)&p_adress.GetAdress(), sizeof(SOCKADDR));
            if(Return == SOCKET_ERROR)
            {
                // Error cause of Socket is buissy in non-blocking mode, non-fatal error
                if(WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    // TODOCM Fix better message
                    throw std::runtime_error("Failed to send UDP data.");
                }
                return false;
            }
            // If returned data is other size then requested, if this is a possibility we will need to fix this
            else if(Return != p_dataSize)
            {
                std::cout << "Requested sent data and sent data was not equal, confront Christian if this problem arises."
                          << std::endl; // TODOCM Solution to sent = req data sent
                return false;
            }

            return true;
        }

        bool Socket::RecieveUDP(AdressImplementation& p_Adress, void* p_data, const uint32_t& p_dataSize, uint32_t& p_dataSizeReceived)
        {
            SOCKADDR_IN Adress = {0};

            int AdressSize = sizeof(Adress);

            // Attempt to recieve data from socket
            int32_t Return = recvfrom(m_socketHandle, (char*)p_data, p_dataSize, 0, (SOCKADDR*)&Adress, &AdressSize);

            p_dataSizeReceived = Return;

            // If some error
            if(Return == SOCKET_ERROR)
            {
                // Error cause of Socket is buissy in non-blocking mode, non-fatal error
                int Error = WSAGetLastError();
                if(Error != WSAEWOULDBLOCK && Error != WSAECONNRESET)
                {
                    // TODOCM Fix better message
                    throw std::runtime_error("Failed to send UDP data.");
                }
                return false;
            }
            // If returned data is other size then requested, if this is a possibility we will need to fix this
            else if(Return != p_dataSize)
            {
                std::cout << "Requested sent data and sent data was not equal, confront Christian if this problem arises."
                          << std::endl; // TODOCM Solution to sent = req data sent
                return false;
            }

            p_Adress.SetAdress(Adress);

            return true;
        }

        bool Socket::RecieveUDP(void* p_data, const uint32_t& p_dataSize, uint32_t& p_dataSizeReceived)
        {
            // Attempt to recieve data from socket
            int32_t Return = recvfrom(m_socketHandle, (char*)p_data, p_dataSize, 0, nullptr, nullptr);

            p_dataSizeReceived = Return;

            // If some error or
            if(Return == SOCKET_ERROR)
            {
                // Error cause of Socket is buissy in non-blocking mode, non-fatal error
                int Error = WSAGetLastError();
                if(Error != WSAEWOULDBLOCK && Error != WSAECONNRESET)
                {
                    // TODOCM Fix better message
                    throw std::runtime_error("Failed to recieve UDP data.");
                }
                return false;
            }
            // If returned data is other size then requested, if this is a possibility we will need to fix this
            else if(Return != p_dataSize)
            {
                std::cout << "Requested recieved data and recieved data was not equal, confront Christian if this problem arises."
                          << std::endl; // TODOCM Solution to sent = req data sent
                return false;
            }

            return true;
        }

        bool Socket::SendTCP(void* p_data, const uint32_t& p_dataSize)
        {
            // Check if message larger then max size of the connection(TCP doesnt care)
            if(p_dataSize > m_messageSize)
            {
                // TODOCM Fix better message
                throw std::runtime_error("Attempting to send too large message.");
            }

            // Attempt to send data to socket,
            int32_t Return = send(m_socketHandle, (char*)p_data, p_dataSize, 0);

            // If some error
            if(Return == SOCKET_ERROR)
            {
                // Error cause of Socket is buissy in non-blocking mode, non-fatal error
                int errorCode = WSAGetLastError();
                if(errorCode != WSAEWOULDBLOCK && errorCode != WSAECONNABORTED)
                {
                    // TODOCM Fix better message
                    // throw std::runtime_error("Failed to send TCP data.");
                    std::cout << "Failed to send TCP data." << std::endl;
                    return false;
                }
                return false;
            }
            // If returned data is other size then requested, if this is a possibility we will need to fix this
            else if(Return != p_dataSize)
            {
                std::cout << "Requested sent data and sent data was not equal, confront Christian if this problem arises."
                          << std::endl; // TODOCM Solution to sent = req data sent
                return false;
            }

            return true;
        }

        bool Socket::RecieveTCP(void* p_data, const uint32_t& p_dataSize, uint32_t& p_dataSizeReceived)
        {
            // Attempt to recieve data from socket
            int32_t Return = recv(m_socketHandle, (char*)p_data, p_dataSize, 0);

            p_dataSizeReceived = Return;

            // If some error or
            if(Return == SOCKET_ERROR)
            {
                // check towards errors
                int errorCode = WSAGetLastError();
                if(errorCode != WSAEWOULDBLOCK && errorCode != WSAECONNABORTED)
                {
                    // TODOCM Fix better message
                    // throw std::runtime_error("Failed to recieve TCP data.");
                    std::cout << "Failed to recieve TCP data." << std::endl;
                    return false;
                }

                return false;
            }
            // If returned data is other size then requested, if this is a possibility we will need to fix this
            else if(Return != p_dataSize)
            {
                std::cout << "Requested recieved data and recieved data was not equal, confront Christian if this problem arises."
                          << std::endl; // TODOCM Solution to sent = req data sent
                return false;
            }

            return true;
        }

        void Socket::BindSocket(const AdressImplementation& p_myAdress)
        {
            // Bind socket to incomming connection to a specific port and "allowed" IP
            int32_t Result = bind(m_socketHandle, (SOCKADDR*)&p_myAdress.GetAdress(), sizeof(SOCKADDR_IN));

            // If failed, throw exception
            if(Result == SOCKET_ERROR)
            {
                std::string Out = "Failed to bind socket with IP: " + p_myAdress.GetIPToString() + " To port: " + std::to_string(p_myAdress.GetPort());
                throw std::runtime_error(Out.c_str());
            }
        }

        void Socket::SetNonBlocking()
        {
            u_long Mode = 1;
            int Result = ioctlsocket(m_socketHandle, FIONBIO, &Mode);
            if(Result == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed setting TCP/UDP to non blocking.");
            }
        }

        bool Socket::ConnectSocket(const AdressImplementation& p_connectAdress)
        {
            // Attempt to connect to another socket with the IP and port specified
            int32_t Result = connect(m_socketHandle, (SOCKADDR*)&p_connectAdress.GetAdress(), sizeof(SOCKADDR));

            // If failed, throw exception
            if(Result == SOCKET_ERROR)
            {
                DWORD errorCode = WSAGetLastError();
                return false;
            }

            // TODOCM TEST TEST TEST, warning might cause undefined behaviour cause of nonblocking nature
            u_long Mode = 1;
            Result = ioctlsocket(m_socketHandle, FIONBIO, &Mode);
            if(Result == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed setting TCP to non blocking.");
            }
            return true;
        }
    }
}