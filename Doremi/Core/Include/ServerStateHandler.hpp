#pragma once
#include <string>

namespace Doremi
{
    namespace Core
    {
        enum class ServerStates
        {
            LOBBY,
            IN_GAME,
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

            ServerStates GetState() { return m_state; }

            GameMap GetMap() { return m_map; }

            uint8_t GetMaxPlayers() { return m_maxPlayers; }

            void SetServerName(std::string p_name);

            std::string GetServerName();


        private:
            ServerStateHandler();

            ~ServerStateHandler();

            static ServerStateHandler* m_singleton;

            ServerStates m_state;

            GameMap m_map;

            uint8_t m_maxPlayers;

            std::string m_serverName;
        };
    }
}