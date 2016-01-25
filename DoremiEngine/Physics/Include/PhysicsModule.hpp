#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

#include <vector>

#if defined(_WINDLL)
#define PHYSICS_DLL_EXPORT __declspec(dllexport)
#else
#define PHYSICS_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Physics
    {
        struct CollisionPair
        {
            int firstID = -1;
            int secondID = -1;
        };

        class RigidBodyManager;
        class PhysicsMaterialManager;
        class CharacterControlManager;
        class FluidManager;
        class RayCastManager;
        class PhysicsModule : public DoremiEngine::Core::EngineModule
        {
        public:
            /**
            Does nothing so far TODOJB update as necessary
            */
            virtual void Update(float p_dt) = 0;

            /**
            Gets the rigid body manager. This manager handles all rigid bodies.
            It can be used to apply all kinds of forces, velocities and positions
            to bodies. Its functionality is expanded as necessary
            */
            virtual RigidBodyManager& GetRigidBodyManager() = 0;
            /**
            Gets the material manager. This manager handles all physical materials.
            This is used to changed attributes of certain materials. It probably will
            not be used all that much. Its functionality is exapnded as necessary
            */
            virtual PhysicsMaterialManager& GetPhysicsMaterialManager() = 0;
            /**
            Gets the character control manager. This manager handles all character
            controllers, used to govern more specific movements where ordinary physics
            on rigid bodies is too complex to handle
            */
            virtual CharacterControlManager& GetCharacterControlManager() = 0;
            /**
            Experimental fluid manager. TODOJB update Docs as necessary*/
            virtual FluidManager& GetFluidManager() = 0;

            /**
            Returns the raycast manager which contains different raycasting functions
            */
            virtual RayCastManager& GetRayCastManager() = 0;
            /**
            Gets a vector if all collision pairs. A collision pair consists of ids
            of two bodies which have collided in the last simulation. This list is
            automatically cleared before each new simulation*/
            virtual std::vector<CollisionPair> GetCollisionPairs() = 0;

            /**
            Gets a vector of all trigger collision pairs. The firstID is always the
            id of the trigger, and the secondID is always the id of the actor which
            collided with the trigger. It's awesome that way*/
            virtual std::vector<CollisionPair> GetTriggerPairs() = 0;
        };
    }
}

extern "C" {
typedef DoremiEngine::Physics::PhysicsModule* (*CREATE_PHYSICS_MODULE)(const DoremiEngine::Core::SharedContext&);
PHYSICS_DLL_EXPORT DoremiEngine::Physics::PhysicsModule* CreatePhysicsModule(const DoremiEngine::Core::SharedContext& p_context);
}