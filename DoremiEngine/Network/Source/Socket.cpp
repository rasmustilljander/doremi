#include <DoremiEngine/Network/Include/Socket.hpp>
#include <iostream>

// TODOCM Fix GetError from failed calls, example: socket(AF_INET, ....)

namespace DoremiEngine
{
    namespace Network
    {


        Socket::Socket()
        {

        }

        Socket::~Socket()
        {

        }

        void Socket::CreateTCPSocket()
        {
            int Handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            if (Handle == INVALID_SOCKET)
            {
                throw std::runtime_error("Failed creating TCP socket.");
            }
        }

        void Socket::CreateWaitingTCPSocket(const Adress &p_myAdress, const uint8_t &p_maxConnections)
        {
            // Create Socket
            CreateTCPSocket();

            // Set adress and bind socket to port
            BindSocket(p_myAdress);

            // Set socket to listening with max number of connections to port
            listen(m_socketHandle, p_maxConnections);
        }

        void Socket::CreateAndConnectTCPSocket(const Adress &p_connectAdress)
        {
            // Create Socket
            CreateTCPSocket();

            // Connect to adress
            ConnectSocket(p_connectAdress);
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

        void Socket::BindSocket(const Adress &p_myAdress)
        {
            int Result = bind(m_socketHandle, (SOCKADDR*)&p_myAdress.GetAdress(), sizeof(SOCKADDR));

            if (Result == SOCKET_ERROR)
            {
                std::string Out = "Failed to bind socket with IP: " + p_myAdress.GetIPToString() + " To port: " + std::to_string(p_myAdress.GetPort());
                throw std::runtime_error(Out.c_str());
            }
        }

        void Socket::ConnectSocket(const Adress &p_connectAdress)
        {
            int Result = connect(m_socketHandle, (SOCKADDR*)&p_connectAdress.GetAdress(), sizeof(SOCKADDR));

            if (Result == SOCKET_ERROR)
            {
                std::string Out = "Failed to connect to socket with IP: " + p_connectAdress.GetIPToString() +
                    " To port: " + std::to_string(p_connectAdress.GetPort());
                throw std::runtime_error(Out.c_str());
            }
        }

    }
}