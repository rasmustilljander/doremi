#include <NetworkModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <NetMessage.hpp>
#include <Connection.hpp>

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
		NetworkModuleImplementation::NetworkModuleImplementation()
		{
			
		}

		NetworkModuleImplementation::~NetworkModuleImplementation()
		{

		}

		void NetworkModuleImplementation::Startup()
		{
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
		}

		void NetworkModuleImplementation::SetWorkingDirectory(const std::string& p_workingDirectory)
		{

		}

		void NetworkModuleImplementation::SendNeworkMessage(const NetMessage& p_message, Connection& p_sendToConnection)
		{

		}

		void NetworkModuleImplementation::Shutdown()
		{
            //TODO move shutdown to other place
            #ifdef WIN32
            WSACleanup();
            #endif
		}
	}
}

DoremiEngine::Network::NetworkModule* CreateNetworkModule(const DoremiEngine::Core::SharedContext& p_context)
{
	DoremiEngine::Network::NetworkModule* network = new DoremiEngine::Network::NetworkModuleImplementation();
	return network;
}
