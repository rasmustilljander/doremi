#pragma once
#include <Internal/PotentialField/PotentialFieldActorImpl.hpp>
#include <DirectXMath.h>
#include <vector>
namespace DoremiEngine
{
    namespace AI
    {
        PotentialFieldActorImpl::PotentialFieldActorImpl() {}
        PotentialFieldActorImpl::~PotentialFieldActorImpl() {}

        void PotentialFieldActorImpl::SetPosition(const DirectX::XMFLOAT3& p_position) { m_position = p_position; }

        void PotentialFieldActorImpl::SetCharge(const float& p_charge) { m_charge = p_charge; }
    }
}
