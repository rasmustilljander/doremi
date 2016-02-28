#pragma once
#include <string>
#include <ServerStateHandler.hpp>
#include <map>

namespace Doremi
{
    namespace Core
    {
        struct IP_Split
        {
            IP_Split() : IP_a(0), IP_b(0), IP_c(0), IP_d(0) {}
            IP_Split(uint8_t p_IP_a, uint8_t p_IP_b, uint8_t p_IP_c, uint8_t p_IP_d) : IP_a(p_IP_a), IP_b(p_IP_b), IP_c(p_IP_c), IP_d(p_IP_d) {}
            uint8_t IP_a;
            uint8_t IP_b;
            uint8_t IP_c;
            uint8_t IP_d;
        };

        struct ServerData
        {
            std::string Name;
            ServerStates ServerState;
            GameMap Map;
            uint16_t Ping;

            uint8_t CurrentNumPlayers;
            uint8_t MaxNumPlayers;

            double LastResponse;
        };

        typedef uint16_t Port;

        class ServerListHandler
        {
        public:
            static ServerListHandler* GetInstance();

            void Update(double p_dt);

            void UpdateServer(std::string p_name, ServerStates p_serverState, GameMap p_map, uint16_t p_ping, uint8_t p_currentNumPlayers,
                              uint8_t p_maxNumPlayers, uint16_t p_port, uint8_t p_IP_a, uint8_t p_IP_b, uint8_t p_IP_c, uint8_t p_IP_d);

        private:
            ServerListHandler();
            ~ServerListHandler();

            static ServerListHandler* m_singleton;

            std::map<std::pair<IP_Split, Port>, ServerData> m_serversList;

            double m_timeout;
        };
    }
}
