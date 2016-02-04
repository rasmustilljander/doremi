#pragma once
#include <DirectXMath.h>
#include <vector>
#include <functional>
namespace DoremiEngine
{
    namespace AI
    {
        enum class AIActorType : size_t
        {
            None = 0,
            Player = 1,
            Enemy = 2,
            Wall = 4,
            Goal = 8
        };
        struct PotentialChargeInformation
        {
            float charge; // charge as float
            float range; // range as float
            bool AddedOnAttracting; // if this field should only be added when checking vs attracting forces TODOKO obsoloete?
            bool AddedOnReppeling; // if this field should only be added when checking vs reppeling forces
            bool active; // If it's active or not
            bool usePhermoneTrail; // If true the phermone trail wont be used if this field is active and used/in range
            AIActorType actorToBeAddedTo; // Adds this potentialfield to all actors that have the bitmask specified
            std::function<float(float, float, float)> forceEquation; // The field uses the given function to calculate force impact
            // TODOKO add a specific actor this field should be used when checking against, if needed
            PotentialChargeInformation(const float& p_charge, const float& p_range, const bool& p_addedOnAttracting, const bool& p_addedOnReppeling,
                                       const bool& p_active, const bool& p_usePhermoneTrail, const AIActorType& p_actorType,
                                       const std::function<float(float, float, float)>& p_forceEquation)
                : charge(p_charge),
                  range(p_range),
                  AddedOnAttracting(p_addedOnAttracting),
                  AddedOnReppeling(p_addedOnReppeling),
                  active(p_active),
                  usePhermoneTrail(p_usePhermoneTrail),
                  actorToBeAddedTo(p_actorType),
                  forceEquation(p_forceEquation){};
            /**
            Standard is false for everything and 0 in range and charge
            */
            PotentialChargeInformation()
                : charge(0), range(0), AddedOnAttracting(false), AddedOnReppeling(false), active(false), usePhermoneTrail(false), actorToBeAddedTo(AIActorType::None)
            {
            }
        };
        /**
        A potentialfield actor contains information on reppeling or attracting force. It also contains a range and charge information.
        it also contains private fields that should be used when trying to move to another actor with some information on what different actors it
        should use these fields on
        */
        class PotentialFieldActor
        {
        public:
            /**
            Sets the actor position
            */
            virtual void SetPosition(const DirectX::XMFLOAT3& p_position) = 0;
            /**
            Sets the charge of the actor. + for attraction - for repulsion
            */
            virtual void SetCharge(const float& p_charge) = 0;
            /**
            Sets the range of the actor
            */
            virtual void SetRange(const float& p_range) = 0;
            /**
            Sets if the actor is static or dynamic
            */
            virtual void SetStatic(const bool& p_static) = 0;
            /**
            Adds a quad to the occupied list
            */
            virtual void AddOccupiedQuad(const DirectX::XMINT2& p_quad) = 0;
            /**
            Gets the actor position
            */
            virtual const DirectX::XMFLOAT3& GetPosition() const = 0;
            /**
            Gets the actors charge
            */
            virtual const float& GetCharge() const = 0;
            /**
            Gets the range of the actor
            */
            virtual const float& GetRange() const = 0;
            /**
            Gets the actors type
            */
            virtual const AIActorType& GetActorType() const = 0;
            /**
            Sets the actors type
            */
            virtual void SetActorType(const AIActorType& p_type) = 0;
            /**
            Gets all the quads the actor is occupiing
            */
            virtual const std::vector<DirectX::XMINT2>& GetOccupiedQuads() const = 0;
            /**
            Returns true if the actor is static, false for dynamic
            */
            virtual const bool& IsStatic() const = 0;
            /**
            Returns the closest actor occupied quad to the given quad, the range is not taken in to consideration
            */
            virtual const DirectX::XMINT2 GetClosestOccupied(const DirectX::XMINT2& p_quad) = 0;
            /**
            Returns the phermonetrail vector.
            */
            virtual const std::vector<DirectX::XMINT2>& GetPhermoneTrail() const = 0;
            /**
            Returns last updates GridPos.
            */
            virtual const DirectX::XMINT2 GetPrevGridPos() const = 0;
            /**
            Sets the actor gridpos x,y
            */
            virtual void SetPrevGridPosition(const DirectX::XMINT2& p_prevGridPos) = 0;
            /**
            If the vector.size is more than 5 we remove the last one and add the new one. Otherwise we just add the gridpos.
            */
            virtual void UpdatePhermoneTrail(const DirectX::XMINT2& p_gridPosToAdd) = 0;
            /**
            Erase the first([0]) element in the phermone list!
            */
            virtual void EraseLatestAddedToPhermoneList() = 0;
            /**
            Adds a new potential to be used when checking vs other actors. This is what makes ranged stay on range and melee go to melee.
            If a actor contains one of these fields it will be used when checking vs other actors. For example a range unit might contain a 
            potential which will only be used when checking against positive charges. This will make the enemy move against the player since
            the player have a strong attracting force. This ranged enemy will also add this potential to the total charge making it almost as
            though the player have 2 charges with different information enabling the ranged unit to stay at range.
            */
            virtual void AddPotentialVsOther(const PotentialChargeInformation& p_newPotential) = 0;
            /**
            Gets the list with potentials vs others
            */
            virtual const std::vector<PotentialChargeInformation>& GetPotentialVsOthers() const = 0;
        };
    }
}
