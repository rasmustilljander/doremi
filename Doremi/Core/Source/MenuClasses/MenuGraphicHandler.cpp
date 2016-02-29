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
#include <DoremiEngine/Graphic/Include/Interface/State/DepthStencilState.hpp>
#include <DoremiEngine/Graphic/Include/Interface/State/RasterizerState.hpp>
#include <dxgi.h>
#include <d3d11_1.h>

namespace Doremi
{
    namespace Core
    {
        MenuGraphicHandler::MenuGraphicHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

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
    }
}