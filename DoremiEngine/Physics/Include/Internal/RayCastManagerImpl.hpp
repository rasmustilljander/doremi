#pragma once
#include <DoremiEngine/Physics/Include/RayCastManager.hpp>
namespace DoremiEngine
{
    namespace Physics
    {
        struct InternalPhysicsUtils;
        class RayCastManagerImpl : public RayCastManager
        {
        public:
            RayCastManagerImpl(InternalPhysicsUtils& p_utils);
            virtual ~RayCastManagerImpl();
            int CastRay(const DirectX::XMFLOAT3& p_origin, const DirectX::XMFLOAT3& p_direction, const float& p_range) override;
            int CastSweep(const XMFLOAT3& p_origin, XMFLOAT3& p_direction, float p_width, const float& p_range) override;
            std::vector<int> OverlapBoxMultipleHits(const XMFLOAT3& p_origin, const XMFLOAT3& p_halfExtents) override;

        private:
            InternalPhysicsUtils& m_utils;
        };
    }
}