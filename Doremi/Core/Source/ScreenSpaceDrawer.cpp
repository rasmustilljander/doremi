/// Project
#include <ScreenSpaceDrawer.hpp>
#include <Handler/StateHandler.hpp>
#include <MenuClasses/MenuHandler.hpp>
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


// DirectX
#include <dxgi.h> // TODOXX booring to include directx thingies outside module
#include <d3d11_1.h>
namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;
        ScreenSpaceDrawer::ScreenSpaceDrawer(const DoremiEngine::Core::SharedContext& p_sharedContext, XMFLOAT2 p_resolution)
            : m_sharedContext(p_sharedContext), m_resolution(p_resolution)
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

            CreateVictoryScreen();
        }
        void ScreenSpaceDrawer::CreateVictoryScreen()
        {
            DoremiEngine::Graphic::MeshInfo* t_meshInfo = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildQuadMeshInfo("Quad");
            m_victoryScreen = new VictoryScreen(t_meshInfo);

            DoremiEngine::Graphic::MaterialInfo* t_materialInfo =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().BuildMaterialInfo("HappyFace.dds");
            XMFLOAT2 t_position = XMFLOAT2(0, 0);
            XMFLOAT2 t_extents = XMFLOAT2(100, 100);
            t_position = ConvertWithResolution(t_position);
            // t_extents = ConvertWithResolution(t_extents);

            m_victoryScreen->AddScreenObject(t_materialInfo, t_position, t_extents);
        }
        XMFLOAT2 ScreenSpaceDrawer::ConvertWithResolution(XMFLOAT2 p_point)
        {
            XMFLOAT2 t_newPoint;
            t_newPoint.x = (p_point.x + 1) * m_resolution.x * 0.5f;
            t_newPoint.y = (p_point.y + 1) * m_resolution.y * 0.5f;
            return t_newPoint;
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
                    // Draw options screen. TODOKO implement
                    break;
                }
                case Doremi::Core::DoremiGameStates::RUNGAME:
                {
                    // Nothing to draw, the manager will handle this. Maybe HUD though? TODOKO
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
        }

        void ScreenSpaceDrawer::Begin2DDraw()
        {
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActivePixelShader(m_menuPixelShader);
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActiveVertexShader(m_menuVertexShader);
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActiveGeometryShader(m_menuGeometryShader);

            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().EnableBlend();
        }

        void ScreenSpaceDrawer::End2DDraw()
        {

            // Disable blend again?
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().DisableBlend();
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().RemoveGeometryShader();
        }

        void ScreenSpaceDrawer::DrawVictoryScreen()
        {
            Begin2DDraw();


            std::vector<ScreenObject*> t_screenObjects = m_victoryScreen->GetScreen();
            size_t length = t_screenObjects.size();
            for(size_t i = 0; i < length; i++)
            {

                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(*t_screenObjects[i]->m_meshInfo,
                                                                                                          *t_screenObjects[i]->m_materialInfo,
                                                                                                          t_screenObjects[i]->m_transformMatrix);
            }

            DoremiEngine::Graphic::RasterizerState* t_rasterizer =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().GetDefaultRasterizerState();
            DoremiEngine::Graphic::DepthStencilState* t_depthStencil =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().GetDefaultDepthStencilState();
            /*m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().DrawCurrentRenderList(t_rasterizer->GetRasterizerState(),
                                                                                                               t_depthStencil->GetDepthStencilState());*/
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().Render2D(t_rasterizer->GetRasterizerState(),
                                                                                                  t_depthStencil->GetDepthStencilState());

            End2DDraw();
        }

        void ScreenSpaceDrawer::DrawMainMenu()
        {
            Begin2DDraw();

            // Get buttons to draw
            // std::vector<Button> t_buttonsToDraw = MenuHandler::GetInstance()->GetButtons();

            // For each button we add to render list
            /*size_t length = t_buttonsToDraw.size();
            for(size_t i = 0; i < length; i++)
            {
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(*t_buttonsToDraw[i].m_meshInfo,
                                                                                                          *t_buttonsToDraw[i].m_materialInfo,
                                                                                                          t_buttonsToDraw[i].m_transformMatrix);
            }*/

            //// Set rasteriser to defau,t
            // DoremiEngine::Graphic::RasterizerState* t_rasterizer =
            //    m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().GetDefaultRasterizerState();


            //// Set depth stencil to default
            // DoremiEngine::Graphic::DepthStencilState* t_depthStencil =
            //    m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().GetDefaultDepthStencilState();

            //// Draw the sprites
            // m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().Render2D(t_rasterizer->GetRasterizerState(),
            //                                                                                      t_depthStencil->GetDepthStencilState());


            //////////// New draw style for sprites

            // Get buttons to draw
            std::vector<Button> t_buttonsToDraw = MenuHandler::GetInstance()->GetButtons();

            // For each button we add to render list
            size_t length = t_buttonsToDraw.size();
            for(size_t i = 0; i < length; i++)
            {
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddSpriteToRenderList(*t_buttonsToDraw[i].m_spriteInfo,
                                                                                                                *t_buttonsToDraw[i].m_materialInfo);
            }

            // Set rasteriser to defau,t
            DoremiEngine::Graphic::RasterizerState* t_rasterizer =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().GetDefaultRasterizerState();

            // Set depth stencil to default
            DoremiEngine::Graphic::DepthStencilState* t_depthStencil =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().GetDefaultDepthStencilState();

            // Draw the sprites
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().RenderSprites(t_rasterizer->GetRasterizerState(),
                                                                                                       t_depthStencil->GetDepthStencilState());

            End2DDraw();
        }

        void ScreenSpaceDrawer::DrawServerBrowser()
        {
            Begin2DDraw();
            End2DDraw();
        }
    }
}