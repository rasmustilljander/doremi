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
            return ConnectTCPSocket(p_connectAdress);
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

        bool Socket::CreateAndConnectUDPSocket(const AdressImplementation& p_connectAdress)
        {
            // Create Socket
            CreateUDPSocket();

            // Connect to adress
            return ConnectUDPSocket(p_connectAdress);
        }

        bool Socket::AcceptUDPConnection(Socket*& p_socket, AdressImplementation& p_adress)
        {
            SOCKADDR_IN Adress = {0};

            int SizeOfAdress = sizeof(Adress);

            uint32_t t_message;
            uint32_t t_dataSizeReceived = 0;

            // Try recieve
            if(!ReceiveUDP(p_adress, &t_message, sizeof(uint32_t), t_dataSizeReceived))
            {
                return false;
            }

            if(t_dataSizeReceived != sizeof(uint32_t))
            {
                return false;
            }

            // Only create the socket if we receive something
            p_socket = new Socket();
            p_socket->CreateUDPSocketToSendAndReceive();

            int32_t Result = connect(p_socket->m_socketHandle, (SOCKADDR*)&(p_adress.GetAdress()), sizeof(SOCKADDR));
            if(Result == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed setting TCP to non blocking.");
                return false;
            }

            // Send response with new socket
            p_socket->SendUDP(&t_message, sizeof(uint32_t));


            // Return a socket used to send to later
            return true;
        }

        void Socket::CreateUDPSocketToSendAndReceive()
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

            // Bind socket to Receive incomming
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
                int Error = WSAGetLastError();
                if(Error != WSAEWOULDBLOCK)
                {
                    // TODOCM log message
                }
                return false;
            }

            return true;
        }

        bool Socket::SendUDP(void* p_data, const uint32_t& p_dataSize)
        {
            // Check if message larger then max size of the connection
            if(p_dataSize > m_messageSize)
            {
                // TODOCM Fix better message
                throw std::runtime_error("Attempting to send too large message.");
            }

            int32_t Return = send(m_socketHandle, (char*)p_data, p_dataSize, 0);
            if(Return == SOCKET_ERROR)
            {
                // Error cause of Socket is buissy in non-blocking mode, non-fatal error
                int Error = WSAGetLastError();
                if(Error != WSAEWOULDBLOCK)
                {
                    // TODOCM log message
                }
                return false;
            }

            return true;
        }

        bool Socket::ReceiveUDP(AdressImplementation& p_Adress, void* p_data, const uint32_t& p_dataSize, uint32_t& p_dataSizeReceived)
        {
            SOCKADDR_IN Adress = {0};

            int AdressSize = sizeof(Adress);

            // Attempt to Receive data from socket
            int32_t Return = recvfrom(m_socketHandle, (char*)p_data, p_dataSize, 0, (SOCKADDR*)&Adress, &AdressSize);

            p_dataSizeReceived = Return;

            // If some error
            if(Return == SOCKET_ERROR)
            {
                // Error cause of Socket is buissy in non-blocking mode, non-fatal error
                int Error = WSAGetLastError();
                if(Error != WSAEWOULDBLOCK && Error != WSAECONNRESET)
                {
                    // TODOCM log message
                }
                return false;
            }

            p_Adress.SetAdress(Adress);

            return true;
        }

        bool Socket::ReceiveUDP(void* p_data, const uint32_t& p_dataSize, uint32_t& p_dataSizeReceived)
        {
            // Attempt to Receive data from socket
            int32_t Return = recvfrom(m_socketHandle, (char*)p_data, p_dataSize, 0, nullptr, nullptr);

            p_dataSizeReceived = Return;

            // If some error or
            if(Return == SOCKET_ERROR)
            {
                // Error cause of Socket is buissy in non-blocking mode, non-fatal error
                int Error = WSAGetLastError();
                if(Error != WSAEWOULDBLOCK && Error != WSAECONNRESET)
                {
                    // TODOCM Log message
                }
                return false;
            }

            return true;
        }

        bool Socket::ReceiveUDPConnected(void* p_data, const uint32_t& p_dataSize, uint32_t& p_dataSizeReceived)
        {
            // Attempt to Receive data from socket
            int32_t Return = recv(m_socketHandle, (char*)p_data, p_dataSize, 0);

            p_dataSizeReceived = Return;

            // If some error or
            if(Return == SOCKET_ERROR)
            {
                // Error cause of Socket is buissy in non-blocking mode, non-fatal error
                int Error = WSAGetLastError();
                if(Error != WSAEWOULDBLOCK && Error != WSAECONNRESET)
                {
                    // TODOCM Log message
                }
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
                    // TODOCM Log message
                }
                return false;
            }

            return true;
        }

        bool Socket::ReceiveTCP(void* p_data, const uint32_t& p_dataSize, uint32_t& p_dataSizeReceived)
        {
            // Attempt to Receive data from socket
            int32_t Return = recv(m_socketHandle, (char*)p_data, p_dataSize, 0);

            p_dataSizeReceived = Return;

            // If some error or
            if(Return == SOCKET_ERROR)
            {
                // check towards errors
                int errorCode = WSAGetLastError();
                if(errorCode != WSAEWOULDBLOCK && errorCode != WSAECONNABORTED)
                {
                    // TODOCM Log message
                }

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
                int Error = WSAGetLastError();
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

        bool Socket::ConnectTCPSocket(const AdressImplementation& p_connectAdress)
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

        bool Socket::ConnectUDPSocket(const AdressImplementation& p_connectAdress)
        {
            // While we succeed sending a message
            uint32_t p_Message = UDP_RELIABLE_CONTROL_ID;

            // Do this until send.. may cause deadlock
            uint32_t t_numTimes = 0;
            while(!SendUDP(p_connectAdress, &p_Message, sizeof(uint32_t)))
            {
                // If we fail sending a few times.. we give up
                t_numTimes++;
                if(t_numTimes > 100)
                {
                    return false;
                }
            }
            std::cout << t_numTimes << std::endl;

            // Set our timeout
            timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 100; // 1000 ms

            uint32_t Result = setsockopt(m_socketHandle, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
            if(Result == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed setting TCP to non blocking.");
            }

            uint32_t t_dataSizeReceived = 0;
            AdressImplementation* t_newAdress = new AdressImplementation();

            // Wait for receive
            if(ReceiveUDP(*t_newAdress, &p_Message, sizeof(uint32_t), t_dataSizeReceived))
            {
                if(t_dataSizeReceived == sizeof(uint32_t))
                {
                    if(p_Message == UDP_RELIABLE_CONTROL_ID)
                    {
                        int32_t Result = connect(m_socketHandle, (SOCKADDR*)&(t_newAdress->GetAdress()), sizeof(SOCKADDR));
                        if(Result == SOCKET_ERROR)
                        {
                            throw std::runtime_error("Failed setting TCP to non blocking.");
                        }

                        delete t_newAdress;

                        SetNonBlocking();

                        return true;
                    }
                }
            }

            std::cout << "Failed connect UDP" << std::endl;

            delete t_newAdress;

            return false;
        }
    }
}