#pragma once
#include <NetworkModule.hpp>
#include <string>

namespace DoremiEngine
{
	namespace Network
	{
		class NetworkModuleImplementation : public NetworkModule
		{
			public:
			/**
				TODOCM docs	
			*/
				NetworkModuleImplementation();

			/**
				TODOCM docs
			*/
			virtual ~NetworkModuleImplementation();

			/**
				TODOCM docs
			*/
			void Startup() override;

			/**
				TODOCM docs
			*/
			void SetWorkingDirectory(const std::string& p_workingDirectory) override;

			/**
				TODOCM docs
			*/
			void SendNeworkMessage(const NetMessage& p_message, Connection& p_sendToConnection) override;

			/**
				TODOCM docs
			*/
			void Shutdown() override;
		};
	}
}