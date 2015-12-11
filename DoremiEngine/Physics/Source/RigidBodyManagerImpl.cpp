#include <Internal/RigidBodyManagerImpl.hpp>
#include <Internal/PhysicsModuleImplementation.hpp>
namespace DoremiEngine
{
    namespace Physics
    {
        RigidBodyManagerImpl::RigidBodyManagerImpl(InternalPhysicsUtils& p_utils) : m_utils(p_utils) { m_nextBody = 0; }
        RigidBodyManagerImpl::~RigidBodyManagerImpl() {}

        int RigidBodyManagerImpl::AddBoxBodyDynamic(XMFLOAT3 p_position, XMFLOAT4 p_orientation, XMFLOAT3 p_dims, int p_materialID)
        {
            // Create a world matrix (only translation) i think
            PxVec3 position = PxVec3(p_position.x, p_position.y, p_position.z);
            PxQuat orientation = PxQuat(p_orientation.x, p_orientation.y, p_orientation.z, p_orientation.w);
            PxVec3 dims = PxVec3(p_dims.x, p_dims.y, p_dims.z);
            // Creates the physics object shape thingy, which collides with stuff. Shapes are just objects. I
            PxShape* shape = m_utils.m_physics->createShape(PxBoxGeometry(dims), *m_utils.m_physicsMaterialManager->GetMaterial(p_materialID));
            // Creates the actual body.
            PxTransform transform = PxTransform(position, orientation);
            PxRigidDynamic* body = m_utils.m_physics->createRigidDynamic(transform);
            // Attach shape to the body
            body->attachShape(*shape);
            // Give the body some mass
            PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
            // Add the now fully created body to the scene
            m_utils.m_worldScene->addActor(*body);
            // We're done with the shape. Release it
            shape->release();

            // Finally add the body to our list
            m_bodies[m_nextBody];
            m_nextBody++;

            /*
            And now we have added a box to the world at the given position
            I'm not too sure how we update the box, or the scene, or perform
            any form of interaction however*/

            // Return ID of body we just created
            return m_nextBody - 1;
        }
        void RigidBodyManagerImpl::AddForceToBody(int p_bodyID, XMFLOAT3 p_force)
        {
            m_bodies[p_bodyID]->addForce(PxVec3(p_force.x, p_force.y, p_force.z));
        }

        void RigidBodyManagerImpl::SetBodyVelocity(int p_bodyID, XMFLOAT3 p_v)
        {
            m_bodies[p_bodyID]->setAngularVelocity(PxVec3(p_v.x, p_v.y, p_v.z));
        }

        void RigidBodyManagerImpl::SetBodyPosition(int p_bodyID, XMFLOAT3 p_v, XMFLOAT4 p_o)
        {
            PxVec3 position = PxVec3(p_v.x, p_v.y, p_v.z);
            PxQuat orientation = PxQuat(p_o.x, p_o.y, p_o.z, p_o.w);
            m_bodies[p_bodyID]->setGlobalPose(PxTransform(position, orientation));
        }

        XMFLOAT3 RigidBodyManagerImpl::GetBodyPosition(int p_bodyID)
        {
            PxVec3 p = m_bodies[p_bodyID]->getGlobalPose().p;
            return XMFLOAT3(p.x, p.y, p.z);
        }

        XMFLOAT4 RigidBodyManagerImpl::GetBodyOrientation(int p_bodyID)
        {
            PxQuat q = m_bodies[p_bodyID]->getGlobalPose().q;
            return XMFLOAT4(q.x, q.y, q.z, q.w);
        }

        XMFLOAT3 RigidBodyManagerImpl::GetBodyVelocity(int p_bodyID)
        {
            PxVec3 v = m_bodies[p_bodyID]->getLinearVelocity();
            return XMFLOAT3(v.x, v.y, v.z);
        }
    }
}