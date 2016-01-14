#include <Internal/FluidManagerImpl.hpp>
#include <Internal/PhysicsModuleImplementation.hpp>
#include <Internal/ParticleClasses/ParticleEmitter.hpp>
namespace DoremiEngine
{
    namespace Physics
    {
        FluidManagerImpl::FluidManagerImpl(InternalPhysicsUtils& p_utils) : m_utils(p_utils) {}
        FluidManagerImpl::~FluidManagerImpl() {}

        void FluidManagerImpl::CreateParticleEmitter(int p_id, ParticleEmitterData p_data)
        {
            m_emitters[p_id] = new ParticleEmitter(p_data, m_utils);
        }
        void FluidManagerImpl::GetParticlePositions(int p_id, vector<XMFLOAT3>& o_positions) { m_emitters[p_id]->GetPositions(o_positions); }

        void FluidManagerImpl::Update(float p_dt)
        {
            // Update all our emitters
            for(auto it = m_emitters.begin(); it != m_emitters.end(); ++it)
            {
                it->second->Update(p_dt);
            }
        }
    }
}