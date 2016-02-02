#pragma once
#include <Internal/PotentialField/PotentialFieldActorImpl.hpp>
#include <DirectXMath.h>
#include <vector>
#include <iostream>
namespace DoremiEngine
{
    namespace AI
    {
        PotentialFieldActorImpl::PotentialFieldActorImpl() 
        {
            //TODOEA FULT SOM FAN att sätat en hårdkodad fulposition så att listan inte är tom. bättre än att kolla varje gång att den är tom.
            m_prevGridPos = DirectX::XMINT2(0, 0);
        }
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

        void PotentialFieldActorImpl::UpdatePhermoneTrail(const DirectX::XMINT2 & p_gridPosToAdd)
        {
            int vectorSize = m_phermoneTrail.size();
            // if it is greater than 5 we remove the last before we add this one
            // TODOCONFIG HÅRDKODAT VÄRDE ATT DET ÄR en trail på 5

            if (vectorSize >= 5) 
            {

                m_phermoneTrail.erase(m_phermoneTrail.begin());
                // m_phermoneTrail.push_back(p_gridPosToAdd);
            }

            m_phermoneTrail.push_back(p_gridPosToAdd);
        }
        void PotentialFieldActorImpl::EraseFirstInPhermoneList()
        {
            //m_phermoneTrail.erase(m_phermoneTrail.begin());
        }
        void PotentialFieldActorImpl::AddPotentialVsOther(const PotentialChargeInformation& p_newPotential)
        {
            m_potentialsVsOther.push_back(p_newPotential);
        }
    }
}
