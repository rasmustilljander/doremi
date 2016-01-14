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
            bool occupied = false;
            PotentialFieldGridPoint(const DirectX::XMFLOAT2& p_position, const float& p_charge, const bool& p_occupied)
                : position(p_position), charge(p_charge), occupied(p_occupied)
            {
            }
            PotentialFieldGridPoint() {}
        };
        /**
        TODOKO docs
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
            virtual void SetCenter(const DirectX::XMFLOAT3& p_center) = 0;
            /**
            Set the quadsize in that field.
            */
            virtual void SetQuadSize(const DirectX::XMFLOAT2& p_quadSize) = 0;
            /**
            Get the center of the field
            */
            virtual const DirectX::XMFLOAT3& GetCenter() const = 0;
            /**
            Get the quadsize
            */
            virtual const DirectX::XMFLOAT2& GetQuadSize() const = 0;
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
            Returns the position of the gridpoint the given units position is most attracted to. If a actor is given the charge given by that actor
            will be ignored. If the static check flag is set to true the function will use the stored value in the potential field, if set to false
            the function will calculate the charges of the surrounding quads on the fly. This is usale if the field is dynamic since the whole field
            never needs updating.
            */
            virtual DirectX::XMFLOAT2 GetAttractionPosition(const DirectX::XMFLOAT3& p_unitPosition,
                                                            const PotentialFieldActor* p_currentActor = nullptr, const bool& p_staticCheck = true) = 0;

            virtual const std::vector<std::vector<PotentialFieldGridPoint>>& GetGrid() const = 0;
        };
    }
}
