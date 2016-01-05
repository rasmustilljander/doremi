#pragma once
#include <DirectXMath.h>
#include <vector>
namespace DoremiEngine
{
    namespace AI
    {
        struct PotentialFieldGridPoint
        {
            DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0, 0, 0);
            float charge = 0;
            PotentialFieldGridPoint(const DirectX::XMFLOAT3& p_position, const float& p_charge) : position(p_position), charge(p_charge) {}
        };
        /**
        Contains information on a mapgrid of positions
        */
        class PotentialFieldActor;
        class PotentialField
        {
            public:
            /**
            Saves a 2D array of 3D positions, TODOKO Might be changed to arrays instead of vector and 2d positions
            */
            virtual void SetGrid(const std::vector<std::vector<PotentialFieldGridPoint>>& p_grid) = 0;
            /**
            Adds a new actor that will influate the potential field
            */
            virtual void AddActor(PotentialFieldActor* p_newActor) = 0;
        };
    }
}
