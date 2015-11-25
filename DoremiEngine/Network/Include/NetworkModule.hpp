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

		class NetworkModule : public DoremiEngine::Core::EngineModule
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
			virtual void SendNeworkMessage(const NetMessage& p_message, Connection& p_sendToConnection) = 0;

            /**
            TODOCM docs
            */
            virtual void ConnectUnrealiable(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port) = 0;

            /**
            TODOCM docs
            */
            virtual void ConnectReliable(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port) = 0;

			/**
			TODO
			*/
			virtual void Shutdown() = 0;
		};
	}
}

extern "C" {
	typedef DoremiEngine::Network::NetworkModule* (*CREATE_NETWORK_MODULE)(const DoremiEngine::Core::SharedContext&);
	NETWORK_DLL_EXPORT DoremiEngine::Network::NetworkModule*
		CreateNetworkModule(const DoremiEngine::Core::SharedContext& p_context);
}