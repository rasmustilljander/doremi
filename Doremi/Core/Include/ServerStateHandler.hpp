#pragma once
#include <string>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        enum class ServerStates
        {
            LOBBY,
            IN_GAME,
            EXIT
        };

        /**
            Enum used for map, max 255 maps, could do in some other way just template
        */
        enum class GameMap
        {
            BEST_MAP,
        };

        class ServerStateHandler
        {
        public:
            static ServerStateHandler* GetInstance();

            static void StartupServerStateHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ServerStates GetState() { return m_state; }

            void SetState(const ServerStates& p_serverState) { m_state = p_serverState; }

            GameMap GetMap() { return m_map; }

            uint8_t GetMaxPlayers() { return m_maxPlayers; }

            void SetServerName(std::string p_name);

            std::string GetServerName();

        private:
            explicit ServerStateHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~ServerStateHandler();

            const DoremiEngine::Core::SharedContext& m_sharedContext;

            static ServerStateHandler* m_singleton;

            ServerStates m_state;

            GameMap m_map;

            uint8_t m_maxPlayers;

            std::string m_serverName;
        };
    }
}