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

            /**
            TODO
            */
            virtual void Startup() = 0;

            /**
            TODO
            */
            virtual void SetWorkingDirectory(const std::string& p_workingDirectory) = 0;

            /**
            TODO
            */
            virtual void Shutdown() = 0;

            //Test function
            virtual float ExampleMethod(const float& posx) = 0;
        };
    }
}

extern "C" {
    typedef DoremiEngine::Physics::PhysicsModule* (*CREATE_PHYSICS_MODULE)(const DoremiEngine::Core::SharedContext&);
    PHYSICS_DLL_EXPORT DoremiEngine::Physics::PhysicsModule*
        CreatePhysicsModule(const DoremiEngine::Core::SharedContext& p_context);
}