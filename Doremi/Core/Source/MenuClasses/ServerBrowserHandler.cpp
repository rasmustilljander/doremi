#include <Doremi/Core/Include/MenuClasses/ServerBrowserHandler.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/ChangeMenuState.hpp>

namespace Doremi
{
    namespace Core
    {
        bool operator==(const IP_Split& p_IP1, const IP_Split& p_IP2)
        {
            if(p_IP1.IP_a == p_IP2.IP_a && p_IP1.IP_b == p_IP2.IP_b && p_IP1.IP_c == p_IP2.IP_c && p_IP1.IP_d == p_IP2.IP_d)
            {
                return true;
            }
            return false;
        }

        ServerBrowserHandler* ServerBrowserHandler::m_singleton = nullptr;

        void ServerBrowserHandler::StartupServerBrowserHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("ServerBrowserHandler called multiple times");
            }
            m_singleton = new ServerBrowserHandler(p_sharedContext);
        }

        ServerBrowserHandler* ServerBrowserHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("GetInstance was  called before StartupServerBrowserHandler");
            }
            return m_singleton;
        }

        ServerBrowserHandler::ServerBrowserHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext), m_timeout(5.0f), m_selectedServer(nullptr)
        {
        }

        ServerBrowserHandler::~ServerBrowserHandler() {}


        void ServerBrowserHandler::Update(double p_dt)
        {
            UpdateTimeouts(p_dt);

            UpdateInputs();
        }

        void ServerBrowserHandler::UpdateTimeouts(double p_dt)
        {
            auto t_server = m_serversList.begin();

            // Update timeouts
            while(t_server != m_serversList.end())
            {
                (*t_server)->LastResponse += p_dt;

                if((*t_server)->LastResponse >= m_timeout)
                {
                    // If our selected server is the one we remove
                    if(m_selectedServer == (*t_server))
                    {
                        m_selectedServer == nullptr;
                    }

                    t_server = m_serversList.erase(t_server);
                }
                else
                {
                    ++t_server;
                }
            }
        }

        void ServerBrowserHandler::UpdateInputs()
        {
            uint32_t t_mousePosX;
            uint32_t t_mousePosY;

            // Get mouse cursor position
            InputHandlerClient* t_inputHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance())->GetInputHandler();
            t_inputHandler->GetMousePos(t_mousePosX, t_mousePosY);

            // Get screen resolution
            XMFLOAT2 t_screenResolution = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().GetScreenResolution();

            // Make position to screen cordinates
            float t_mouseScreenPosX = static_cast<float>(t_mousePosX) / t_screenResolution.x;
            float t_mouseScreenPosY = static_cast<float>(t_mousePosY) / t_screenResolution.y;

            // Set standard not to be selected
            m_highlightedButton = nullptr;

            // Check if cursor is inside one of the buttons if it is then save that buttons index
            for(auto& t_server : m_serversList)
            {
                if(t_server->m_serverButton.CheckIfInside(t_mouseScreenPosX, t_mouseScreenPosY))
                {
                    m_highlightedButton = t_server;
                }
                else
                {
                    // do nothing
                }
            }


            // Check if we're pressing
            if(t_inputHandler->CheckBitMaskInputFromGame((int)UserCommandPlaying::LeftClick) && m_highlightedButton != nullptr)
            {
                // If our highlighted button and selected button are the same, we essentialy double click
                if(m_highlightedButton == m_selectedServer)
                {
                    switch(m_selectedServer->m_serverButton.m_buttonAction)
                    {
                        case DoremiButtonActions::START_GAME:
                        {
                            // passing state change event
                            ChangeMenuState* menuEvent = new Core::ChangeMenuState();
                            menuEvent->state = DoremiGameStates::RUNGAME;
                            EventHandler::GetInstance()->BroadcastEvent(menuEvent);
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
                else // else we set the clicked button to be selected
                {
                    m_selectedServer = m_highlightedButton;
                }
            }
            else
            {
                // Nothing
            }

            // Keep the one we selected selected texture
            if(m_selectedServer != nullptr)
            {
                m_selectedServer->m_serverButton.SetSelected();
            }
        }


        void ServerBrowserHandler::UpdateServer(std::string p_name, ServerStates p_serverState, GameMap p_map, uint16_t p_ping, uint8_t p_currentNumPlayers,
                                                uint8_t p_maxNumPlayers, uint16_t p_port, uint8_t p_IP_a, uint8_t p_IP_b, uint8_t p_IP_c, uint8_t p_IP_d)
        {
            // Create key
            IP_Split t_incIP = IP_Split(p_IP_a, p_IP_a, p_IP_a, p_IP_a);

            for(auto& t_server : m_serversList)
            {
                // If same IP and port
                if(t_server->IP == t_incIP && t_server->Port == p_port)
                {
                    t_server->Name = p_name;
                    t_server->ServerState = p_serverState;
                    t_server->Map = p_map;
                    t_server->CurrentNumPlayers = p_currentNumPlayers;
                    t_server->MaxNumPlayers = p_maxNumPlayers;
                    t_server->LastResponse = 0;
                }
            }
        }

        IP_Split ServerBrowserHandler::GetSelectedServerIP()
        {
            if(m_selectedServer != nullptr)
            {
                return m_selectedServer->IP;
            }
            return IP_Split(127, 0, 0, 1);
        }

        uint16_t ServerBrowserHandler::GetSelectedServerPort()
        {
            if(m_selectedServer != nullptr)
            {
                return m_selectedServer->Port;
            }
            return 0;
        }
    }
}
