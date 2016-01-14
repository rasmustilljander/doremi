#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DirectXMath.h>

#if defined(_WINDLL)
#define AI_DLL_EXPORT __declspec(dllexport)
#else
#define AI_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldSubModule;
        class AStarSubModule;
        /**
        TODO docs
        */
        class AIModule : public DoremiEngine::Core::EngineModule
        {
        public:
            /**
            Updates the AImodule
            */
            virtual void Update() = 0;

            /**
            TODO DOCS
            */
            virtual void Startup() = 0;

            /**
            TODO docs
            */
            virtual void Shutdown() = 0;

            /**
            Returns a pointer to the potentialfieldsubmodule
            */
            virtual PotentialFieldSubModule& GetPotentialFieldSubModule() = 0;

            /**
            Returns a pointer to the AStarSubModule
            */
            virtual AStarSubModule& GetAstarSubModule() = 0;
        };
    }
}

extern "C" {
typedef DoremiEngine::AI::AIModule* (*CREATE_AI_MODULE)(const DoremiEngine::Core::SharedContext&);
AI_DLL_EXPORT DoremiEngine::AI::AIModule* CreateAIModule(const DoremiEngine::Core::SharedContext& p_context);
}