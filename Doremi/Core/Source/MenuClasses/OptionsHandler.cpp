#include <Doremi/Core/Include/MenuClasses/OptionsHandler.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/HealthComponent.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/EventHandler/Events/ChangeMenuState.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>

namespace Doremi
{
    namespace Core
    {
        OptionsHandler* OptionsHandler::m_singleton = nullptr;

        OptionsHandler* OptionsHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("GetInstance called before StartOptionsHandler");
            }
            return m_singleton;
        }

        void OptionsHandler::StartOptionsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("StartOptionsHandler called multiple times");
            }
            m_singleton = new OptionsHandler(p_sharedContext);
        }

        OptionsHandler::OptionsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext), t_resolutionDropdownIsActive(false), t_refreshDropdownIsActive(false), t_monitorDropdownIsActive(false)
        {
            DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();
            DoremiEngine::Graphic::DirectXManager& t_directxManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager();

            // Load start
            // Get current resolution
            XMFLOAT2 t_resolution = t_directxManager.GetScreenResolution();
            m_currentResolutionWidth = static_cast<uint32_t>(t_resolution.x);
            m_currentResolutionHeight = static_cast<uint32_t>(t_resolution.y);

            // Get current Monitor
            m_currentMonitor = t_directxManager.GetCurrentMonitor();

            // Get current refresh rate
            m_currentRefreshRate = t_directxManager.GetRefreshRate();

            // Need later
            // Get number of monitors
            uint32_t t_numberOfMonitors = t_directxManager.GetNumberOfMonitors();

            // Get resolutions
            std::vector<std::pair<uint32_t, uint32_t>> t_resolutions1 = t_directxManager.GetResolutions(0);

            // Get refresh rates for monitor and resolution
            // std::vector<uint32_t> m_refreshRAtes = t_directxManager.GetRefreshRates(0, *(t_resolutions1.rend()));


            CreateScreenResolutionOption(p_sharedContext);
            CreateRefreshOption(p_sharedContext);
            CreateMonitorOption(p_sharedContext);
        }

        OptionsHandler::~OptionsHandler() {}

        void OptionsHandler::CreateScreenResolutionOption(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Create the square you press to get the dropdown

            // Basic position
            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.05f, 0.015f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.5f, 0.2f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);

            // Skapa en buttonmaterial struct. Denna håller 2 buildmaterialinfos för att göra kortare parameterlistor
            Doremi::Core::ButtonMaterials t_buttonMaterialsMiddle;

            // Load materials for the actual square to show resoluton
            t_buttonMaterialsMiddle.m_vanillaMaterial = t_meshManager.BuildMaterialInfo("NormalServer.dds");
            t_buttonMaterialsMiddle.m_highLightedMaterial = t_meshManager.BuildMaterialInfo("HoverServer.dds");
            t_buttonMaterialsMiddle.m_selectedLightedMaterial = nullptr;

            DoremiEngine::Graphic::SpriteInfo* t_spriteInfoMiddleButton = t_meshManager.BuildSpriteInfo(t_data);

            m_resolutionButton = Button(t_buttonMaterialsMiddle, t_spriteInfoMiddleButton, DoremiButtonActions::RESOLUTION_PRESS);

            m_optionsButtons.push_back(&m_resolutionButton);


            // Create the overlaying text
            DoremiEngine::Graphic::MaterialInfo* t_matinfoText = t_meshManager.BuildMaterialInfo("FontNormal.dds");

            XMFLOAT2 t_tableCharSize = XMFLOAT2(0.0625f, 0.049f);
            m_resolutionText = Text(t_matinfoText, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(0.5f, 0.2f), XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));
            m_resolutionText.SetText(p_sharedContext, std::to_string(m_currentResolutionWidth) + "x" + std::to_string(m_currentResolutionHeight));

            m_text.push_back(&m_resolutionText);
        }

        void OptionsHandler::CreateRefreshOption(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Create the square you press to get the dropdown

            // Basic position
            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.05f, 0.015f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.5f, 0.3f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);

            // Skapa en buttonmaterial struct. Denna håller 2 buildmaterialinfos för att göra kortare parameterlistor
            Doremi::Core::ButtonMaterials t_buttonMaterialsMiddle;

            // Load materials for the actual square to show resoluton
            t_buttonMaterialsMiddle.m_vanillaMaterial = t_meshManager.BuildMaterialInfo("NormalServer.dds");
            t_buttonMaterialsMiddle.m_highLightedMaterial = t_meshManager.BuildMaterialInfo("HoverServer.dds");
            t_buttonMaterialsMiddle.m_selectedLightedMaterial = nullptr;

            DoremiEngine::Graphic::SpriteInfo* t_spriteInfoMiddleButton = t_meshManager.BuildSpriteInfo(t_data);

            m_refreshButton = Button(t_buttonMaterialsMiddle, t_spriteInfoMiddleButton, DoremiButtonActions::REFRESH_PRESS);

            m_optionsButtons.push_back(&m_refreshButton);


            // Create the overlaying text
            DoremiEngine::Graphic::MaterialInfo* t_matinfoText = t_meshManager.BuildMaterialInfo("FontNormal.dds");

            XMFLOAT2 t_tableCharSize = XMFLOAT2(0.0625f, 0.049f);
            m_refreshText = Text(t_matinfoText, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(0.5f, 0.3f), XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));
            m_refreshText.SetText(p_sharedContext, std::to_string(m_currentRefreshRate));

            m_text.push_back(&m_refreshText);
        }

        void OptionsHandler::CreateMonitorOption(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Create the square you press to get the dropdown

            // Basic position
            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.05f, 0.015f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.5f, 0.4f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);

            // Skapa en buttonmaterial struct. Denna håller 2 buildmaterialinfos för att göra kortare parameterlistor
            Doremi::Core::ButtonMaterials t_buttonMaterialsMiddle;

            // Load materials for the actual square to show resoluton
            t_buttonMaterialsMiddle.m_vanillaMaterial = t_meshManager.BuildMaterialInfo("NormalServer.dds");
            t_buttonMaterialsMiddle.m_highLightedMaterial = t_meshManager.BuildMaterialInfo("HoverServer.dds");
            t_buttonMaterialsMiddle.m_selectedLightedMaterial = nullptr;

            DoremiEngine::Graphic::SpriteInfo* t_spriteInfoMiddleButton = t_meshManager.BuildSpriteInfo(t_data);

            m_monitorButton = Button(t_buttonMaterialsMiddle, t_spriteInfoMiddleButton, DoremiButtonActions::MONITOR_PRESS);

            m_optionsButtons.push_back(&m_monitorButton);


            // Create the overlaying text
            DoremiEngine::Graphic::MaterialInfo* t_matinfoText = t_meshManager.BuildMaterialInfo("FontNormal.dds");

            XMFLOAT2 t_tableCharSize = XMFLOAT2(0.0625f, 0.049f);
            m_monitorText = Text(t_matinfoText, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(0.5f, 0.4f), XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));
            m_monitorText.SetText(p_sharedContext, std::to_string(m_currentMonitor));

            m_text.push_back(&m_monitorText);
        }

        void OptionsHandler::Update(double p_dt)
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

            // Check dropdowns first
            size_t length = m_dropdownResolution.size();
            for(size_t i = 0; i < length; i++)
            {
                if(m_dropdownResolution[i].button.CheckIfInside(t_mouseScreenPosX, t_mouseScreenPosY))
                {
                    // Since stupied design of check inside also update texture we check if nothing is highlighted here
                    if(m_highlightedButton == nullptr)
                    {
                        m_highlightedButton = &m_dropdownResolution[i].button;
                        m_highlightedWidth = m_dropdownResolution[i].width;
                        m_highlightedHegiht = m_dropdownResolution[i].height;
                    }
                }
            }

            // Check dropdowns first
            length = m_dropdownRefresh.size();
            for(size_t i = 0; i < length; i++)
            {
                if(m_dropdownRefresh[i].button.CheckIfInside(t_mouseScreenPosX, t_mouseScreenPosY))
                {
                    // Since stupied design of check inside also update texture we check if nothing is highlighted here
                    if(m_highlightedButton == nullptr)
                    {
                        m_highlightedButton = &m_dropdownRefresh[i].button;
                        m_highlightedRefreshrate = m_dropdownRefresh[i].refreshRate;
                    }
                }
            }

            // Check dropdowns first
            length = m_dropdownMonitors.size();
            for(size_t i = 0; i < length; i++)
            {
                if(m_dropdownMonitors[i].button.CheckIfInside(t_mouseScreenPosX, t_mouseScreenPosY))
                {
                    // Since stupied design of check inside also update texture we check if nothing is highlighted here
                    if(m_highlightedButton == nullptr)
                    {
                        m_highlightedButton = &m_dropdownMonitors[i].button;
                        m_highlightedMonitorindex = m_dropdownMonitors[i].monitorIndex;
                    }
                }
            }


            // Check normal buttons later
            length = m_optionsButtons.size();
            for(size_t i = 0; i < length; i++)
            {
                if(m_optionsButtons[i]->CheckIfInside(t_mouseScreenPosX, t_mouseScreenPosY))
                {
                    // Since stupied design of check inside also update texture we check if nothing is highlighted here
                    if(m_highlightedButton == nullptr)
                    {
                        m_highlightedButton = m_optionsButtons[i];
                    }
                }
            }


            // Check if we're pressing
            if(t_inputHandler->CheckForOnePress((int)UserCommandPlaying::LeftClick))
            {
                if(m_highlightedButton != nullptr)
                {
                    switch(m_highlightedButton->m_buttonAction)
                    {
                        case DoremiButtonActions::SET_FULLSCREEN:
                        {
                            break;
                        }
                        case DoremiButtonActions::RESOLUTION_PRESS:
                        {
                            // Swap resolution dropdown
                            t_resolutionDropdownIsActive = !t_resolutionDropdownIsActive;
                            t_refreshDropdownIsActive = false;
                            t_monitorDropdownIsActive = false;
                            ClearRefreshDropDown();
                            ClearMonitorDropDown();

                            // Set all other dropdowns false

                            // If we should show them now, create them
                            if(t_resolutionDropdownIsActive)
                            {
                                CreateResolutionDropDown();
                            }
                            else // else remove them
                            {
                                ClearResolutionDropDown();
                            }

                            break;
                        }
                        case DoremiButtonActions::RESOLUTION_SELECT:
                        {
                            m_currentResolutionHeight = m_highlightedHegiht;
                            m_currentResolutionWidth = m_highlightedWidth;
                            m_resolutionText.UpdateText(m_sharedContext, std::to_string(m_highlightedWidth) + "x" + std::to_string(m_highlightedHegiht));

                            t_resolutionDropdownIsActive = false;
                            ClearResolutionDropDown();
                            UpdateRefreshRate();

                            break;
                        }
                        case DoremiButtonActions::REFRESH_PRESS:
                        {
                            // Swap resolution dropdown
                            t_refreshDropdownIsActive = !t_refreshDropdownIsActive;
                            t_resolutionDropdownIsActive = false;
                            t_monitorDropdownIsActive = false;
                            ClearResolutionDropDown();
                            ClearMonitorDropDown();

                            // Set all other dropdowns false

                            // If we should show them now, create them
                            if(t_refreshDropdownIsActive)
                            {
                                CreateRefreshRateDropDown();
                            }
                            else // else remove them
                            {
                                ClearRefreshDropDown();
                            }

                            break;
                        }
                        case DoremiButtonActions::REFRESH_SELECT:
                        {
                            m_currentRefreshRate = m_highlightedRefreshrate;
                            m_refreshText.UpdateText(m_sharedContext, std::to_string(m_highlightedRefreshrate));

                            t_refreshDropdownIsActive = false;
                            ClearRefreshDropDown();

                            break;
                        }
                        case DoremiButtonActions::MONITOR_PRESS:
                        {
                            // Swap
                            t_monitorDropdownIsActive = !t_monitorDropdownIsActive;
                            t_resolutionDropdownIsActive = false;
                            t_refreshDropdownIsActive = false;
                            ClearResolutionDropDown();
                            ClearResolutionDropDown();

                            // If we should show them now, create them
                            if(t_monitorDropdownIsActive)
                            {
                                CreateMonitorDropDown();
                            }
                            else // else remove them
                            {
                                ClearMonitorDropDown();
                            }

                            break;
                        }
                        case DoremiButtonActions::MONITOR_SELECT:
                        {
                            m_currentMonitor = m_highlightedMonitorindex;
                            m_monitorText.UpdateText(m_sharedContext, std::to_string(m_currentMonitor));

                            t_monitorDropdownIsActive = false;
                            ClearMonitorDropDown();
                            UpdateResolution();
                            UpdateRefreshRate();

                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
                else
                {
                    // Deselect dropdowns
                }
            }
            else
            {
                // Nothing
            }

            // Check to exit TODO move place of code
            if(t_inputHandler->CheckForOnePress(static_cast<uint32_t>(UserCommandPlaying::ExitGame)))
            {
                ChangeMenuState* t_newEvent = new ChangeMenuState();
                t_newEvent->state = DoremiGameStates::MAINMENU;


                EventHandler::GetInstance()->BroadcastEvent(t_newEvent);

                ClearMonitorDropDown();
                ClearRefreshDropDown();
                ClearResolutionDropDown();
                t_monitorDropdownIsActive = false;
                t_refreshDropdownIsActive = false;
                t_refreshDropdownIsActive = false;
            }
        }

        void OptionsHandler::CreateResolutionDropDown()
        {
            DoremiEngine::Graphic::DirectXManager& t_directxManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager();
            DoremiEngine::Graphic::MeshManager& t_meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Get resolutions
            std::vector<std::pair<uint32_t, uint32_t>> t_resolutions = t_directxManager.GetResolutions(m_currentMonitor);

            // Basic position
            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.05f, 0.015f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.5f, 0.2f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);

            DoremiEngine::Graphic::MaterialInfo* t_materialNormal = t_meshManager.BuildMaterialInfo("NormalServer.dds");
            DoremiEngine::Graphic::MaterialInfo* t_materialHover = t_meshManager.BuildMaterialInfo("HoverServer.dds");
            DoremiEngine::Graphic::MaterialInfo* t_matinfoText = t_meshManager.BuildMaterialInfo("FontNormal.dds");

            XMFLOAT2 t_startOffset = XMFLOAT2(0.5f, 0.2f);
            float t_offset = 0.0f;
            auto t_reverseRes = t_resolutions.rbegin();
            for(; t_reverseRes != t_resolutions.rend(); ++t_reverseRes)
            {
                t_offset += 1.0f;
                t_data.position = XMFLOAT2(t_startOffset.x, t_startOffset.y + 0.015f * 2.0f * t_offset);

                // Skapa en buttonmaterial struct. Denna håller 2 buildmaterialinfos för att göra kortare parameterlistor
                Doremi::Core::ButtonMaterials t_buttonMaterialsMiddle;

                // Load materials for the actual square to show resoluton
                t_buttonMaterialsMiddle.m_vanillaMaterial = t_materialNormal;
                t_buttonMaterialsMiddle.m_highLightedMaterial = t_materialHover;
                t_buttonMaterialsMiddle.m_selectedLightedMaterial = nullptr;

                DoremiEngine::Graphic::SpriteInfo* t_spriteInfoMiddleButton = t_meshManager.BuildSpriteInfo(t_data);

                Button m_resolutionButton = Button(t_buttonMaterialsMiddle, t_spriteInfoMiddleButton, DoremiButtonActions::RESOLUTION_SELECT);


                XMFLOAT2 t_tableCharSize = XMFLOAT2(0.0625f, 0.049f);
                Text m_resolutionText = Text(t_matinfoText, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(t_startOffset.x, t_startOffset.y + 0.015f * 2.0f * t_offset),
                                             XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));

                m_resolutionText.SetText(m_sharedContext, std::to_string(t_reverseRes->first) + "x" + std::to_string(t_reverseRes->second));

                DropDownResolution t_newDropDownItem;
                t_newDropDownItem.button = m_resolutionButton;
                t_newDropDownItem.text = m_resolutionText;
                t_newDropDownItem.width = t_reverseRes->first;
                t_newDropDownItem.height = t_reverseRes->second;

                m_dropdownResolution.push_back(t_newDropDownItem);
            }
        }

        void OptionsHandler::CreateRefreshRateDropDown()
        {
            DoremiEngine::Graphic::DirectXManager& t_directxManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager();
            DoremiEngine::Graphic::MeshManager& t_meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Create resolution pair
            std::pair<uint32_t, uint32_t> t_resolution;
            t_resolution.first = m_currentResolutionWidth;
            t_resolution.second = m_currentResolutionHeight;

            // Get resolutions
            std::vector<uint32_t> t_refreshRates = t_directxManager.GetRefreshRates(m_currentMonitor, t_resolution);

            // Basic position
            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.05f, 0.015f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.5f, 0.3f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);

            DoremiEngine::Graphic::MaterialInfo* t_materialNormal = t_meshManager.BuildMaterialInfo("NormalServer.dds");
            DoremiEngine::Graphic::MaterialInfo* t_materialHover = t_meshManager.BuildMaterialInfo("HoverServer.dds");
            DoremiEngine::Graphic::MaterialInfo* t_matinfoText = t_meshManager.BuildMaterialInfo("FontNormal.dds");

            XMFLOAT2 t_startOffset = XMFLOAT2(0.5f, 0.3f);

            float t_offset = 0.0f;
            auto t_reverseRefresh = t_refreshRates.begin();
            for(; t_reverseRefresh != t_refreshRates.end(); ++t_reverseRefresh)
            {
                t_offset += 1.0f;
                t_data.position = XMFLOAT2(t_startOffset.x, t_startOffset.y + 0.015f * 2.0f * t_offset);

                // Skapa en buttonmaterial struct. Denna håller 2 buildmaterialinfos för att göra kortare parameterlistor
                Doremi::Core::ButtonMaterials t_buttonMaterialsMiddle;

                // Load materials for the actual square to show resoluton
                t_buttonMaterialsMiddle.m_vanillaMaterial = t_materialNormal;
                t_buttonMaterialsMiddle.m_highLightedMaterial = t_materialHover;
                t_buttonMaterialsMiddle.m_selectedLightedMaterial = nullptr;

                DoremiEngine::Graphic::SpriteInfo* t_spriteInfoMiddleButton = t_meshManager.BuildSpriteInfo(t_data);

                Button m_resolutionButton = Button(t_buttonMaterialsMiddle, t_spriteInfoMiddleButton, DoremiButtonActions::REFRESH_SELECT);


                XMFLOAT2 t_tableCharSize = XMFLOAT2(0.0625f, 0.049f);
                Text m_resolutionText = Text(t_matinfoText, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(t_startOffset.x, t_startOffset.y + 0.015f * 2.0f * t_offset),
                                             XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));

                m_resolutionText.SetText(m_sharedContext, std::to_string(*t_reverseRefresh));

                DropDownRefreshRate t_newDropDownItem;
                t_newDropDownItem.button = m_resolutionButton;
                t_newDropDownItem.text = m_resolutionText;
                t_newDropDownItem.refreshRate = *t_reverseRefresh;


                m_dropdownRefresh.push_back(t_newDropDownItem);
            }
        }

        void OptionsHandler::ClearResolutionDropDown()
        {
            for(auto& t_dropdownItem : m_dropdownResolution)
            {
                delete t_dropdownItem.button.m_spriteInfo;
                t_dropdownItem.text.DeleteText();
            }

            m_dropdownResolution.clear();
        }

        void OptionsHandler::UpdateResolution()
        {
            DoremiEngine::Graphic::DirectXManager& t_directxManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager();

            std::vector<std::pair<uint32_t, uint32_t>> t_resolutions = t_directxManager.GetResolutions(m_currentMonitor);

            std::pair<uint32_t, uint32_t> t_resolution;
            t_resolution.first = m_currentResolutionWidth;
            t_resolution.second = m_currentResolutionHeight;

            auto iterator = std::find(t_resolutions.begin(), t_resolutions.end(), t_resolution);
            if(iterator == t_resolutions.end())
            {
                if(t_resolutions.size())
                {
                    m_currentResolutionWidth = (*t_resolutions.rbegin()).first;
                    m_currentResolutionHeight = (*t_resolutions.rbegin()).second;
                    m_resolutionText.UpdateText(m_sharedContext, std::to_string(m_currentResolutionWidth) + "x" + std::to_string(m_currentResolutionHeight));
                }
            }
        }

        void OptionsHandler::UpdateRefreshRate()
        {
            DoremiEngine::Graphic::DirectXManager& t_directxManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager();

            std::pair<uint32_t, uint32_t> t_resolution;
            t_resolution.first = m_currentResolutionWidth;
            t_resolution.second = m_currentResolutionHeight;

            std::vector<uint32_t> t_refreshRates = t_directxManager.GetRefreshRates(m_currentMonitor, t_resolution);

            auto iterator = std::find(t_refreshRates.begin(), t_refreshRates.end(), m_currentRefreshRate);
            if(iterator == t_refreshRates.end())
            {
                if(t_refreshRates.size())
                {
                    m_currentRefreshRate = *(t_refreshRates.begin());
                    m_refreshText.UpdateText(m_sharedContext, std::to_string(m_currentRefreshRate));
                }
            }
        }

        void OptionsHandler::ClearRefreshDropDown()
        {
            for(auto& t_dropdownItem : m_dropdownRefresh)
            {
                delete t_dropdownItem.button.m_spriteInfo;
                t_dropdownItem.text.DeleteText();
            }

            m_dropdownRefresh.clear();
        }

        void OptionsHandler::CreateMonitorDropDown()
        {
            DoremiEngine::Graphic::DirectXManager& t_directxManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager();
            DoremiEngine::Graphic::MeshManager& t_meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Create resolution pair
            std::pair<uint32_t, uint32_t> t_resolution;
            t_resolution.first = m_currentResolutionWidth;
            t_resolution.second = m_currentResolutionHeight;

            // Get resolutions
            uint32_t t_numOfMonitors = t_directxManager.GetNumberOfMonitors();

            // Basic position
            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.05f, 0.015f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.5f, 0.4f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);

            DoremiEngine::Graphic::MaterialInfo* t_materialNormal = t_meshManager.BuildMaterialInfo("NormalServer.dds");
            DoremiEngine::Graphic::MaterialInfo* t_materialHover = t_meshManager.BuildMaterialInfo("HoverServer.dds");
            DoremiEngine::Graphic::MaterialInfo* t_matinfoText = t_meshManager.BuildMaterialInfo("FontNormal.dds");

            XMFLOAT2 t_startOffset = XMFLOAT2(0.5f, 0.4f);

            float t_offset = 0.0f;

            for(int i = 0; i < t_numOfMonitors; ++i)
            {
                t_offset += 1.0f;
                t_data.position = XMFLOAT2(t_startOffset.x, t_startOffset.y + 0.015f * 2.0f * t_offset);

                // Skapa en buttonmaterial struct. Denna håller 2 buildmaterialinfos för att göra kortare parameterlistor
                Doremi::Core::ButtonMaterials t_buttonMaterialsMiddle;

                // Load materials for the actual square to show resoluton
                t_buttonMaterialsMiddle.m_vanillaMaterial = t_materialNormal;
                t_buttonMaterialsMiddle.m_highLightedMaterial = t_materialHover;
                t_buttonMaterialsMiddle.m_selectedLightedMaterial = nullptr;

                DoremiEngine::Graphic::SpriteInfo* t_spriteInfoMiddleButton = t_meshManager.BuildSpriteInfo(t_data);

                Button m_resolutionButton = Button(t_buttonMaterialsMiddle, t_spriteInfoMiddleButton, DoremiButtonActions::MONITOR_SELECT);


                XMFLOAT2 t_tableCharSize = XMFLOAT2(0.0625f, 0.049f);
                Text m_resolutionText = Text(t_matinfoText, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(t_startOffset.x, t_startOffset.y + 0.015f * 2.0f * t_offset),
                                             XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));

                m_resolutionText.SetText(m_sharedContext, std::to_string(i));

                DropDownMonitor t_newDropDownItem;
                t_newDropDownItem.button = m_resolutionButton;
                t_newDropDownItem.text = m_resolutionText;
                t_newDropDownItem.monitorIndex = i;


                m_dropdownMonitors.push_back(t_newDropDownItem);
            }
        }

        void OptionsHandler::ClearMonitorDropDown()
        {
            for(auto& t_dropdownItem : m_dropdownMonitors)
            {
                delete t_dropdownItem.button.m_spriteInfo;
                t_dropdownItem.text.DeleteText();
            }

            m_dropdownMonitors.clear();
        }
    }
}