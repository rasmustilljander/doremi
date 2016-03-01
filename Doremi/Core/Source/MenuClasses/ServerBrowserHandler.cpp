#include <Doremi/Core/Include/MenuClasses/ServerBrowserHandler.hpp>

namespace Doremi
{
    namespace Core
    {
        bool operator<(const IP_Split& p_IP1, const IP_Split& p_IP2)
        {
            if(p_IP1.IP_a == p_IP2.IP_a)
            {
                if(p_IP1.IP_b == p_IP2.IP_b)
                {
                    if(p_IP1.IP_c == p_IP2.IP_c)
                    {
                        return p_IP1.IP_d < p_IP2.IP_d;
                    }
                    else
                    {
                        return p_IP1.IP_c < p_IP2.IP_c;
                    }
                }
                else
                {
                    return p_IP1.IP_b < p_IP2.IP_b;
                }
            }
            else
            {
                return p_IP1.IP_a < p_IP2.IP_a;
            }
        }

        ServerBrowserHandler* ServerBrowserHandler::m_singleton = nullptr;

        ServerBrowserHandler* ServerBrowserHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new ServerBrowserHandler();
            }
            return m_singleton;
        }

        ServerBrowserHandler::ServerBrowserHandler() : m_timeout(5.0f) {}

        ServerBrowserHandler::~ServerBrowserHandler() {}

        void ServerBrowserHandler::UpdateTimeouts(double p_dt)
        {
            auto t_server = m_serversList.begin();

            // Update timeouts
            while(t_server != m_serversList.end())
            {
                t_server->second.LastResponse += p_dt;

                if(t_server->second.LastResponse >= m_timeout)
                {
                    t_server = m_serversList.erase(t_server);
                }
                else
                {
                    ++t_server;
                }
            }
        }

        void ServerBrowserHandler::Update() {}


        void ServerBrowserHandler::UpdateServer(std::string p_name, ServerStates p_serverState, GameMap p_map, uint16_t p_ping, uint8_t p_currentNumPlayers,
                                                uint8_t p_maxNumPlayers, uint16_t p_port, uint8_t p_IP_a, uint8_t p_IP_b, uint8_t p_IP_c, uint8_t p_IP_d)
        {
            // Create key
            std::pair<IP_Split, Port> t_key;
            t_key.first = IP_Split(p_IP_a, p_IP_a, p_IP_a, p_IP_a);
            t_key.second = p_port;

            // Update by key
            m_serversList[t_key].Name = p_name;
            m_serversList[t_key].ServerState = p_serverState;
            m_serversList[t_key].Map = p_map;
            m_serversList[t_key].CurrentNumPlayers = p_currentNumPlayers;
            m_serversList[t_key].MaxNumPlayers = p_maxNumPlayers;
            m_serversList[t_key].LastResponse = 0;
        }

        IP_Split ServerBrowserHandler::GetSelectedServerIP()
        {
            if(m_serversList.size())
            {
                return m_serversList.begin()->first.first;
            }
            return IP_Split(127, 0, 0, 1);
        }

        uint16_t ServerBrowserHandler::GetSelectedServerPort()
        {
            if(m_serversList.size())
            {
                return m_serversList.begin()->first.second;
            }
            return 0;
        }
    }
}
