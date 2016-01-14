#pragma once
#include <AIModule.hpp>
#include <FMod\fmod.hpp>
#include <FMod\fmod_errors.h>
#include <vector>
#include <DirectXMath.h>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
    namespace AI
    {
        class PotentialFieldSubModuleImpl;
        class AStarSubModuleImpl;
        class AIModuleImplementation : public AIModule
        {
        public:
            /**
                TODO docs
            */
            AIModuleImplementation(const Core::SharedContext& p_sharedContext);
            /**
                TODO docs
            */
            virtual ~AIModuleImplementation();
            /**
                TODO docs
            */
            void Startup() override;

            /**
                TORORT docs
            */
            void Update() override;

            /**
                TODO docs
            */
            void Shutdown() override;

            PotentialFieldSubModule& GetPotentialFieldSubModule() override;

            AStarSubModule& GetAstarSubModule() override;


        private:
            const Core::SharedContext& m_sharedContext;
            PotentialFieldSubModuleImpl* m_potentialFieldSubModuleImpl;
            AStarSubModuleImpl* m_aStarSubModuleImpl;
        };
    }
}
