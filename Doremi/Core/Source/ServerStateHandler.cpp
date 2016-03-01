#include <Doremi/Core/Include/ServerStateHandler.hpp>
#include <DoremiEngine/Configuration/Include/ConfigurationModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

namespace Doremi
{
    namespace Core
    {
        ServerStateHandler* ServerStateHandler::m_singleton = nullptr;

        ServerStateHandler* ServerStateHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("GetInstance called before StartupServerStateHandler");
            }
            return m_singleton;
        }

        void ServerStateHandler::StartupServerStateHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("StartupServerStateHandler called multiple times.");
            }
            m_singleton = new ServerStateHandler(p_sharedContext);
        }

        ServerStateHandler::ServerStateHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext), m_state(ServerStates::IN_GAME), m_serverName("FirstServer"), m_maxPlayers(16), m_map(GameMap::BEST_MAP)
        {
            const DoremiEngine::Configuration::ConfiguartionInfo& t_configInfo = p_sharedContext.GetConfigurationModule().GetAllConfigurationValues();
            m_maxPlayers = t_configInfo.MaxPlayers;
            m_serverName = t_configInfo.ServerName;
        }

        ServerStateHandler::~ServerStateHandler() {}

        void ServerStateHandler::SetServerName(std::string p_name)
        {
            // 19 chars is max size of name
            p_name.resize(19);

            m_serverName = p_name;
        }

        std::string ServerStateHandler::GetServerName() { return m_serverName; }
    }
}