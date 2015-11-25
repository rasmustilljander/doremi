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

        void Socket::CreateWaitingTCPSocket(const Adress &p_adress, const uint8_t &p_maxConnections)
        {
            // Create Socket
            CreateTCPSocket();

            // Set adress and bind socket to port
            BindSocket(p_adress);

            // Set socket to listening with max number of connections to port
            listen(m_socketHandle, p_maxConnections);
        }

        void Socket::CreateConnectingTCPSocket()
        {
            CreateTCPSocket();
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

        void Socket::BindSocket(const Adress &p_adress)
        {
            // Bind socket to be used
            int Result = bind(m_socketHandle, (SOCKADDR*)&p_adress.GetAdress(), sizeof(SOCKADDR));
            if (Result == SOCKET_ERROR)
            {
                std::string Out = "Failed to bind socket with IP: " + p_adress.GetIPToString() + " To port: " + std::to_string(p_adress.GetPort());
                throw std::runtime_error(Out.c_str());
            }
        }

    }
}