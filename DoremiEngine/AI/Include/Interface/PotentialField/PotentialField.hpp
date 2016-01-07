#pragma once
#include <DirectXMath.h>
#include <vector>
namespace DoremiEngine
{
    namespace AI
    {
        struct PotentialFieldGridPoint
        {
            DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(0, 0);
            float charge = 0;
            PotentialFieldGridPoint(const DirectX::XMFLOAT2& p_position, const float& p_charge) : position(p_position), charge(p_charge) {}
            PotentialFieldGridPoint() {}
        };
        /**
        Contains information on a mapgrid of positions, only works for squares
        */
        class PotentialFieldActor;
        class PotentialField
        {
            public:
            /**
            Saves a 2D array of 3D positions, TODOKO Might be changed to arrays instead of vector
            */
            virtual void SetGrid(const std::vector<std::vector<PotentialFieldGridPoint>>& p_grid) = 0;
            virtual void SetHeight(const float& p_height) = 0;
            virtual void SetWidth(const float& p_width) = 0;
            virtual void SetCenter(const DirectX::XMFLOAT2& p_center) = 0;
            /**
            Updates the potentialfield using all the actors TODOKO review if it should only update the dynamic actors and if this option should exist
            in potentialfieldactor
            */
            virtual void Update() = 0;
            /**
            Adds a new actor that will influate the potential field
            */
            virtual void AddActor(PotentialFieldActor* p_newActor) = 0;
            /**
            Returns the position of the gridpoint the given units position is most attracted to
            */
            virtual DirectX::XMFLOAT2 GetAttractionPosition(const DirectX::XMFLOAT3& p_unitPosition) = 0;
        };
    }
}
