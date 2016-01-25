#include <DoremiEngine/Physics/Include/Internal/RayCastManagerImpl.hpp>
#include <DoremiEngine/Physics/Include/Internal/PhysicsModuleImplementation.hpp>
namespace DoremiEngine
{
    namespace Physics
    {

        RayCastManagerImpl::RayCastManagerImpl(InternalPhysicsUtils& p_utils) : m_utils(p_utils) {}
        RayCastManagerImpl::~RayCastManagerImpl() {}
        int RayCastManagerImpl::CastRay(const DirectX::XMFLOAT3& p_origin, const DirectX::XMFLOAT3& p_direction, const float& p_range)
        {
            // Cast directx things to physx
            PxVec3 origin = PxVec3(p_origin.x, p_origin.y, p_origin.z);
            PxVec3 direction = PxVec3(p_direction.x, p_direction.y, p_direction.z);

            PxRaycastBuffer hit; // Used to save the hit
            bool status = m_utils.m_worldScene->raycast(origin, direction, p_range, hit);
            if(!status && !hit.hasBlock)
            {
                // No hit detected, return -1 TODOKO Maybee i should return something better?
                return -1;
            }
            // Start with checking static and dynamic rigid bodies
            unordered_map<PxRigidActor*, int> idsByRigidBody = m_utils.m_rigidBodyManager->GetIDsByBodies();
            if(idsByRigidBody.find(hit.block.actor) != idsByRigidBody.end())
            {
                return idsByRigidBody[hit.block.actor];
            }
            else
            {
                // Nothing
            }
            // Now comes the difficult task of checking vs character controllers
            unordered_map<PxController*, int> idsByCharacterController = m_utils.m_characterControlManager->GetIdsByControllers();
            for(auto pairs : idsByCharacterController)
            {
                if(pairs.first->getActor() == hit.block.actor)
                {
                    return pairs.second;
                }
            }
            return 0;
        }
    }
}