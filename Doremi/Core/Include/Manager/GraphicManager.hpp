#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>

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
        class GraphicManager : public Manager, public Subscriber
        {
        public:
            GraphicManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~GraphicManager();

            /**
            Walks thorugh every entity and tells the engine to render every entity with a render component
            */
            void Update(double p_dt) override;
            /**
            TODOKO check is we need any event and write doc, could be needed for particles
            */
            void OnEvent(Event* p_event) override;

        private:
            DoremiEngine::Graphic::PixelShader* m_pixelShader;
            DoremiEngine::Graphic::VertexShader* m_vertexShader;
            DoremiEngine::Graphic::DepthStencilState* m_depthStencilState;
            DoremiEngine::Graphic::RasterizerState* m_rasterizerState;
        };
    }
}
