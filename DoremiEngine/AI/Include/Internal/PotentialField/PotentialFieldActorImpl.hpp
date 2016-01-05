#pragma once
#include <Interface/PotentialField/PotentialFieldActor.hpp>
#include <DirectXMath.h>
#include <vector>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldActorImpl : public PotentialFieldActor
        {
            public:
            PotentialFieldActorImpl();
            virtual ~PotentialFieldActorImpl();

            void SetPosition(const DirectX::XMFLOAT3& p_position) override;

            void SetCharge(const float& p_charge) override;

            private:
            float m_charge;
            DirectX::XMFLOAT3 m_position;
        };
    }
}
