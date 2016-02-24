#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        class SkeletalAnimationImpl;

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
        /**
            This manager updates the skeletalanimations. It increases the timer of every animation and fetches + updates the bonetransforms.
        */

        class SkeletalAnimationCoreManager : public Manager, public Subscriber
        {
        public:
            explicit SkeletalAnimationCoreManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~SkeletalAnimationCoreManager();
            /**
            Updates the timer on every aminmation and updates every meshs transforms
            */
            void Update(double p_dt) override;
            void OnEvent(Event* p_event) override;

        private:
            DoremiEngine::Graphic::SkeletalAnimationImpl* m_animator;
            DoremiEngine::Graphic::PixelShader* m_pixelShader;
            DoremiEngine::Graphic::VertexShader* m_vertexShader;
            DoremiEngine::Graphic::DepthStencilState* m_depthStencilState;
            DoremiEngine::Graphic::RasterizerState* m_rasterizerState;
            void CheckANDPerformAnimationTransition(const size_t& p_entityID);
        };
    }
}
