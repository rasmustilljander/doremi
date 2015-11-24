#include <DoremiEngine/Network/Include/Socket.hpp>
#include <iostream>



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

            if (Handle <= 0)
            {
                return;
            }
        }

        void Socket::CreateWaitingTCPSocket(const Adress &p_adress, const uint8_t &p_maxConnections)
        {
            CreateTCPSocket();

            // Set adress to socket
            SetAdress(p_adress);
            
            // Bind socket to be used
            //int Result = bind(m_socketHandle, (SOCKADDR*)&AddrListen, sizeof(SOCKADDR));
            
           
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

        void Socket::SetAdress(const Adress &p_adress)
        {
            
        }

    }
}