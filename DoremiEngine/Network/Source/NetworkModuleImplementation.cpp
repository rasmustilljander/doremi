// Project specific
#include <NetworkModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>


#if PLATFORM == PLATFORM_WINDOWS
#include <WinSock2.h>
#elif PLATFORM == PLATFORM_UNIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif

//#define USE_TCP 1
#define USE_UDP 1

#include <iostream>

namespace DoremiEngine
{
    namespace Network
    {
        NetworkModuleImplementation::NetworkModuleImplementation() : m_isInitialized(false) {}

        NetworkModuleImplementation::~NetworkModuleImplementation() { Shutdown(); }

        void NetworkModuleImplementation::Startup()
        {
            if(m_isInitialized)
            {
                throw std::runtime_error("Failed to Startup Network Module, function already called.");
            }

#if PLATFORM == PLATFORM_WINDOWS
            WSADATA wsaData;

            // Initialize Winsock
            int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if(Result != NO_ERROR)
            {
                throw std::runtime_error("Failed to load Winsock, WSAStartup failed.");
            }
#elif
// nothing on unix
#endif
            m_isInitialized = true;
        }

        Adress* NetworkModuleImplementation::CreateAdress()
        {
            Adress* newAdress = new AdressImplementation();

            return newAdress;
        }

        Adress* NetworkModuleImplementation::CreateAdress(const Adress& m_adress)
        {
            Adress* newAdress = new AdressImplementation(*(AdressImplementation*)&m_adress);

            return newAdress;
        }

        Adress* NetworkModuleImplementation::CreateAdress(uint16_t p_port)
        {
            Adress* newAdress = new AdressImplementation(p_port);

            return newAdress;
        }

        Adress* NetworkModuleImplementation::CreateAdress(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port)
        {
            Adress* newAdress = new AdressImplementation(p_a, p_b, p_c, p_d, p_port);

            return newAdress;
        }

        Socket* NetworkModuleImplementation::GetSocketFromMap(size_t p_handle)
        {
            // Attempt to find socket from map
            std::map<size_t, Socket*>::iterator iter = m_socketHandleMap.find(p_handle);

            // If none is found, error
            if(iter == m_socketHandleMap.end())
            {
                return nullptr;
            }

            return iter->second;
        }

        bool NetworkModuleImplementation::SendReliableData(void* t_data, const uint32_t& t_dataSize, const size_t& p_sendToSocket)
        {
            // Get socket from map
            Socket* socketToSendTo = GetSocketFromMap(p_sendToSocket);

            if(socketToSendTo == nullptr)
            {
                return false;
            }

#ifdef USE_TCP
            // Attempt to send data, returns true if all data is sent
            bool SendSuccessful = socketToSendTo->SendTCP(t_data, t_dataSize);
#elif USE_UDP
            // Attempt to send data, returns true if all data is sent
            bool SendSuccessful = socketToSendTo->SendUDP(t_data, t_dataSize);
#endif

            return SendSuccessful;
        }

        bool NetworkModuleImplementation::ReceiveReliableData(void* t_data, const uint32_t& t_dataSize, const size_t& p_ReceiveFromSocket, uint32_t& p_dataSizeReceived)
        {
            // Get socket from map
            Socket* socketToReceiveFrom = GetSocketFromMap(p_ReceiveFromSocket);

            if(socketToReceiveFrom == nullptr)
            {
                return false;
            }

#ifdef USE_TCP
            // Attempt to Receive data, returns true if all data is sent
            bool ReceiveSuccessful = socketToReceiveFrom->ReceiveTCP(t_data, t_dataSize, p_dataSizeReceived);
#elif USE_UDP
            bool ReceiveSuccessful = socketToReceiveFrom->ReceiveUDPConnected(t_data, t_dataSize, p_dataSizeReceived);
#endif

            return ReceiveSuccessful;
        }

        bool NetworkModuleImplementation::SendUnreliableData(void* p_data, const uint32_t& p_dataSize, const size_t& p_sendToSocketHandle, const Adress* p_adressToSendTo)
        {
            // Get socket from map
            Socket* socketToSendTo = GetSocketFromMap(p_sendToSocketHandle);

            if(socketToSendTo == nullptr)
            {
                return false;
            }

            // Send Message ( will bind socket implicit)
            bool SendSuccessful = socketToSendTo->SendUDP(*(AdressImplementation*)p_adressToSendTo, p_data, p_dataSize);

            // return SendSuccessful;
            return SendSuccessful;
        }

        bool NetworkModuleImplementation::ReceiveUnreliableData(void* p_data, const uint32_t& p_dataSize, const size_t& p_ReceiveFromSocketHandle,
                                                                Adress* p_AdressOut, uint32_t& p_dataSizeReceived)
        {
            // Get socket from map
            Socket* socketToReceiveFrom = GetSocketFromMap(p_ReceiveFromSocketHandle);

            if(socketToReceiveFrom == nullptr)
            {
                return false;
            }

            // Receive data and fetch adress Received from
            bool ReceiveSuccessful = socketToReceiveFrom->ReceiveUDP(*(AdressImplementation*)p_AdressOut, p_data, p_dataSize, p_dataSizeReceived);

            return ReceiveSuccessful;
        }

