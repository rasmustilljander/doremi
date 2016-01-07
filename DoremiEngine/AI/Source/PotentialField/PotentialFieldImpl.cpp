#include <Internal/PotentialField/PotentialFieldImpl.hpp>

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
            size_t nrOfActors = m_actors.size();
            size_t nrOfQuadsX = m_grid.size();
            size_t nrOfQuadsY = m_grid[0].size();
            for
                each(PotentialFieldActor * actor in m_actors)
                {
                    XMFLOAT3 actorPos3d = actor->GetPosition(); // Dont really care about the third dimension TODOKO review if 3d is needed
                    XMFLOAT2 actorPos = XMFLOAT2(actorPos3d.x, actorPos3d.z);
                    float actorCharge = actor->GetCharge();
                    float actorRange = actor->GetRange();
                    for(size_t x = 0; x < nrOfQuadsX; x++)
                    {
                        for(size_t y = 0; y < nrOfQuadsY; y++)
                        {
                            XMFLOAT2 quadPos = m_grid[x][y].position;
                            // Calculate charge
                            XMVECTOR actorPosVec = XMLoadFloat2(&actorPos);
                            XMVECTOR quadPosVec = XMLoadFloat2(&quadPos);

                            XMVECTOR distance = actorPosVec - quadPosVec;
                            float dist = *XMVector3Length(distance).m128_f32;
                            if(dist < actorRange)
                            {
                                float force = actorCharge / dist;
                                m_grid[x][y].charge += force;
                            }
                        }
                    }
                }
        }
        void PotentialFieldImpl::AddActor(PotentialFieldActor* p_newActor)
        {
            m_actors.push_back(p_newActor);
        } // TODOKO check if actor is already in the field
        DirectX::XMFLOAT2 PotentialFieldImpl::GetAttractionPosition(const DirectX::XMFLOAT3& p_unitPosition)
        {
            // TODOKO Test!
            // Good thing to note is that the grid is originated from bottom left corner so [0][0] is bottom left corner
            using namespace DirectX;
            XMFLOAT3 unitPos = p_unitPosition; // Needs to be modifiable
            XMFLOAT2 position2D = XMFLOAT2(p_unitPosition.x, p_unitPosition.z);
            float gridQuadWidth = m_width / (float)m_grid.size(); // Gets the width and hight of one quad
            float gridQuadHeight = m_height / (float)m_grid[0].size();
            // Offset given position with the fields offset to take it back to origo so we are able to calculate which quad we are in
            XMFLOAT2 bottomLeft = XMFLOAT2(m_center.x - m_width / 2.0f, m_center.y - m_height / 2.0f);
            unitPos.x -= bottomLeft.x;
            unitPos.y -= bottomLeft.y;
            int quadNrX = std::floor(unitPos.x / gridQuadWidth); // What quad in x and y
            int quadNrY = std::floor(unitPos.y / gridQuadHeight);
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
            XMFLOAT2 highestChargedPos;
            float highestCharge = -100000;
            for(size_t i = 0; i < length; i++)
            {
                int x = quadsToCheck[i].x;
                int y = quadsToCheck[i].y;
                if(x >= 0 && x < m_grid.size() && y >= 0 && y < m_grid[0].size())
                {
                    // The quad exists!
                    float quadCharge = m_grid[x][y].charge;
                    if(quadCharge > highestCharge)
                    {
                        highestCharge = quadCharge;
                        highestChargedPos = m_grid[x][y].position;
                    }
                }
            }
            // If now charge was found we are probably outside the field... in which case we should start walking towards the field
            if(highestCharge <= -100000)
            {
                // TODOKO Find out which quad is closest
            }
            return XMFLOAT2(0, 0);
        }
    }
}