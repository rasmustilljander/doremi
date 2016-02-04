#include <DoremiEngine/Physics/Include/Internal/RayCastManagerImpl.hpp>
#include <DoremiEngine/Physics/Include/Internal/PhysicsModuleImplementation.hpp>

// DEBUG
#include <iostream>
using namespace std;
namespace DoremiEngine
{
    namespace Physics
    {

        RayCastManagerImpl::RayCastManagerImpl(InternalPhysicsUtils& p_utils) : m_utils(p_utils) {}
        RayCastManagerImpl::~RayCastManagerImpl() {}
        int RayCastManagerImpl::CastRay(const DirectX::XMFLOAT3& p_origin, const DirectX::XMFLOAT3& p_direction, const float& p_range)
        {
            if(p_range <= 0.0f)
            {
                // TODOKO log error
                return -1;
            }
            // Cast directx things to physx
            PxVec3 origin = PxVec3(p_origin.x, p_origin.y, p_origin.z);
            PxVec3 direction = PxVec3(p_direction.x, p_direction.y, p_direction.z);
            direction.normalize();
            PxRaycastBuffer hit; // Used to save the hit
            // casts a ray vs the physics scene. various hit information is saved in the hit variable. For now we only care about the object we hit
            // eMESH_BOTH_SIDES specifices that no culling should be used, shouldnt need to be there but let's be safe
            bool status = m_utils.m_worldScene->raycast(origin, direction, p_range, hit, PxHitFlag::eMESH_BOTH_SIDES);
            // hit.block.position;
            if(!status && !hit.hasBlock)
            {
                // No hit detected, return -1 TODOKO Maybee i should return something better?
                return -1;
            }

            // Start with checking static and dynamic rigid bodies
            unordered_map<PxRigidActor*, int> idsByRigidBody = m_utils.m_rigidBodyManager->GetIDsByBodies();
            if(idsByRigidBody.find(hit.block.actor) != idsByRigidBody.end())
            {
                return idsByRigidBody.find(hit.block.actor)->second;
            }
            else
            {
                // Nothing
            }
            // Now comes the difficult task of checking vs character controllers
            unordered_map<PxController*, int> idsByCharacterController = m_utils.m_characterControlManager->GetIdsByControllers();
            for(auto pairs : idsByCharacterController) // Loop through every pair in the list
            {
                if(pairs.first->getActor() == hit.block.actor) // The first part contains the actor pointer
                {
                    return pairs.second; // If this is true we found a hit vs character controller, second contains ID
                }
            }

            return -1;
        }

        int RayCastManagerImpl::CastSweep(const XMFLOAT3& p_origin, XMFLOAT3& p_direction, float p_width, const float& p_range)
        {
            if(p_range <= 0.0f)
            {
                cout << "Physcs. Raytracer. Sweep. Range of sweep was zero or below" << endl;
                return -1;
            }
            // Cast directx things to physx
            PxVec3 origin = PxVec3(p_origin.x, p_origin.y, p_origin.z);
            PxVec3 direction = PxVec3(p_direction.x, p_direction.y, p_direction.z);
            direction.normalize();
            PxSweepBuffer hit; // Used to save the hit
            /// Paramters for the sweep
            // PxGeometry* geometry
            bool status = m_utils.m_worldScene->sweep(PxSphereGeometry(p_width), PxTransform(origin), direction, p_range, hit);
            // hit.block.position;
            if(!status && !hit.hasBlock)
            {
                // No hit detected, return -1 TODOKO Maybee i should return something better?
                return -1;
            }

            // Start with checking static and dynamic rigid bodies
            unordered_map<PxRigidActor*, int> idsByRigidBody = m_utils.m_rigidBodyManager->GetIDsByBodies();
            if(idsByRigidBody.find(hit.block.actor) != idsByRigidBody.end())
            {
                return idsByRigidBody.find(hit.block.actor)->second;
            }
            else
            {
                // Nothing
            }
            // Now comes the difficult task of checking vs character controllers
            unordered_map<PxController*, int> idsByCharacterController = m_utils.m_characterControlManager->GetIdsByControllers();
            for(auto pairs : idsByCharacterController) // Loop through every pair in the list
            {
                if(pairs.first->getActor() == hit.block.actor) // The first part contains the actor pointer
                {
                    return pairs.second; // If this is true we found a hit vs character controller, second contains ID
                }
            }

            return -1;
        }
    }
}