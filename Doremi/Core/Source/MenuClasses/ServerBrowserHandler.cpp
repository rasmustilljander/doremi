#include <Doremi/Core/Include/MenuClasses/ServerBrowserHandler.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/ChangeMenuState.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <algorithm>


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
            : m_sharedContext(p_sharedContext), m_timeout(1.0f), m_selectedServer(nullptr), m_curIndexTop(0)
        {
            m_serverBoxSize = 0.04;

            DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Create server browser background TEMPORARY WILL BECOME SCREEN OBJECT
            ButtonMaterials t_butMat;
            t_butMat.m_vanillaMaterial = t_meshManager.BuildMaterialInfo("ANB_Menu__0002_SERVER_Frame2.dds");
            t_butMat.m_selectedLightedMaterial = nullptr;
            t_butMat.m_highLightedMaterial = nullptr;

            // Basic position
            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.355f, m_serverBoxSize * 8 + 0.01);
            t_data.origo = XMFLOAT2(0.0f, 4 * m_serverBoxSize * 2.0f);
            t_data.position = XMFLOAT2(0.5f, 0.2f - m_serverBoxSize);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);
            DoremiEngine::Graphic::SpriteInfo* t_spriteInfo = t_meshManager.BuildSpriteInfo(t_data);

            m_background = Button(t_butMat, t_spriteInfo, DoremiButtonActions::EXIT);
        }

        ServerBrowserHandler::~ServerBrowserHandler() {}


        void ServerBrowserHandler::UpdateServer(std::string p_name, ServerStates p_serverState, GameMap p_map, uint16_t p_ping, uint8_t p_currentNumPlayers,
                                                uint8_t p_maxNumPlayers, uint16_t p_port, uint8_t p_IP_a, uint8_t p_IP_b, uint8_t p_IP_c, uint8_t p_IP_d)
        {
            // Create key
            IP_Split t_incIP = IP_Split(p_IP_a, p_IP_b, p_IP_c, p_IP_d);

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

                    // Update texts

                    t_server->NameText.UpdateText(m_sharedContext, t_server->Name);

                    std::string t_serverState = "";
                    switch(p_serverState)
                    {
                        case ServerStates::IN_GAME:
                        {
                            t_serverState = "IN GAME";
                            break;
                        }
                        case ServerStates::LOBBY:
                        {
                            t_serverState = "LOBBY";
                            break;
                        }
                        default:
                        {
                            t_serverState = "UNKOWN";
                            break;
                        }
                    }

                    t_server->StateText.UpdateText(m_sharedContext, t_serverState);
                    t_server->MapText.UpdateText(m_sharedContext, "Beat in the City"); // TODO change here if we create more maps
                    t_server->NumPlayerText.UpdateText(m_sharedContext, std::to_string(p_currentNumPlayers) + "/" + std::to_string(p_maxNumPlayers));
                    t_server->PingText.UpdateText(m_sharedContext, "0");

                    return;
                }
            }

            // If we didn't find a matchin, we create a new
            ServerData* t_newServer = new ServerData();
            t_newServer->Name = p_name;
            t_newServer->ServerState = p_serverState;
            t_newServer->Map = p_map;
            t_newServer->CurrentNumPlayers = p_currentNumPlayers;
            t_newServer->MaxNumPlayers = p_maxNumPlayers;
            t_newServer->LastResponse = 0;
            t_newServer->IP = t_incIP;
            t_newServer->Port = p_port;


            DoremiEngine::Graphic::MeshManager& t_meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();
            // Basic position
            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.35f, m_serverBoxSize);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.5f, 0.0f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);

            // Skapa en buttonmaterial struct. Denna håller 2 buildmaterialinfos för att göra kortare parameterlistor
            Doremi::Core::ButtonMaterials t_buttonMaterials;

            // Load materials
            t_buttonMaterials.m_vanillaMaterial = t_meshManager.BuildMaterialInfo("ANB_Menu__0001_SERVER_Bar_Highlight.dds");
            t_buttonMaterials.m_highLightedMaterial = t_meshManager.BuildMaterialInfo("ANB_Menu__0000_SERVER_Bar_Highlight.dds");
            t_buttonMaterials.m_selectedLightedMaterial = t_meshManager.BuildMaterialInfo("ANB_Menu__0000_SERVER_Bar_Highlight.dds");

            DoremiEngine::Graphic::SpriteInfo* t_spriteInfo = t_meshManager.BuildSpriteInfo(t_data);

            t_newServer->m_serverButton = Button(t_buttonMaterials, t_spriteInfo, DoremiButtonActions::START_GAME);

            DoremiEngine::Graphic::MaterialInfo* t_matInfo = t_meshManager.BuildMaterialInfo("FontNormal.dds");

            /*
            37 width x 16 = 592
            29 height x 14 = 406
            is 592x592 so 37, 29 divided by that
            */
            // TODOCM use aspect ratio to determine sizes
            XMFLOAT2 t_tableCharSize = XMFLOAT2(0.0625f, 0.0714285714285714f);
            t_newServer->NameText = Text(t_matInfo, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));
            t_newServer->NameText.SetText(m_sharedContext, p_name);


            t_newServer->NumPlayerText =
                Text(t_matInfo, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));
            t_newServer->NumPlayerText.SetText(m_sharedContext, std::to_string(p_currentNumPlayers) + "/" + std::to_string(p_maxNumPlayers));


            std::string t_serverState = "";
            switch(p_serverState)
            {
                case ServerStates::IN_GAME:
                {
                    t_serverState = "IN GAME";
                    break;
                }
                case ServerStates::LOBBY:
                {
                    t_serverState = "LOBBY";
                    break;
                }
                default:
                {
                    t_serverState = "UNKOWN";
                    break;
                }
            }
            t_newServer->StateText = Text(t_matInfo, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));
            t_newServer->StateText.SetText(m_sharedContext, t_serverState);


            // TODO change based on type
            t_newServer->MapText = Text(t_matInfo, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));
            t_newServer->MapText.SetText(m_sharedContext, "Beat in the City");

            // TODOCM put ping here when we introduce ping messages
            t_newServer->PingText = Text(t_matInfo, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));
            t_newServer->PingText.SetText(m_sharedContext, "0");

            m_serversList.push_back(t_newServer);
        }

        void ServerBrowserHandler::Update(double p_dt)
        {
            UpdateTimeouts(p_dt);

            UpdatePositions();

            UpdateInputs();

            // split update position from visual, cause that's logical
            UpdateVisual();
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

                    delete(*t_server)->m_serverButton.m_spriteInfo;
                    (*t_server)->NameText.DeleteText();
                    (*t_server)->NumPlayerText.DeleteText();
                    (*t_server)->StateText.DeleteText();
                    (*t_server)->MapText.DeleteText();
                    (*t_server)->PingText.DeleteText();


                    t_server = m_serversList.erase(t_server);
                }
                else
                {
                    ++t_server;
                }
            }
        }

        void ServerBrowserHandler::UpdatePositions()
        {
            // max visisble is 6
            m_frameActiveServerList.clear();


            uint32_t t_numServers = m_serversList.size();
            if(t_numServers <= 6)
            {
                m_curIndexTop = 0;
            }
            else if(m_curIndexTop + 6 > t_numServers) // if we have 7 servers, and index is 0 this should ignore
            {
                m_curIndexTop = t_numServers - 6;
            }

            // Step over
            auto& t_server = m_serversList.begin();
            size_t activeServerCounter = 0;
            while(t_server != m_serversList.end() && activeServerCounter < m_curIndexTop)
            {
                ++activeServerCounter;
                ++t_server;
            }

            // Add max 6 to list
            activeServerCounter = 0;
            while(t_server != m_serversList.end() && activeServerCounter < 6)
            {
                m_frameActiveServerList.push_back(*t_server);
                ++activeServerCounter;
                ++t_server;
            }


            // For each server we got on display

            t_server = m_frameActiveServerList.begin();
            size_t counter = 0;
            while(t_server != m_frameActiveServerList.end())
            {
                DoremiEngine::Graphic::SpriteData& t_data = (*t_server)->m_serverButton.m_spriteInfo->GetData();
                t_data.position.y = 0.3f + counter * m_serverBoxSize * 2.0f;

                (*t_server)->NameText.UpdatePosition(XMFLOAT2(0.17f, 0.3f + counter * m_serverBoxSize * 2.0f));
                (*t_server)->StateText.UpdatePosition(XMFLOAT2(0.37f, 0.3f + counter * m_serverBoxSize * 2.0f));
                (*t_server)->MapText.UpdatePosition(XMFLOAT2(0.50f, 0.3f + counter * m_serverBoxSize * 2.0f));
                (*t_server)->NumPlayerText.UpdatePosition(XMFLOAT2(0.72f, 0.3f + counter * m_serverBoxSize * 2.0f));
                (*t_server)->PingText.UpdatePosition(XMFLOAT2(0.82f, 0.3f + counter * m_serverBoxSize * 2.0f));

                ++counter;
                ++t_server;
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
            for(auto& t_server : m_frameActiveServerList)
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
            if(t_inputHandler->CheckForOnePress((int)UserCommandPlaying::LeftClick))
            {
                if(m_highlightedButton != nullptr)
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
                                menuEvent->state = DoremiGameStates::LOADING;
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
                    m_selectedServer = nullptr;
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

            if(t_inputHandler->CheckForOnePress((int)UserCommandPlaying::ScrollWpnDown))
            {
                m_curIndexTop++;
            }
            else if(t_inputHandler->CheckForOnePress((int)UserCommandPlaying::ScrollWpnUp))
            {
                m_curIndexTop--;
                m_curIndexTop = static_cast<int32_t>(std::max(static_cast<float>(m_curIndexTop), 0.0f));
            }
        }

        void ServerBrowserHandler::UpdateVisual()
        {
            // Reset previous buttons
            m_drawButtons.clear();
            m_drawText.clear();

            m_drawButtons.push_back(&m_background);

            auto& t_server = m_frameActiveServerList.begin();

            // For each server we got on display
            size_t counter = 0;
            while(t_server != m_frameActiveServerList.end())
            {

                m_drawButtons.push_back(&((*t_server)->m_serverButton));


                m_drawText.push_back(&((*t_server)->NameText));
                m_drawText.push_back(&((*t_server)->NumPlayerText));
                m_drawText.push_back(&((*t_server)->StateText));
                m_drawText.push_back(&((*t_server)->MapText));
                m_drawText.push_back(&((*t_server)->PingText));

                ++counter;
                ++t_server;
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
