#include <Internal/PotentialField/PotentialGroupImpl.hpp>
#include <Internal/PotentialField/PotentialFieldActorImpl.hpp>
#include <iostream>
namespace DoremiEngine
{
    namespace AI
    {

        PotentialGroupImpl::PotentialGroupImpl() {}
        PotentialGroupImpl::~PotentialGroupImpl() {}
        void PotentialGroupImpl::AddActor(PotentialFieldActor* p_newActor) { m_group.insert(p_newActor); };
        DirectX::XMFLOAT3 PotentialGroupImpl::GetForceDirection(const DirectX::XMFLOAT3& p_unitPosition, const PotentialFieldActor* p_actor)
        {
            using namespace DirectX;
            XMFLOAT3 forceDirection = XMFLOAT3(0, 0, 0); // Start with 0
            size_t length = m_group.size();
            for(auto actor : m_group) // Walk through every actor in group
            {
                if(actor != nullptr && p_actor != actor)
                {
                    float power = actor->GetCharge(); // Get important data
                    float area = actor->GetRange();
                    XMVECTOR chargePos = XMLoadFloat3(&actor->GetPosition());
                    XMVECTOR addedForce = XMLoadFloat3(&forceDirection);
                    XMVECTOR myPosition = XMLoadFloat3(&p_unitPosition);

                    XMVECTOR direction = chargePos - myPosition; // Vector between actors position and given position
                    float dist = *XMVector3Length(direction).m128_f32; // Distance between
                    if(dist < area && dist != 0) // just another check so we dont accidentaly divide with 0
                    {
                        float force = power / dist; // Total force
                        direction *= force; // TODOKO Direction should maybe be normalized, in which case the force calc might need changin
                        addedForce += direction; // Adds the vector with the force from the current actor to the total force
                        XMStoreFloat3(&forceDirection, addedForce);
                    }
                }
            }
            XMVECTOR normalDirection = XMLoadFloat3(&forceDirection); // Normalize
            normalDirection = XMVector3Normalize(normalDirection);
            XMStoreFloat3(&forceDirection, normalDirection);
            return forceDirection;
        }
    }
}