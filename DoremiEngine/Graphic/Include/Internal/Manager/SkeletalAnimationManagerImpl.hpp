#pragma once
#include <Interface/Manager/SkeletalAnimationManager.hpp>
#include <d3d11_1.h>

namespace DoremiEngine
{

    namespace Graphic
    {
        class SkeletalInformationImpl;
        struct GraphicModuleContext;
        class SkeletalAnimationManagerImpl : public SkeletalAnimationManager
        {
        public:
            SkeletalAnimationManagerImpl(const GraphicModuleContext& p_graphicContext);
            virtual ~SkeletalAnimationManagerImpl();
            void Initialize();
            SkeletalInformation* CreateSkeletalInformation() override;
            void GetInterpolatedAnimationsMatrices(SkeletalInformation* p_skeletalInformation, std::string t_clipName, float t_timePos,
                                                   std::vector<DirectX::XMFLOAT4X4>& p_finalTransforms,
                                                   std::map<std::string, float> p_animationTransitions) const override;
            void GetFinalTransforms(const std::string& p_clipName, float t_timePos, std::vector<DirectX::XMFLOAT4X4>& p_finalTransforms,
                                    SkeletalInformation* p_skeletalInformation) const override;
            void PushMatricesToDevice(std::vector<DirectX::XMFLOAT4X4> p_transformsTopush) override;


        private:
            struct InterpolatedVectors
            {
                std::vector<DirectX::XMFLOAT3> position;
                std::vector<DirectX::XMFLOAT3> scale;
                std::vector<DirectX::XMFLOAT4> rotation;
                float timeElapsed;
            };
            void InterpolateBlending(std::vector<DirectX::XMFLOAT4X4>& o_finalTransforms, std::vector<InterpolatedVectors> p_interpolatedVector) const;
            const GraphicModuleContext& m_graphicContext;
            ID3D11Buffer* m_matricesBuffer;
        };
    }
}