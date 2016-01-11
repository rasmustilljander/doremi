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
                                                                    const int& p_numberOfQuadsHeight, const DirectX::XMFLOAT2& p_center)
        {
            // TODOKO review if we need to create a 3d grid or if a 2d grid is enough just to see where they can move in x and z
            using namespace std;
            using namespace DirectX;
            PotentialField* newField = new PotentialFieldImpl();
            vector<vector<PotentialFieldGridPoint>> grid;
            float quadWidth = p_width / (float)p_numberOfQuadsWidth;
            float quadHeight = p_height / (float)p_numberOfQuadsHeight;
            float halfWidth = p_width / 2.0f;
            float halfHeight = p_height / 2.0f;
            XMFLOAT2 bottomLeft = XMFLOAT2(p_center.x - halfWidth, p_center.y - halfHeight);
            grid.resize(p_numberOfQuadsWidth);
            for(size_t x = 0; x < p_numberOfQuadsWidth; x++)
            {
                for(size_t y = 0; y < p_numberOfQuadsHeight; y++)
                {
                    PotentialFieldGridPoint newPoint;
                    newPoint.position = XMFLOAT2((float)x * quadWidth + bottomLeft.x, (float)y * quadHeight + bottomLeft.y);
                    grid[x].push_back(newPoint);
                }
            }
            newField->SetGrid(grid);
            newField->SetCenter(p_center);
            newField->SetHeight(p_height);
            newField->SetWidth(p_width);
            return newField;
        }
        PotentialGroup* PotentialFieldSubModuleImpl::CreateNewPotentialGroup()
        {
            PotentialGroup* newGroup = new PotentialGroupImpl();
            return newGroup;
        }
        PotentialFieldActor*
        PotentialFieldSubModuleImpl::CreateNewActor(const DirectX::XMFLOAT3& p_position, const float& p_charge, const float& p_range, const bool& p_static) // TODOKO position should possibly be float2. Pointer so we dont need to send pos every frame?
        {
            PotentialFieldActor* newActor = new PotentialFieldActorImpl();
            newActor->SetPosition(p_position);
            newActor->SetCharge(p_charge);
            newActor->SetRange(p_range);
            newActor->SetStatic(p_static);
            return newActor;
        }
        void PotentialFieldSubModuleImpl::AttachActor(PotentialField& o_field, PotentialFieldActor* p_actor) { o_field.AddActor(p_actor); }
        int PotentialFieldSubModuleImpl::GetNextStep() { return 0; }
    }
}