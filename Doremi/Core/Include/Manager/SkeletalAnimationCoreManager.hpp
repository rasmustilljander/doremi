#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>

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

        class SkeletalAnimationCoreManager : public Manager
        {
        public:
            SkeletalAnimationCoreManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~SkeletalAnimationCoreManager();
            void Update(double p_dt) override;
            /**
                Updates the timer on every aminmation and updates every meshs transforms
            */
        private:
            DoremiEngine::Graphic::SkeletalAnimationImpl* m_animator;
            DoremiEngine::Graphic::PixelShader* m_pixelShader;
            DoremiEngine::Graphic::VertexShader* m_vertexShader;
            DoremiEngine::Graphic::DepthStencilState* m_depthStencilState;
            DoremiEngine::Graphic::RasterizerState* m_rasterizerState;
        };
    }
}
