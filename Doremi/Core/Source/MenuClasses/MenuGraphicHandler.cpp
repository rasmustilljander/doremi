// Project specific
#include <Doremi/Core/Include/MenuClasses/MenuGraphicHandler.hpp>
// Engine
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/ShaderManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MeshInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <dxgi.h>
#include <d3d11_1.h>

namespace Doremi
{
    namespace Core
    {

        MenuGraphicHandler::MenuGraphicHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            m_menuPixelShader = m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildPixelShader("TextPixelShader.hlsl");
            D3D11_INPUT_ELEMENT_DESC ied[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };
            m_menuVertexShader =
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().BuildVertexShader("TextVertexShader.hlsl", ied, ARRAYSIZE(ied));
        }

        MenuGraphicHandler::~MenuGraphicHandler() {}
        MenuGraphicHandler* MenuGraphicHandler::m_singleton = nullptr;

        void MenuGraphicHandler::StartMenuGraphicHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("MenuGraphicHandler StartMenuHandler called multiple times.");
            }
            m_singleton = new MenuGraphicHandler(p_sharedContext);
        }

        MenuGraphicHandler* MenuGraphicHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("MenuGraphicHandler not initialized, GetInstance called.");
            }

            return m_singleton;
        }


        void MenuGraphicHandler::DrawButtons(double p_dt, std::vector<Button> p_buttonsToDraw)
        {
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActivePixelShader(m_menuPixelShader);
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetShaderManager().SetActiveVertexShader(m_menuVertexShader);
            size_t length = p_buttonsToDraw.size();
            for(size_t i = 0; i < length; i++)
            {
                m_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager().AddToRenderList(*p_buttonsToDraw[i].m_meshInfo,
                                                                                                          *p_buttonsToDraw[i].m_materialInfo,
                                                                                                          p_buttonsToDraw[i].m_transformMatrix);
            }
            m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().EndDraw();
        }
    }
}