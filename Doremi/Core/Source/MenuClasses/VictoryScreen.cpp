// Project specific
#include <Doremi/Core/Include/MenuClasses/VictoryScreen.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <Doremi/Core/Include/MenuClasses/ScreenObject.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/ChangeMenuState.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;
        VictoryScreen::VictoryScreen(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();
            DoremiEngine::Graphic::DirectXManager& t_dierctxManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager();

            // Create background
            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.5f, 0.5f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.5f, 0.5f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);

            DoremiEngine::Graphic::SpriteInfo* t_spriteInfo = t_meshManager.BuildSpriteInfo(t_data);
            DoremiEngine::Graphic::MaterialInfo* t_matInfo = t_meshManager.BuildMaterialInfo("VictoryScreen_v1.dds");

            m_background = ScreenObject(t_matInfo, t_spriteInfo);

            m_screenObjects.push_back(&m_background);
        }

        VictoryScreen::~VictoryScreen() {}

        VictoryScreen* VictoryScreen::m_singleton = nullptr;

        void VictoryScreen::StartupVictoryScreen(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("To tierd for  this");
            }
            m_singleton = new VictoryScreen(p_sharedContext);
        }

        VictoryScreen* VictoryScreen::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("To tierd for  this");
            }
            return m_singleton;
        }
        void VictoryScreen::Update()
        {
            InputHandlerClient* t_inputHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance())->GetInputHandler();

            if(t_inputHandler->CheckForOnePress((int)UserCommandPlaying::ExitGame))
            {
                ChangeMenuState* t_change = new ChangeMenuState();
                t_change->state = DoremiGameStates::MAINMENU;
                EventHandler::GetInstance()->BroadcastEvent(t_change);
            }
        }
    }
}