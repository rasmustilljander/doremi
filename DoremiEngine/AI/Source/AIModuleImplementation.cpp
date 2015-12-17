#include <Internal/AIModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Internal/SubModule/AStarSubModuleImpl.hpp>
#include <Internal/SubModule/PotentialFieldSubModuleImpl.hpp>
#include <iostream>
#include <windows.h>

namespace DoremiEngine
{
    namespace AI
    {
        AIModuleImplementation::AIModuleImplementation(const Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

        AIModuleImplementation::~AIModuleImplementation() {}


        void AIModuleImplementation::Startup()
        {
            m_aStarSubModuleImpl = new AStarSubModuleImpl();
            m_potentialFieldSubModuleImpl = new PotentialFieldSubModuleImpl();
        }


        void AIModuleImplementation::Shutdown() {}

        void AIModuleImplementation::Update() {}
        AStarSubModule& AIModuleImplementation::GetAstarSubModule() { return *m_aStarSubModuleImpl; }
        PotentialFieldSubModule& AIModuleImplementation::GetPotentialFieldSubModule() { return *m_potentialFieldSubModuleImpl; }
    }
}

DoremiEngine::AI::AIModule* CreateAIModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::AI::AIModule* AI = new DoremiEngine::AI::AIModuleImplementation(p_sharedContext);
    return AI;
}