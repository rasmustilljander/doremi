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

            // Get current resolution
            XMFLOAT2 t_resolution = t_directxManager.GetScreenResolution();

            // Get current Monitor
            uint32_t t_currentMonitor = t_directxManager.GetCurrentMonitor();

            // Get number of monitors
            uint32_t t_numberOfMonitors = t_directxManager.GetNumberOfMonitors();

            // Get current refresh rate
            float t_refreshRate = t_directxManager.GetRefreshRate();

            // later we want to -> Get all resolutions for monitor


            CreateScreenResolutionOption(p_sharedContext);
        }

        OptionsHandler::~OptionsHandler() {}

        void OptionsHandler::CreateScreenResolutionOption(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Basic position
            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.35f, 0.1f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.5f, 0.0f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);

            // Skapa en buttonmaterial struct. Denna håller 2 buildmaterialinfos för att göra kortare parameterlistor
            Doremi::Core::ButtonMaterials t_buttonMaterialsLeft;
            Doremi::Core::ButtonMaterials t_buttonMaterialsRight;
            Doremi::Core::ButtonMaterials t_buttonMaterialsMiddle;

            // Load materials left
            t_buttonMaterialsLeft.m_vanillaMaterial = t_meshManager.BuildMaterialInfo("NormalServer.dds");
            t_buttonMaterialsLeft.m_highLightedMaterial = t_meshManager.BuildMaterialInfo("HoverServer.dds");
            t_buttonMaterialsLeft.m_selectedLightedMaterial = t_meshManager.BuildMaterialInfo("SelectedServer.dds");

            // Load materials right
            t_buttonMaterialsRight.m_vanillaMaterial = t_meshManager.BuildMaterialInfo("NormalServer.dds");
            t_buttonMaterialsRight.m_highLightedMaterial = t_meshManager.BuildMaterialInfo("HoverServer.dds");
            t_buttonMaterialsRight.m_selectedLightedMaterial = t_meshManager.BuildMaterialInfo("SelectedServer.dds");

            // Load materials for the actual square to show resoluton
            t_buttonMaterialsMiddle.m_vanillaMaterial = t_meshManager.BuildMaterialInfo("NormalServer.dds");
            t_buttonMaterialsMiddle.m_highLightedMaterial = t_meshManager.BuildMaterialInfo("HoverServer.dds");
            t_buttonMaterialsMiddle.m_selectedLightedMaterial = t_meshManager.BuildMaterialInfo("SelectedServer.dds");

            DoremiEngine::Graphic::SpriteInfo* t_spriteInfoLeft = t_meshManager.BuildSpriteInfo(t_data);
            DoremiEngine::Graphic::SpriteInfo* t_spriteInfoRight = t_meshManager.BuildSpriteInfo(t_data);
            DoremiEngine::Graphic::SpriteInfo* t_spriteInfoMiddle = t_meshManager.BuildSpriteInfo(t_data);

            m_leftResolutionButton = Button(t_buttonMaterialsLeft, t_spriteInfoLeft, DoremiButtonActions::RESOLUTION_LEFT);
            m_rightResolutionButton = Button(t_buttonMaterialsRight, t_spriteInfoRight, DoremiButtonActions::RESOLUTION_RIGHT);
            m_middleResolutionButton = Button(t_buttonMaterialsMiddle, t_spriteInfoMiddle, DoremiButtonActions::RESOLUTION_PRESS);

            m_menuButtons.push_back(&m_leftResolutionButton);
            m_menuButtons.push_back(&m_rightResolutionButton);
            m_menuButtons.push_back(&m_middleResolutionButton);
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
            size_t length = m_menuButtons.size();
            for(size_t i = 0; i < length; i++)
            {
                if(m_menuButtons[i]->CheckIfInside(t_mouseScreenPosX, t_mouseScreenPosY))
                {
                    m_highlightedButton = m_menuButtons[i];
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
                        case DoremiButtonActions::RESOLUTION_LEFT:
                        {

                            break;
                        }
                        case DoremiButtonActions::RESOLUTION_RIGHT:
                        {
                            break;
                        }
                        case DoremiButtonActions::RESOLUTION_PRESS:
                        {
                            // Swap resolution dropdown
                            t_resolutionDropdownIsActive = !t_resolutionDropdownIsActive;

                            // If we should show them now, create them
                            if(t_resolutionDropdownIsActive)
                            {
                            }
                            else // else remove them
                            {
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
    }
}