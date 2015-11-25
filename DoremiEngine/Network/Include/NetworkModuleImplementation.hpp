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
            void ConnectUnrealiable(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port);

            /**
                TODOCM docs
            */
            void ConnectReliable(uint32_t p_a, uint32_t p_b, uint32_t p_c, uint32_t p_d, uint16_t p_port);

			/**
				TODOCM docs
			*/
			void Shutdown() override;

            private:
            /**
                TODOCM docs
            */
            bool m_isInitialized;
		};
	}
}