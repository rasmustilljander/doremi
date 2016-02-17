#include <Internal/FluidManagerImpl.hpp>
#include <Internal/PhysicsModuleImplementation.hpp>
#include <Internal/ParticleClasses/ParticleEmitter.hpp>

#include <iostream>
using namespace std;
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

        const vector<XMFLOAT3>& FluidManagerImpl::GetRemovedParticlesPositions(int p_id)
        {
            // Check if the particle system exists
            if(m_emitters.find(p_id) == m_emitters.end())
            {
                cout << "Physics. Particle systems. GetRemoveParticlesPositions. No particle system exists with id" << p_id << endl;
                return vector<XMFLOAT3>();
            }
            return m_emitters.at(p_id)->GetRemovedParticlesPositions();
        }

        void FluidManagerImpl::SetParticleEmitterData(int p_id, ParticleEmitterData p_data) { m_emitters[p_id]->SetData(p_data); }

        vector<int> FluidManagerImpl::GetDrainsHit(int p_id)
        {
            // Secure that the emitter exists
            if(m_emitters.find(p_id) == m_emitters.end())
            {
                throw std::runtime_error("Physics: DrainsHit fetch failed: No particle emitter exists with id: " + to_string(p_id));
            }
            return m_emitters[p_id]->GetDrainsHit();
        }

        void FluidManagerImpl::Update(float p_dt)
        {
            // Update all our emitters
            for(auto it = m_emitters.begin(); it != m_emitters.end(); ++it)
            {
                it->second->SetParticlesLifeTime(1); // TODOJB ensure this isn't hard-codes
                it->second->Update(p_dt);
            }
        }
    }
}