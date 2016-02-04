#pragma once
#include <Internal/SubModule/PotentialFieldSubModuleImpl.hpp>
#include <Internal/PotentialField/PotentialFieldImpl.hpp>
#include <Internal/PotentialField/PotentialGroupImpl.hpp>
#include <Internal/PotentialField/PotentialFieldActorImpl.hpp>
namespace DoremiEngine
{
    namespace AI
    {
        PotentialFieldSubModuleImpl::PotentialFieldSubModuleImpl() {}
        PotentialFieldSubModuleImpl::~PotentialFieldSubModuleImpl() {}
        PotentialField* PotentialFieldSubModuleImpl::CreateNewField(const float& p_width, const float& p_height, const int& p_numberOfQuadsWidth,
                                                                    const int& p_numberOfQuadsHeight, const DirectX::XMFLOAT3& p_center)
        {
            // TODOKO review if we need to create a 3d grid or if a 2d grid is enough just to see where they can move in x and z
            using namespace std;
            using namespace DirectX;
            PotentialField* newField = new PotentialFieldImpl();
            vector<vector<PotentialFieldGridPoint>> grid;
            float quadWidth = p_width / static_cast<float>(p_numberOfQuadsWidth);
            float quadHeight = p_height / static_cast<float>(p_numberOfQuadsHeight);
            float halfWidth = p_width / 2.0f;
            float halfHeight = p_height / 2.0f;
            XMFLOAT2 bottomLeft = XMFLOAT2(p_center.x - halfWidth, p_center.z - halfHeight);
            grid.resize(p_numberOfQuadsWidth);
            for(size_t x = 0; x < p_numberOfQuadsWidth; x++)
            {
                for(size_t y = 0; y < p_numberOfQuadsHeight; y++)
                {
                    PotentialFieldGridPoint newPoint;
                    newPoint.position = XMFLOAT2((float)x * quadWidth + bottomLeft.x + quadWidth * 0.5f , (float)y * quadHeight + bottomLeft.y + quadHeight * 0.5f);
                    grid[x].push_back(newPoint);
                }
            }
            newField->SetGrid(grid);
            newField->SetCenter(p_center);
            newField->SetHeight(p_height);
            newField->SetWidth(p_width);
            newField->SetQuadSize(XMFLOAT2(quadWidth, quadHeight));
            m_fields.push_back(newField);
            return newField;
        }
        PotentialGroup* PotentialFieldSubModuleImpl::CreateNewPotentialGroup()
        {
            PotentialGroup* newGroup = new PotentialGroupImpl();
            return newGroup;
        }
        PotentialFieldActor* PotentialFieldSubModuleImpl::CreateNewActor(const DirectX::XMFLOAT3& p_position, const float& p_charge,
                                                                         const float& p_range, const bool& p_static, const AIActorType& p_actorType) // TODOKO position should possibly be float2. Pointer so we dont need to send pos every frame?
        {
            PotentialFieldActor* newActor = new PotentialFieldActorImpl();
            newActor->SetStatic(p_static); // needs to be done first since set position is dependent on static or not
            newActor->SetPosition(p_position); // TODOKO Might produce a error since no static objects will have a good position
            newActor->SetCharge(p_charge);
            newActor->SetRange(p_range);
            newActor->SetActorType(p_actorType);

            return newActor;
        }
        void PotentialFieldSubModuleImpl::AttachActor(PotentialField& o_field, PotentialFieldActor* p_actor) { o_field.AddActor(p_actor); }
        int PotentialFieldSubModuleImpl::GetNextStep() { return 0; }
        PotentialField* PotentialFieldSubModuleImpl::FindBestPotentialField(const DirectX::XMFLOAT3& p_position)
        {
            // Take out all PF that you are within
            using namespace std;
            using namespace DirectX;
            vector<PotentialField*> possibleMatches;
            PotentialField* returnField = nullptr;
            for (size_t i = 0; i < m_fields.size(); i++)
            {
                XMFLOAT3 fieldCenter = m_fields[i]->GetCenter();
                XMFLOAT2 quadSize = m_fields[i]->GetQuadSize();
                int quadsX = m_fields[i]->GetGrid().size(); // Get number of quads in x and y
                int quadsY = m_fields[i]->GetGrid()[0].size();
                float fieldHalfWidth = quadSize.x * (float)quadsX * 0.5f; // Half the fields width and height
                float fieldHalfHeight = quadSize.y * (float)quadsY * 0.5f;
                float fieldLeftBoundary = fieldCenter.x - fieldHalfWidth; // takes out the fields left, right and top, bottom boundary positions 
                float fieldRightBoundary = fieldCenter.x + fieldHalfWidth;
                float fieldTopBoundary = fieldCenter.y + fieldHalfHeight;
                float fieldBottomBoundary = fieldCenter.y - fieldHalfHeight;
                if (p_position.x <= fieldRightBoundary && p_position.x >= fieldLeftBoundary &&
                    p_position.y <= fieldTopBoundary && p_position.y >= fieldBottomBoundary) // If inside the field
                {
                    possibleMatches.push_back(m_fields[i]); // Still need to check which field is closest in Y
                }
            }
            // Check which possible field is closest in Y
            float closestY = 1000000;
            for (size_t i = 0; i < possibleMatches.size(); i++)
            {
                XMFLOAT3 fieldCenter = possibleMatches[i]->GetCenter();
                float distanceY = p_position.y - fieldCenter.y;
                distanceY = abs(distanceY);
                if (distanceY < closestY)
                {
                    returnField = possibleMatches[i];
                }
            }
            if (returnField == nullptr)
            {
                // TODOKO log error / message
            }
            return returnField;
        }
    }
}