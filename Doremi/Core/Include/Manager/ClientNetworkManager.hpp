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
				TODO doc
			*/
			ClientNetworkManager(const DoremiEngine::Core::SharedContext& p_sharedContext);

			/**
				TODO doc
			*/
			virtual ~ClientNetworkManager();

			/**
				TODO doc
			*/
			void Update(double p_dt) override;

			private:
			/**
				TODO doc
			*/
			double m_nextUpdateTimer;

			/**
			TODO doc
			*/
			double m_updateInterval;

			/**
			TODO doc
			*/
			ConnectionState m_masterConnectionState;

			/**
			TODO doc
			*/
			ConnectionState m_serverConnectionState;
		};
	}
}

