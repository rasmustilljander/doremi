#include <Internal/PotentialField/PotentialFieldImpl.hpp>

#include <iostream>
namespace DoremiEngine
{
    namespace AI
    {
        PotentialFieldImpl::PotentialFieldImpl() {}
        PotentialFieldImpl::~PotentialFieldImpl() {}
        void PotentialFieldImpl::SetGrid(const std::vector<std::vector<PotentialFieldGridPoint>>& p_grid)
        {
            // m_grid = p_grid;
            m_grid = std::move(p_grid); // TODOKO Might not work...
        }
        void PotentialFieldImpl::Update()
        {
            // TODOKO optimize!!!! threads would be awesome here...
            // Lets all the actors update all the gridpoints in the potentialfield using the distance and charge
            using namespace DirectX;
            size_t nrOfQuadsX = m_grid.size();
            size_t nrOfQuadsY = m_grid[0].size();

            for(size_t x = 0; x < nrOfQuadsX; x++)
            {
                for(size_t y = 0; y < nrOfQuadsY; y++)
                {
                    XMFLOAT2 quadPos = m_grid[x][y].position;
                    float totalCharge = 0;
                    for(auto actor : m_staticActors)
                    {
                        XMINT2 myQuad = XMINT2(x, y);
                        XMINT2 closestQuad = actor->GetClosestOccupied(myQuad);
                        if(myQuad.x == closestQuad.x && myQuad.y == closestQuad.y)
                        {
                            // Same quad
                            m_grid[x][y].occupied = true;
                        }
                        XMFLOAT2 actorQuadPosition = m_grid[closestQuad.x][closestQuad.y].position;

                        float actorCharge = actor->GetCharge();
                        float actorRange = actor->GetRange();
                        // Calculate charge
                        XMVECTOR actorQuadPosVec = XMLoadFloat2(&actorQuadPosition);
                        XMVECTOR quadPosVec = XMLoadFloat2(&quadPos);

                        XMVECTOR distance = actorQuadPosVec - quadPosVec;
                        float dist = *XMVector2Length(distance).m128_f32;
                        if(dist < actorRange)
                        {
                            float force = actorCharge / dist;
                            totalCharge += force;
                        }
                    }
                    m_grid[x][y].charge = totalCharge;
                }
            }
        }
        void PotentialFieldImpl::AddActor(PotentialFieldActor* p_newActor)
        {
            if(p_newActor->IsStatic() && m_staticActors.find(p_newActor) == m_staticActors.end())
            {
                // The actor is static and not in list
                m_staticActors.insert(p_newActor);
            }
            else
            {
                // TODOKO check if actor is already in the field
                m_dynamicActors.push_back(p_newActor);
            }
        }

        DirectX::XMFLOAT2 PotentialFieldImpl::GetAttractionPosition(const DirectX::XMFLOAT3& p_unitPosition,
                                                                    const PotentialFieldActor* p_currentActor, const bool& p_staticCheck)
        {
            // TODOKO Test!
            // Good thing to note is that the grid is originated from bottom left corner so [0][0] is bottom left corner
            using namespace DirectX;
            XMFLOAT2 position2D = XMFLOAT2(p_unitPosition.x, p_unitPosition.z); // Needs to be modifiable
            float gridQuadWidth = m_width / (float)m_grid.size(); // Gets the width and hight of one quad
            float gridQuadHeight = m_height / (float)m_grid[0].size();
            // Offset given position with the fields offset to take it back to origo so we are able to calculate which quad we are in
            XMFLOAT2 bottomLeft = XMFLOAT2(m_center.x - m_width / 2.0f, m_center.y - m_height / 2.0f);
            position2D.x -= bottomLeft.x - 0.5f;
            position2D.y -= bottomLeft.y - 0.5f;
            int quadNrX = std::floor(position2D.x / gridQuadWidth); // What quad in x and y
            int quadNrY = std::floor(position2D.y / gridQuadHeight);
            // Add quads that needs checking, 3x3 square around the unit

            std::vector<XMINT2> quadsToCheck;
            for(int x = -1; x < 2; x++) // -1, 0, 1
            {
                for(int y = -1; y < 2; y++)
                {

                    quadsToCheck.push_back(XMINT2(quadNrX + x, quadNrY + y));
                }
            }
            // Check for special cases
            size_t length = quadsToCheck.size();
            XMFLOAT2 highestChargedPos = XMFLOAT2(m_center.x, m_center.z); // TODOEA KANSKE SKA VARA float3 om vi vill ha mer 3d
            float highestCharge = 0;
            if(quadNrX >= 0 && quadNrX < m_grid.size() && quadNrY >= 0 && quadNrY < m_grid[0].size())
            {
                // take the quad the unit is in as the highest charge. If all the qauds have the same charge the unit shouldnt move
                highestCharge = CalculateCharge(quadNrX, quadNrY, p_currentActor); // TODOKO secure for if the unit is outside the grid
                highestChargedPos = XMFLOAT2(p_unitPosition.x, p_unitPosition.z);
            }
            for(size_t i = 0; i < length; i++)
            {
                int x = quadsToCheck[i].x;
                int y = quadsToCheck[i].y;
                if(x >= 0 && x < m_grid.size() && y >= 0 && y < m_grid[0].size())
                {
                    // The quad exists!
                    float quadCharge;

                    if(p_staticCheck)
                    {
                        quadCharge = m_grid[x][y].charge;
                    }
                    else
                    {
                        quadCharge = CalculateCharge(x, y, p_currentActor);
                    }
                    if(quadCharge > highestCharge)
                    {
                        highestCharge = quadCharge;
                        highestChargedPos = m_grid[x][y].position;
                    }
                }
            }
            // If now charge was found we are probably outside the field... in which case we should start walking towards the field
            if(highestCharge <= -9000)
            {
            }
            return XMFLOAT2(highestChargedPos.x, highestChargedPos.y); // TODOEA Kanske skicka bak en xmfloat3
        }

        float PotentialFieldImpl::CalculateCharge(int p_quadX, int p_quadY, const PotentialFieldActor* p_currentActor)
        {
            using namespace DirectX;
            XMFLOAT2 quadPos = m_grid[p_quadX][p_quadY].position;
            float totalCharge = 0;
            for(auto actor : m_dynamicActors)
            {
                if(actor != p_currentActor) // this should mean that the current actor is skipped when calculating charge... still doesnt work...
                {
                    XMFLOAT3 actorPos3d = actor->GetPosition(); // Dont really care about the third dimension TODOKO review if 3d is needed
                    XMFLOAT2 actorPos = XMFLOAT2(actorPos3d.x, actorPos3d.z);
                    float actorCharge = actor->GetCharge();
                    float actorRange = actor->GetRange();
                    // Calculate charge
                    XMVECTOR actorPosVec = XMLoadFloat2(&actorPos);
                    XMVECTOR quadPosVec = XMLoadFloat2(&quadPos);

                    XMVECTOR distance = actorPosVec - quadPosVec;
                    float dist = *XMVector3Length(distance).m128_f32;
                    if(dist < actorRange)
                    {
                        float force = actorCharge / dist;
                        totalCharge += force;
                    }
                }
            }
            // std::cout << p_quadX << " " << p_quadY << " " << totalCharge << std::endl;
            return totalCharge + m_grid[p_quadX][p_quadY].charge;
        }
    }
}