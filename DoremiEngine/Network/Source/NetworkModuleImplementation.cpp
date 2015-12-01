// Project specific
#include <NetworkModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Adress.hpp>

// Standard libraries

#ifdef WIN32
#include <WinSock2.h>
#pragma comment(lib, "wsock32.lib") // TODOCM remove after merge with Rasmus
#elif
#error Platform not supported
#endif


namespace DoremiEngine
{
    namespace Network
    {
        NetworkModuleImplementation::NetworkModuleImplementation() : m_isInitialized(false)
        {
        }

        NetworkModuleImplementation::~NetworkModuleImplementation()
        {
        }

        void NetworkModuleImplementation::Startup()
        {
            if(m_isInitialized)
            {
                throw std::runtime_error(
                "Failed to Startup Network Module, function already called.");
            }

#ifdef WIN32
            WSADATA wsaData;

            // Initialize Winsock
            int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if(Result != NO_ERROR)
            {
                throw std::runtime_error("Failed to load Winsock, WSAStartup failed.");
            }
#elif
#endif

            m_isInitialized = true;
        }

        void NetworkModuleImplementation::SetWorkingDirectory(const std::string& p_workingDirectory)
        {
        }

        bool NetworkModuleImplementation::SendData(void* t_data, const uint32_t &t_dataSize, const size_t& p_sendToSocket)
        {
            // Check if socket exist in map TODOCM remove if not using map
            std::map<size_t, Socket*>::iterator iter = m_SocketHandleMap.find(p_sendToSocket);
            if (iter == m_SocketHandleMap.end())
            {
                throw std::runtime_error("Invalid socketID sent to SendData.");
            }

            // Attempt to send data, returns true if all data is sent
            bool SendSuccessful = iter->second->Send(t_data, t_dataSize);

            return SendSuccessful;
        }

        bool NetworkModuleImplementation::RecieveData(void* t_data, const uint32_t &t_dataSize, const size_t& p_recieveFromSocket)
        {
            // Check if socket exist in map TODOCM remove if not using map
            std::map<size_t, Socket*>::iterator iter = m_SocketHandleMap.find(p_recieveFromSocket);
            if (iter == m_SocketHandleMap.end())
            {
                throw std::runtime_error("Invalid socketID sent to RecieveData.");
            }

            // Attempt to recieve data, returns true if all data is sent
            bool RecieveSuccessful = iter->second->Recieve(t_data, t_dataSize);
            
            return RecieveSuccessful;
        }

        void NetworkModuleImplementation::ConnectUnrealiable(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port)
        {
        }

        size_t NetworkModuleImplementation::CreateReliableConnection(uint32_t p_a,
                                                                     uint32_t p_b,
                                                                     uint32_t p_c,
                                                                     uint32_t p_d,
                                                                     uint16_t p_port)
        {
            // TODOCM add try catch here, remove test code
            Socket* newSocket = new Socket();
            Adress newAdress = Adress(p_a, p_b, p_c, p_d, p_port);

            // Create a TCP socket used for incomming connections
            newSocket->CreateWaitingTCPSocket(newAdress, 1);

            // Save socket to map
            std::hash<Socket*> HashMap;
            size_t key = HashMap(newSocket);
            m_SocketHandleMap[key] = newSocket;

            return key;
        }

        size_t NetworkModuleImplementation::AcceptConnection(size_t p_socketID)
        {
            // TODOCM add parameters, speculate if we send adress from outside or not(on other
            // functions as well)
            // TODOCM add try catch here, remove test code
            Adress newAdress = Adress();
            Socket* newSocket;

            // Check if socket exist in map TODOCM remove if not using map
            std::map<size_t, Socket*>::iterator iter = m_SocketHandleMap.find(p_socketID);
            if(iter == m_SocketHandleMap.end())
            {
                throw std::runtime_error("Invalid socketID sent to AcceptConnection.");
            }

            // Accept an incomming connection
            newSocket = new Socket(iter->second->AcceptTCPConnection(newAdress));

            // Save socket to map
            std::hash<Socket*> HashMap;
            size_t key = HashMap(newSocket);
            m_SocketHandleMap[key] = newSocket;

            return key;
        }

        size_t NetworkModuleImplementation::ConnectToReliable(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port)
        {
            // TODOCM add try catch here, remove test code
            Socket* newSocket = new Socket();
            Adress newAdress = Adress(p_a, p_b, p_c, p_d, p_port);

            // Create a socket and attempt connect it to a socket used for incomming
            newSocket->CreateAndConnectTCPSocket(newAdress);

            // Save socket to map
            std::hash<Socket*> HashMap;
            size_t key = HashMap(newSocket);
            m_SocketHandleMap[key] = newSocket;

            return key;
        }

        void NetworkModuleImplementation::Shutdown()
        {
            // TODO move shutdown to other place
#ifdef WIN32
            int Result = WSACleanup();
            if(Result == SOCKET_ERROR)
            {
                throw std::runtime_error("Failed to shutdown winsock, WSACleanup failed.");
            }
#endif
        }
    }
}

DoremiEngine::Network::NetworkModule* CreateNetworkModule(const DoremiEngine::Core::SharedContext& p_context)
{
    DoremiEngine::Network::NetworkModule* network = new DoremiEngine::Network::NetworkModuleImplementation();
    return network;
}
