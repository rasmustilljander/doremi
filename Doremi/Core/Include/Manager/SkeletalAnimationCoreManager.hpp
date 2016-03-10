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

namespace DirectX
{
    struct XMFLOAT3;
}

namespace Doremi
{
    namespace Core
    {
        struct SkeletalAnimationComponent;
        struct LowerSkeletalAnimationComponent;
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

            void UpdatePlayer(double p_dt, SkeletalAnimationComponent* t_skeletalAnimationComponent,
                              LowerSkeletalAnimationComponent* t_lowerSkeletalAnimationComponent, const size_t& j);

            void UpdateEnemy(double p_dt, SkeletalAnimationComponent* t_skeletalAnimationComponent,
                             LowerSkeletalAnimationComponent* t_lowerSkeletalAnimationComponent, const size_t& j);

        private:
            DoremiEngine::Graphic::PixelShader* m_pixelShader;
            DoremiEngine::Graphic::VertexShader* m_vertexShader;
            DoremiEngine::Graphic::DepthStencilState* m_depthStencilState;
            DoremiEngine::Graphic::RasterizerState* m_rasterizerState;
            int CheckPlatformMovementAndRunPlayer(DirectX::XMFLOAT3 p_position, float p_epsilon, float p_movementLenghtVector, int p_bodyHit);
            void CheckANDPerformAnimationTransitionPlayer(const size_t& p_entityID);
            void CheckANDPerformAnimationTransitionEnemy(const size_t& p_entityID);
            /**
                Updates the transitiontimers and switches animaitonclip
            */
            void UpdateTransitionsPlayer(SkeletalAnimationComponent& p_skeltalAnimationComponent,
                                         LowerSkeletalAnimationComponent& p_lowerSkeletalAnimationComponent, double p_dt);
            void UpdateTransitionsEnemy(SkeletalAnimationComponent& p_skeletalAnimationComponent,
                                        LowerSkeletalAnimationComponent& p_lowerSkeletalAnimationComponent, double p_dt);
            void STimer(const std::string& p_animationName, SkeletalAnimationComponent* o_skeletalAnimationComponent, float p_startTimer);
            void STimer(const std::string& p_animationName, LowerSkeletalAnimationComponent* o_skeletalAnimationComponent, float p_startTimer);
            std::string CheckForTransitions(float& o_lowestTimeElapsed, SkeletalAnimationComponent* o_skeletalAnimationComponent);
            std::string CheckForTransitions(float& o_lowestTimeElapsed, LowerSkeletalAnimationComponent* o_skeletalAnimationComponent);
            double m_animationTransitionTime;
        };
    }
}
