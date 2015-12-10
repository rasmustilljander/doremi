#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

#if defined(_WINDLL)
#define PHYSICS_DLL_EXPORT __declspec(dllexport)
#else
#define PHYSICS_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Physics
    {
        class PhysicsModule : public DoremiEngine::Core::EngineModule
        {
            public:
            virtual void Update(float p_dt) = 0;
            virtual void StartPhysX() = 0;


            // Test function
            virtual float ExampleMethod(const float& posx) = 0;
            virtual void ExampleMethodAltersData(float* p_posX, float* p_posY) = 0;
        };
    }
}

extern "C" {
    typedef DoremiEngine::Physics::PhysicsModule* (*CREATE_PHYSICS_MODULE)(const DoremiEngine::Core::SharedContext&);
    PHYSICS_DLL_EXPORT DoremiEngine::Physics::PhysicsModule*
        CreatePhysicsModule(const DoremiEngine::Core::SharedContext& p_context);
}