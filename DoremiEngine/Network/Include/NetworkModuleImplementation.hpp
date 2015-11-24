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
				TODO docs	
			*/
				NetworkModuleImplementation();

			/**
				TODO docs
			*/
			virtual ~NetworkModuleImplementation();

			/**
				TODO docs
			*/
			void Startup() override;

			/**
				TODO docs
			*/
			void SetWorkingDirectory(const std::string& p_workingDirectory) override;

			/**
				TODO docs
			*/
			void SendMessage(const NetMessage& p_message, Connection& p_sendToConnection) override;

			/**
				TODO docs
			*/
			void Shutdown() override;
		};
	}
}