#include <NetworkModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <NetMessage.hpp>
#include <Connection.hpp>

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

		}

		void NetworkModuleImplementation::SetWorkingDirectory(const std::string& p_workingDirectory)
		{

		}

		void NetworkModuleImplementation::SendMessage(const NetMessage& p_message, Connection& p_sendToConnection)
		{

		}

		void NetworkModuleImplementation::Shutdown()
		{

		}
	}
}

DoremiEngine::Network::NetworkModule* CreateNetworkModule(const DoremiEngine::Core::SharedContext& p_context)
{
	DoremiEngine::Network::NetworkModule* network = new DoremiEngine::Network::NetworkModuleImplementation();
	return network;
}
