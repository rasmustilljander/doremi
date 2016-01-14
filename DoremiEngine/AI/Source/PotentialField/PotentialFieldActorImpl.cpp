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
        void PotentialFieldActorImpl::SetPosition(const DirectX::XMFLOAT3& p_position)
        {
            if(m_static)
            {
                // TODOKO error/cout
                return;
            }
            else
            {
                m_position = p_position;
            }
        }
        void PotentialFieldActorImpl::AddOccupiedQuad(const DirectX::XMINT2& p_quad)
        {
            // TODOKO add checking for dublicates
            m_occupiedQuads.push_back(p_quad);
        }
        const DirectX::XMINT2& PotentialFieldActorImpl::GetClosestOccupied(const DirectX::XMFLOAT3 p_position)
        {
            return DirectX::XMINT2(0, 0); // KONRAD CONTINUE HERE!
        }
    }
}
