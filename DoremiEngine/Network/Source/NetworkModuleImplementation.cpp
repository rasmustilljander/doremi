#include <NetworkModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <NetMessage.hpp>
#include <Connection.hpp>
#include <Adress.hpp>

#ifdef WIN32
#include <WinSock2.h>
#pragma comment( lib, "wsock32.lib" ) //TODOCM remove after merge with Rasmus 
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
            if (m_isInitialized)
            {
                throw std::runtime_error("Failed to Startup Network Module, function already called.");
            }

            #ifdef WIN32
            WSADATA wsaData;

            // Initialize Winsock
            int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (Result != NO_ERROR)
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

		void NetworkModuleImplementation::SendNeworkMessage(const NetMessage& p_message, Connection& p_sendToConnection)
		{

		}

        void NetworkModuleImplementation::ConnectUnrealiable(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port)
        {

        }

        void NetworkModuleImplementation::ConnectReliable(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port)
        {

        }

		void NetworkModuleImplementation::Shutdown()
		{
            //TODO move shutdown to other place
            #ifdef WIN32
            int Result = WSACleanup();
            if (Result == SOCKET_ERROR)
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
