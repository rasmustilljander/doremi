#pragma once
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <DoremiEngine/Network/Include/NetworkModule.hpp>

namespace Doremi
{
	namespace Core
	{
		/**
			TODO doc
		*/
		enum class ConnectionState
		{
			DISCONNECTED,
			STAGING,
			CONNECTED
		};

		class ClientNetworkManager : public Manager
		{
			public:
			/**
				TODOCM doc
			*/
			ClientNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext);

			/**
				TODOCM doc
			*/
			virtual ~ClientNetworkManager();

			/**
				TODOCM doc
			*/
			void Update(double p_dt) override;

			private:
			/**
				TODOCM doc
			*/
			double m_nextUpdateTimer;

			/**
			    TODOCM doc
			*/
			double m_updateInterval;

			/**
			    TODOCM doc
			*/
			ConnectionState m_masterConnectionState;

			/**
			    TODOCM doc
			*/
			ConnectionState m_serverConnectionState;
		};
	}
}

