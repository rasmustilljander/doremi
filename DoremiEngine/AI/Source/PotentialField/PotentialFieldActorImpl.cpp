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
        const DirectX::XMINT2 PotentialFieldActorImpl::GetClosestOccupied(const DirectX::XMINT2& p_quad)
        {
            using namespace DirectX;
            size_t length = m_occupiedQuads.size();
            DirectX::XMINT2 returnQuad = XMINT2(-1, -1);
            float shortestDistance = 1000000; // Some sort of max distance

            for(size_t i = 0; i < length; i++)
            {
                XMVECTOR inQuad = XMLoadSInt2(&p_quad);
                XMVECTOR checkQuad = XMLoadSInt2(&m_occupiedQuads[i]);
                XMVECTOR distanceVec = inQuad - checkQuad;
                distanceVec = XMVector2Length(distanceVec);
                float distance = *distanceVec.m128_f32;
                if(distance < shortestDistance)
                {
                    shortestDistance = distance;
                    returnQuad = m_occupiedQuads[i];
                }
            }
            return returnQuad;
        }
    }
}
