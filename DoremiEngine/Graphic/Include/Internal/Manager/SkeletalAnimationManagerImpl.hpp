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
            void GetFinalTransforms(const std::string& p_clipName, float t_timePos, std::vector<DirectX::XMFLOAT4X4>& p_finalTransforms,
                                    SkeletalInformation* p_skeletalInformation) const override;
            void PushMatricesToDevice(std::vector<DirectX::XMFLOAT4X4> p_transformsTopush) override;


        private:
            const GraphicModuleContext& m_graphicContext;
            ID3D11Buffer* m_matricesBuffer;
        };
    }
}