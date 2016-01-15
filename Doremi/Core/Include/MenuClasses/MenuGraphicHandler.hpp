#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/MenuClasses/Button.hpp>
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
            /**
            Walks thorugh every button and tells the engine to render them using their translationmatrix matinfo and meshinfo
            */
            void DrawButtons(double p_dt, std::vector<Button> p_buttonsToDraw, int p_currentbutton);


        private:
            static MenuGraphicHandler* m_singleton;
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            DoremiEngine::Graphic::PixelShader* m_menuPixelShader;
            DoremiEngine::Graphic::VertexShader* m_menuVertexShader;
            DoremiEngine::Graphic::DepthStencilState* m_depthStencilState;
            DoremiEngine::Graphic::RasterizerState* m_rasterizerState;
        };
    }
}
