#include <NetworkModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <NetMessage.hpp>
#include <Connection.hpp>

namespace DoremiEngine
{
	namespace Network
	{
		NetworkModule::NetworkModule()
		{
			
		}

		NetworkModule::~NetworkModule()
		{

		}

		void NetworkModule::Startup()
		{

		}

		void NetworkModule::SetWorkingDirectory(const std::string& p_workingDirectory)
		{

		}

		void NetworkModule::SendMessage(const NetMessage& p_message, Connection& p_sendToConnection)
		{

		}

		void NetworkModule::Shutdown()
		{

		}
	}
}

DoremiEngine::Network::NetworkModuleInterface* CreateNetworkModule(const DoremiEngine::Core::SharedContext& p_context)
{
	DoremiEngine::Network::NetworkModuleInterface* network = new DoremiEngine::Network::NetworkModule();
	return network;
}
