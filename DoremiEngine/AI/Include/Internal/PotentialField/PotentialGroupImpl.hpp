#pragma once
#include <Interface/PotentialField/PotentialGroup.hpp>
// Standard
#include <set>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialGroupImpl : public PotentialGroup
        {
            public:
            PotentialGroupImpl();
            virtual ~PotentialGroupImpl();
            void AddActor(PotentialFieldActor* p_newActor) override;
            DirectX::XMFLOAT3 GetForceDirection(const DirectX::XMFLOAT3& p_unitPosition, const PotentialFieldActor* p_actor = nullptr) override;

            private:
            std::set<PotentialFieldActor*> m_group;
        };
    }
}
