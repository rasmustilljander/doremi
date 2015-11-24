#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

#if defined(_WINDLL)
#define NETWORK_DLL_EXPORT __declspec(dllexport)
#else
#define NETWORK_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
	namespace Network
	{
		struct NetMessage;
		struct Connection;

		class NetworkModuleInterface : public DoremiEngine::Core::EngineModule
		{
		public:
			/**
			TODO
			*/
			virtual void Startup() = 0;

			/**
			TODO
			*/
			virtual void SetWorkingDirectory(const std::string& p_workingDirectory) = 0;

			/**
			TODO docs
			*/
			virtual void SendMessage(const NetMessage& p_message, Connection& p_sendToConnection) = 0;

			/**
			TODO
			*/
			virtual void Shutdown() = 0;
		};
	}
}

extern "C" {
	typedef DoremiEngine::Network::NetworkModuleInterface* (*CREATE_NETWORK_MODULE)(const DoremiEngine::Core::SharedContext&);
	NETWORK_DLL_EXPORT DoremiEngine::Network::NetworkModuleInterface*
		CreateNetworkModule(const DoremiEngine::Core::SharedContext& p_context);
}