#pragma once
#include <NetworkModuleInterface.hpp>
#include <string>

namespace DoremiEngine
{
	namespace Network
	{
		class NetworkModule : public NetworkModuleInterface
		{
			public:
			/**
				TODO docs	
			*/
			NetworkModule();

			/**
				TODO docs
			*/
			virtual ~NetworkModule();

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