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
            : m_sharedContext(p_sharedContext), t_resolutionDropdownIsActive(false)
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
            std::vector<uint32_t> m_refreshRAtes = t_directxManager.GetRefreshRates(0, *(t_resolutions1.rend().base()));


            CreateScreenResolutionOption(p_sharedContext);
        }

        OptionsHandler::~OptionsHandler() {}

        void OptionsHandler::CreateScreenResolutionOption(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Create the square you press to get the dropdown

            // Basic position
            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.1f, 0.05f);
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

            m_middleResolutionButton = Button(t_buttonMaterialsMiddle, t_spriteInfoMiddleButton, DoremiButtonActions::RESOLUTION_PRESS);

            m_optionsButtons.push_back(&m_middleResolutionButton);


            // Create the overlaying text
            DoremiEngine::Graphic::MaterialInfo* t_matinfoText = t_meshManager.BuildMaterialInfo("FontNormal.dds");

            XMFLOAT2 t_tableCharSize = XMFLOAT2(0.0625f, 0.049f);
            m_middleResolutionText =
                Text(t_matinfoText, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(0.5f, 0.2f), XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));
            m_middleResolutionText.SetText(p_sharedContext, std::to_string(m_currentResolutionWidth) + "x" + std::to_string(m_currentResolutionHeight));

            m_text.push_back(&m_middleResolutionText);
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


            // Check normal buttons later
            size_t length = m_optionsButtons.size();
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

            t_data.halfsize = XMFLOAT2(0.1f, 0.05f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.5f, 0.2f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);

            DoremiEngine::Graphic::MaterialInfo* t_materialNormal = t_meshManager.BuildMaterialInfo("NormalServer.dds");
            DoremiEngine::Graphic::MaterialInfo* t_materialHover = t_meshManager.BuildMaterialInfo("HoverServer.dds");
            DoremiEngine::Graphic::MaterialInfo* t_matinfoText = t_meshManager.BuildMaterialInfo("FontNormal.dds");

            XMFLOAT2 t_startOffset = XMFLOAT2(0.5f, 0.2f);
            float t_offset = 0.0f;
            for(auto t_resolution : t_resolutions)
            {
                t_offset += 1.0f;
                t_data.position = XMFLOAT2(t_startOffset.x, t_startOffset.y + 0.05f * 2.0f * t_offset);

                // Skapa en buttonmaterial struct. Denna håller 2 buildmaterialinfos för att göra kortare parameterlistor
                Doremi::Core::ButtonMaterials t_buttonMaterialsMiddle;

                // Load materials for the actual square to show resoluton
                t_buttonMaterialsMiddle.m_vanillaMaterial = t_materialNormal;
                t_buttonMaterialsMiddle.m_highLightedMaterial = t_materialHover;
                t_buttonMaterialsMiddle.m_selectedLightedMaterial = nullptr;

                DoremiEngine::Graphic::SpriteInfo* t_spriteInfoMiddleButton = t_meshManager.BuildSpriteInfo(t_data);

                Button m_resolutionButton = Button(t_buttonMaterialsMiddle, t_spriteInfoMiddleButton, DoremiButtonActions::RESOLUTION_PRESS);


                XMFLOAT2 t_tableCharSize = XMFLOAT2(0.0625f, 0.049f);
                Text m_resolutionText = Text(t_matinfoText, XMFLOAT2(0.01f, 0.02f), XMFLOAT2(t_startOffset.x, t_startOffset.y + 0.05f * 2.0f * t_offset),
                                             XMFLOAT2(0.0f, 0.0f), t_tableCharSize, XMFLOAT2(0.0f, 0.0f));

                m_resolutionText.SetText(m_sharedContext, std::to_string(t_resolution.second) + "x" + std::to_string(t_resolution.first));


                m_dropdownButtons.push_back(m_resolutionButton);
                m_dropdownText.push_back(m_resolutionText);
            }
        }

        void OptionsHandler::ClearResolutionDropDown()
        {
            for(auto& t_button : m_dropdownButtons)
            {
                delete t_button.m_spriteInfo;
            }

            for(auto& t_text : m_dropdownText)
            {
                t_text.DeleteText();
            }

            m_dropdownButtons.clear();
            m_dropdownText.clear();
        }
    }
}