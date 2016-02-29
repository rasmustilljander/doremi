#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/MenuClasses/Button.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Shader/PixelShader.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Shader/VertexShader.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Shader/GeometryShader.hpp>


namespace DoremiEngine
{
    namespace Graphic
    {
        class DepthStencilState;
        class RasterizerState;
        class PixelShader;
        class VertexShader;
    }
}
namespace Doremi
{
    namespace Core
    {
        class MenuGraphicHandler
        {
        public:
            MenuGraphicHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~MenuGraphicHandler();

            static MenuGraphicHandler* GetInstance();
            static void StartMenuGraphicHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);


        private:
            static MenuGraphicHandler* m_singleton;
            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}
