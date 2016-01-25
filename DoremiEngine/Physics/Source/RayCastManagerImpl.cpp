#include <DoremiEngine/Physics/Include/Internal/RayCastManagerImpl.hpp>
namespace DoremiEngine
{
    namespace Physics
    {

        RayCastManagerImpl::RayCastManagerImpl(InternalPhysicsUtils& p_utils) : m_utils(p_utils)
        {}
        RayCastManagerImpl::~RayCastManagerImpl()
        {}
        int RayCastManagerImpl::CastRay(const DirectX::XMFLOAT3& p_origin, const DirectX::XMFLOAT3& p_direction, const float& p_range)
        {
            return 0;
        }

    }
}