        bool NetworkModuleImplementation::ReceiveUnreliableData(void* p_data, const uint32_t& p_dataSize, const size_t& p_ReceiveFromSocketHandle,
                                                                uint32_t& p_dataSizeReceived)
        {
            // Get socket from map
            Socket* socketToReceiveFrom = GetSocketFromMap(p_ReceiveFromSocketHandle);

            if(socketToReceiveFrom == nullptr)
            {
                return false;
            }

            // Receive data from a bound socket
            bool ReceiveSuccessful = socketToReceiveFrom->ReceiveUDP(p_data, p_dataSize, p_dataSizeReceived);

            return ReceiveSuccessful;
        }

        bool NetworkModuleImplementation::ConnectToReliable(const Adress* p_adressToConnectTo, size_t& o_socketHandle)
        {
            // TODOCM add try catch here, remove test code
            Socket* newSocket = new Socket();

#ifdef USE_TCP
            // Create a socket and attempt connect it to a socket used for incomming
            bool r_connected = newSocket->CreateAndConnectTCPSocket(*(AdressImplementation*)p_adressToConnectTo);
#elif USE_UDP
            bool r_connected = newSocket->CreateAndConnectUDPSocket(*(AdressImplementation*)p_adressToConnectTo);
#endif

            // Save socket to map
            std::hash<Socket*> HashMap;
            size_t key = HashMap(newSocket);
            m_socketHandleMap[key] = newSocket;

            // Set out value
            o_socketHandle = key;

            return r_connected;
        }

        size_t NetworkModuleImplementation::CreateReliableConnection(const Adress* p_adressToConnectTo, uint8_t p_maxWaitingConnections)
        {
            // TODOCM add try catch here, remove test code
            Socket* newSocket = new Socket();

#ifdef USE_TCP
            // Create a TCP socket used for incomming connections
            newSocket->CreateWaitingTCPSocket(*(AdressImplementation*)p_adressToConnectTo, p_maxWaitingConnections);

#elif USE_UDP
            // Create a UDP socket used for incomming connections
            newSocket->CreateAndBindUDPSocket(*(AdressImplementation*)p_adressToConnectTo);

#endif
            // Save socket to map
            std::hash<Socket*> HashMap;
            size_t key = HashMap(newSocket);
            m_socketHandleMap[key] = newSocket;

            return key;
        }

        bool NetworkModuleImplementation::AcceptConnection(size_t p_socketID, size_t& p_outSocketID, Adress* p_adressOut)
        {
            // TODOCM add parameters, speculate if we send adress from outside or not(on other
            // functions as well)
            // TODOCM add try catch here, remove test code
            // TODOCM Only accept a connection from a specific adress?

            // Get socket from map
            Socket* socketToAcceptFrom = GetSocketFromMap(p_socketID);

            if(socketToAcceptFrom == nullptr)
            {
                return false;
            }

            // Out socket (instead of dynamically allocating every frame)
            Socket* t_newSocket = nullptr;

#ifdef USE_TCP
            SOCKET newSocketHandle;

            // Accept an incomming connection
            if(!socketToAcceptFrom->AcceptTCPConnection(newSocketHandle, *(AdressImplementation*)p_adressOut))
            {
                return false;
            }
            t_newSocket = new Socket(newSocketHandle);
#elif USE_UDP
            // Accept an incomming connection
            if(!socketToAcceptFrom->AcceptUDPConnection(t_newSocket, *(AdressImplementation*)p_adressOut))
            {
                delete t_newSocket;
                return false;
            }

#endif

            // Save socket to map
            std::hash<Socket*> HashMap;
            size_t key = HashMap(t_newSocket);
            m_socketHandleMap[key] = t_newSocket;
            p_outSocketID = key;


            return true;
        }

        size_t NetworkModuleImplementation::CreateUnreliableSocket()
        {
            // TODOCM add try catch here, remove test code
            Socket* newSocket = new Socket();

            // Create a UDP socket used for incomming connections
            newSocket->CreateUDPSocketToSendAndReceive();

            // Save socket to map
            std::hash<Socket*> HashMap;
            size_t key = HashMap(newSocket);
            m_socketHandleMap[key] = newSocket;

            return key;
        }

        size_t NetworkModuleImplementation::CreateUnreliableWaitingSocket(const Adress* p_adressToConnectTo)
        {
            // TODOCM add try catch here, remove test code
            Socket* newSocket = new Socket();

            // Create a UDP socket used for incomming connections
            newSocket->CreateAndBindUDPSocket(*(AdressImplementation*)p_adressToConnectTo);

            // Save socket to map
            std::hash<Socket*> HashMap;
            size_t key = HashMap(newSocket);
            m_socketHandleMap[key] = newSocket;

            return key;
        }

        void NetworkModuleImplementation::DeleteSocket(size_t p_socketID)
        {
            Socket* socket = GetSocketFromMap(p_socketID);

            if(socket != nullptr)
            {
                delete socket;
                m_socketHandleMap.erase(p_socketID);
            }
        }

        void NetworkModuleImplementation::Shutdown()
        {
// TODO move shutdown to other place
#ifdef PLATFORM == PLATFORM_WINDOWS
            int Result = WSACleanup();
            if(Result == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed to shutdown winsock, WSACleanup failed.");
            }
#elif PLATFORM == PLATFORM_UNIX
// nothing on unix
#endif
        }
    }
}

DoremiEngine::Network::NetworkModule* CreateNetworkModule(const DoremiEngine::Core::SharedContext& p_context)
{
    DoremiEngine::Network::NetworkModule* network = new DoremiEngine::Network::NetworkModuleImplementation();
    return network;
}
