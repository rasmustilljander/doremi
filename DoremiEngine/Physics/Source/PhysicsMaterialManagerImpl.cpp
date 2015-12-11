#include <Internal/PhysicsMaterialManagerImpl.hpp>
#include <Internal/PhysicsModuleImplementation.hpp>
namespace DoremiEngine
{
    namespace Physics
    {
        PhysicsMaterialManagerImpl::PhysicsMaterialManagerImpl(InternalPhysicsUtils& p_utils) : m_utils(p_utils) { m_nextMaterial = 0; }
        PhysicsMaterialManagerImpl::~PhysicsMaterialManagerImpl() {}

        int PhysicsMaterialManagerImpl::CreateMaterial(float p_staticFriction, float p_dynamicFriction, float p_restitution)
        {
            m_materials[m_nextMaterial] = m_utils.m_physics->createMaterial(p_staticFriction, p_dynamicFriction, p_restitution);
            m_nextMaterial++;
            return m_nextMaterial - 1;
        }

        PxMaterial* PhysicsMaterialManagerImpl::GetMaterial(int p_id) { return m_materials[p_id]; }
    }
}