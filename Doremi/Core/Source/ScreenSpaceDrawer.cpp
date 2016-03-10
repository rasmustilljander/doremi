/// Project
#include <ScreenSpaceDrawer.hpp>
#include <Handler/StateHandler.hpp>
#include <MenuClasses/MainMenuHandler.hpp>
/// Engine
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Graphic
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/ShaderManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/DepthStencilState.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/RasterizerState.hpp>
#include <Doremi/Core/Include/MenuClasses/VictoryScreen.hpp>
#include <Doremi/Core/Include/MenuClasses/ScreenObject.hpp>
#include <Doremi/Core/Include/MenuClasses/ServerBrowserHandler.hpp>
#include <Doremi/Core/Include/MenuClasses/HUDHandler.hpp>
#include <Doremi/Core/Include/MenuClasses/LoadingScreenHandler.hpp>
#include <Doremi/Core/Include/MenuClasses/OptionsHandler.hpp>
#include <iostream>


// DirectX
#include <dxgi.h> // TODOXX booring to include directx thingies outside module
#include <d3d11_1.h>
namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;

        ScreenSpaceDrawer* ScreenSpaceDrawer::m_singleton = nullptr;

        ScreenSpaceDrawer* ScreenSpaceDrawer::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("GetInstance called before startupScreenSpaceDrawer");
            }
            return m_singleton;
        }

        void ScreenSpaceDrawer::StartupScreenSpaceDrawer(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("startupScreenSpaceDrawer called multiple times");
            }
            m_singleton = new ScreenSpaceDrawer(p_sharedContext);
        }

        ScreenSpaceDrawer::ScreenSpaceDrawer(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            // Initialize shader thingies
            m_menuPixelShader = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildPixelShader("TextPixelShader.hlsl");
            // D3D11_INPUT_ELEMENT_DESC ied[] = {
            //    {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            //    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            //};


            // m_menuVertexShader =
            //    m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildVertexShader("TextVertexShader.hlsl", ied,
            //    ARRAYSIZE(ied));

            m_menuVertexShader =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildVertexShaderWithoutInput("TextVertexShader.hlsl");

            m_menuGeometryShader =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildGeometryShader("TextGeometryShader.hlsl");


            m_postPixelShader = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildPixelShader("PostPixelShader.hlsl");
            D3D11_INPUT_ELEMENT_DESC ied[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };
            m_basicVertexShader =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildVertexShader("BasicVertexShader.hlsl", ied, ARRAYSIZE(ied));

            DoremiEngine::Graphic::DirectXManager& t_dierctxManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager();

            // Create DepthStencilState
            D3D11_DEPTH_STENCIL_DESC t_depthStencilStateDesc;
            ZeroMemory(&t_depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
            t_depthStencilStateDesc.DepthEnable = false;
            t_depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            t_depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_NEVER;

            m_depthStencilStatNone = t_dierctxManager.CreateDepthStencilState(t_depthStencilStateDesc);
        }


        ScreenSpaceDrawer::~ScreenSpaceDrawer() {}
        void ScreenSpaceDrawer::Draw()
        {
            DoremiGameStates t_state = StateHandler::GetInstance()->GetState();
            switch(t_state)
            {
                case Doremi::Core::DoremiGameStates::MAINMENU:
                {
                    DrawMainMenu();
                    break;
                }
                case Doremi::Core::DoremiGameStates::SERVER_BROWSER:
                {
                    DrawServerBrowser();
                    break;
                }
                case Doremi::Core::DoremiGameStates::OPTIONS:
                {
                    DrawOptions();
                    break;
                }
                case Doremi::Core::DoremiGameStates::LOADING:
                {
                    DrawLoadingScreen();
                    break;
                }
                case Doremi::Core::DoremiGameStates::RUNGAME:
                {
                    DrawHUD();
                    break;
                }
                case Doremi::Core::DoremiGameStates::PAUSE:
                {
                    break;
                }
                case Doremi::Core::DoremiGameStates::VICTORY:
                {
                    DrawVictoryScreen();
                    break;
                }
                case Doremi::Core::DoremiGameStates::EXIT:
                {
                    break;
                }
                default:
                {
                    break;
                }
            }
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActivePixelShader(m_postPixelShader);
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActiveVertexShader(m_basicVertexShader);
        }

        void ScreenSpaceDrawer::Begin2DDraw()
        {
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActivePixelShader(m_menuPixelShader);
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActiveVertexShader(m_menuVertexShader);
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActiveGeometryShader(m_menuGeometryShader);

            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().EnableTextBlend();
        }

        void ScreenSpaceDrawer::End2DDraw()
        {
            DoremiEngine::Graphic::DirectXManager& t_dierctxManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager();

            // Set rasteriser to defau,t
            DoremiEngine::Graphic::RasterizerState* t_rasterizer = t_dierctxManager.GetDefaultRasterizerState();

            // Draw the sprites
            t_dierctxManager.RenderSprites(t_rasterizer->GetRasterizerState(), m_depthStencilStatNone->GetDepthStencilState());

            // Disable blend again?
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().DisableBlend();
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().RemoveGeometryShader();

            t_dierctxManager.RenderSprites(t_rasterizer->GetRasterizerState(), t_dierctxManager.GetDefaultDepthStencilState()->GetDepthStencilState());
        }


        void ScreenSpaceDrawer::DrawVictoryScreen()
        {
            Begin2DDraw();
            DoremiEngine::Graphic::MeshManager& t_meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();


            // Get Screenobjects to draw
            std::vector<ScreenObject*>& t_objectsToDraw = LoadingScreenHandler::GetInstance()->GetScreenObjects();

            // For each button add to render list
            for(auto& t_object : t_objectsToDraw)
            {
                t_meshManager.AddSpriteToRenderList(*(t_object->m_spriteInfo), *(t_object->m_materialInfo));
            }
            End2DDraw();
        }

        void ScreenSpaceDrawer::DrawMainMenu()
        {
            Begin2DDraw();

            DoremiEngine::Graphic::MeshManager& t_meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            std::vector<ScreenObject*>& t_objectsToDraw = MainMenuHandler::GetInstance()->GetScreenObjects();

            // For each button add to render list
            for(auto& t_object : t_objectsToDraw)
            {
                t_meshManager.AddSpriteToRenderList(*(t_object->m_spriteInfo), *(t_object->m_materialInfo));
            }

            // Get buttons to draw
            std::vector<Button> t_buttonsToDraw = MainMenuHandler::GetInstance()->GetButtons();

            // For each button we add to render list
            size_t length = t_buttonsToDraw.size();
            for(size_t i = 0; i < length; i++)
            {
                t_meshManager.AddSpriteToRenderList(*t_buttonsToDraw[i].m_spriteInfo, *t_buttonsToDraw[i].m_materialInfo);
            }


            End2DDraw();
        }

        void ScreenSpaceDrawer::DrawServerBrowser()
        {
            Begin2DDraw();

            DoremiEngine::Graphic::MeshManager& t_meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Get buttons to draw
            std::list<Button*> t_buttonsToDraw = ServerBrowserHandler::GetInstance()->GetButtons();

            // For each button add to render list
            for(auto& t_button : t_buttonsToDraw)
            {
                t_meshManager.AddSpriteToRenderList(*(t_button->m_spriteInfo), *(t_button->m_materialInfo));
            }

            // Get text to draw
            std::list<Text*> t_TextToDraw = ServerBrowserHandler::GetInstance()->GetText();

            // For each text add to render list
            for(auto& t_text : t_TextToDraw)
            {
                for(auto& t_textPart : t_text->m_textInfo)
                {
                    t_meshManager.AddSpriteToRenderList(*(t_textPart), *(t_text->m_textMaterial));
                }
            }

            End2DDraw();
        }

        void ScreenSpaceDrawer::DrawOptions()
        {
            Begin2DDraw();

            DoremiEngine::Graphic::MeshManager& t_meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Get screenspace objects
            // Get Screenobjects to draw
            std::vector<ScreenObject*>& t_objectsToDraw = OptionsHandler::GetInstance()->GetScreenObjects();

            // For each button add to render list
            for(auto& t_object : t_objectsToDraw)
            {
                t_meshManager.AddSpriteToRenderList(*(t_object->m_spriteInfo), *(t_object->m_materialInfo));
            }

            Button* m_highlightedButton = OptionsHandler::GetInstance()->GetHighlightButton();
            Text* m_highlightedText = nullptr;

            // Get buttons to draw
            std::vector<Button*> t_buttonsToDraw = OptionsHandler::GetInstance()->GetButtons();

            // For each button add to render list
            for(auto& t_button : t_buttonsToDraw)
            {
                if(m_highlightedButton != t_button)
                {
                    t_meshManager.AddSpriteToRenderList(*(t_button->m_spriteInfo), *(t_button->m_materialInfo));
                }
            }

            // Get buttons to draw
            std::vector<MenuItemBasic*> t_itemsToDraw = OptionsHandler::GetInstance()->GetBasicMenuItems();

            // For each button add to render list
            for(auto& t_item : t_itemsToDraw)
            {
                if(m_highlightedButton != &t_item->button)
                {
                    t_meshManager.AddSpriteToRenderList(*(t_item->button.m_spriteInfo), *(t_item->button.m_materialInfo));

                    for(auto& t_textPart : t_item->text.m_textInfo)
                    {
                        t_meshManager.AddSpriteToRenderList(*(t_textPart), *(t_item->text.m_textMaterial));
                    }
                }
                else
                {
                    m_highlightedText = &t_item->text;
                }
            }


            // Get text to draw
            std::vector<Text*>& t_TextToDraw = OptionsHandler::GetInstance()->GetText();

            // For each text add to render list
            for(auto& t_text : t_TextToDraw)
            {
                for(auto& t_textPart : t_text->m_textInfo)
                {
                    t_meshManager.AddSpriteToRenderList(*(t_textPart), *(t_text->m_textMaterial));
                }
            }

            // Draw sliders
            std::vector<Slider*>& t_sliders = OptionsHandler::GetInstance()->GetSliders();

            // For each text add to render list
            for(auto& t_slide : t_sliders)
            {
                t_meshManager.AddSpriteToRenderList(*(t_slide->m_spriteInfoBack), *(t_slide->m_materialInfoBack));
                t_meshManager.AddSpriteToRenderList(*(t_slide->m_spriteInfoCircle), *(t_slide->m_materialInfoCircle));
            }


            // Draw drop downs LAAAST
            // Get buttons to draw
            std::vector<DropDownResolution>& t_dropDownResolution = OptionsHandler::GetInstance()->GetDropDownResolutions();

            // For each button add to render list
            for(auto& t_item : t_dropDownResolution)
            {
                if(m_highlightedButton != &t_item.button)
                {
                    t_meshManager.AddSpriteToRenderList(*(t_item.button.m_spriteInfo), *(t_item.button.m_materialInfo));

                    for(auto& t_textPart : t_item.text.m_textInfo)
                    {
                        t_meshManager.AddSpriteToRenderList(*(t_textPart), *(t_item.text.m_textMaterial));
                    }
                }
                else
                {
                    m_highlightedText = &t_item.text;
                }
            }

            // Get buttons to draw
            std::vector<DropDownRefreshRate>& t_dropDownRefresh = OptionsHandler::GetInstance()->GetDropDownRefresh();

            // For each button add to render list
            for(auto& t_item : t_dropDownRefresh)
            {
                if(m_highlightedButton != &t_item.button)
                {
                    t_meshManager.AddSpriteToRenderList(*(t_item.button.m_spriteInfo), *(t_item.button.m_materialInfo));

                    for(auto& t_textPart : t_item.text.m_textInfo)
                    {
                        t_meshManager.AddSpriteToRenderList(*(t_textPart), *(t_item.text.m_textMaterial));
                    }
                }
                else
                {
                    m_highlightedText = &t_item.text;
                }
            }

            // Get buttons to draw
            std::vector<DropDownMonitor>& t_dropdownMonitor = OptionsHandler::GetInstance()->GetDropDownMonitors();

            // For each button add to render list
            for(auto& t_item : t_dropdownMonitor)
            {
                if(m_highlightedButton != &t_item.button)
                {
                    t_meshManager.AddSpriteToRenderList(*(t_item.button.m_spriteInfo), *(t_item.button.m_materialInfo));

                    for(auto& t_textPart : t_item.text.m_textInfo)
                    {
                        t_meshManager.AddSpriteToRenderList(*(t_textPart), *(t_item.text.m_textMaterial));
                    }
                }
                else
                {
                    m_highlightedText = &t_item.text;
                }
            }

            if(m_highlightedButton != nullptr)
            {
                t_meshManager.AddSpriteToRenderList(*(m_highlightedButton->m_spriteInfo), *(m_highlightedButton->m_materialInfo));

                if(m_highlightedText != nullptr)
                {
                    for(auto& t_textPart : m_highlightedText->m_textInfo)
                    {
                        t_meshManager.AddSpriteToRenderList(*(t_textPart), *(m_highlightedText->m_textMaterial));
                    }
                }
            }


            End2DDraw();
        }

        void ScreenSpaceDrawer::DrawLoadingScreen()
        {
            Begin2DDraw();

            DoremiEngine::Graphic::MeshManager& t_meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Get Screenobjects to draw
            std::vector<ScreenObject*>& t_objectsToDraw = LoadingScreenHandler::GetInstance()->GetScreenObjects();

            // For each button add to render list
            for(auto& t_object : t_objectsToDraw)
            {
                t_meshManager.AddSpriteToRenderList(*(t_object->m_spriteInfo), *(t_object->m_materialInfo));
            }

            // Get bars to draw
            std::vector<Bar*>& t_barsToDraw = LoadingScreenHandler::GetInstance()->GetBars();

            // For each button add to render list
            // Remember to draw them Back->Bar->front
            for(auto& t_bar : t_barsToDraw)
            {
                if(t_bar->m_barBack.m_spriteInfo != nullptr)
                {
                    t_meshManager.AddSpriteToRenderList(*(t_bar->m_barBack.m_spriteInfo), *(t_bar->m_barBack.m_materialInfo));
                }

                t_meshManager.AddSpriteToRenderList(*(t_bar->m_barBar.m_spriteInfo), *(t_bar->m_barBar.m_materialInfo));

                if(t_bar->m_barFront.m_spriteInfo != nullptr)
                {
                    t_meshManager.AddSpriteToRenderList(*(t_bar->m_barFront.m_spriteInfo), *(t_bar->m_barFront.m_materialInfo));
                }
            }


            End2DDraw();
        }

        void ScreenSpaceDrawer::DrawHUD()
        {
            Begin2DDraw();

            DoremiEngine::Graphic::MeshManager& t_meshManager = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            // Get bars to draw
            std::vector<Bar*>& t_barsToDraw = HUDHandler::GetInstance()->GetBars();

            // For each button add to render list
            // Remember to draw them Back->Bar->front
            for(auto& t_bar : t_barsToDraw)
            {
                if(t_bar->m_barBack.m_spriteInfo != nullptr)
                {
                    t_meshManager.AddSpriteToRenderList(*(t_bar->m_barBack.m_spriteInfo), *(t_bar->m_barBack.m_materialInfo));
                }

                t_meshManager.AddSpriteToRenderList(*(t_bar->m_barBar.m_spriteInfo), *(t_bar->m_barBar.m_materialInfo));

                if(t_bar->m_barFront.m_spriteInfo != nullptr)
                {
                    t_meshManager.AddSpriteToRenderList(*(t_bar->m_barFront.m_spriteInfo), *(t_bar->m_barFront.m_materialInfo));
                }
            }

            // Get Screenobjects to draw
            std::vector<ScreenObject*>& t_objectsToDraw = HUDHandler::GetInstance()->GetScreenObjects();

            // For each button add to render list
            for(auto& t_object : t_objectsToDraw)
            {
                t_meshManager.AddSpriteToRenderList(*(t_object->m_spriteInfo), *(t_object->m_materialInfo));
            }

            End2DDraw();
        }
    }
}