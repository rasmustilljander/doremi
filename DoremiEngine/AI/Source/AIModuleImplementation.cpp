#include <Internal/AIModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Sub Modules
#include <Internal/SubModule/AStarSubModuleImpl.hpp>
#include <Internal/SubModule/PotentialFieldSubModuleImpl.hpp>

// Standard
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
            m_context = new AIContext(m_sharedContext.GetConfigurationModule());
            m_aStarSubModuleImpl = new AStarSubModuleImpl(*m_context);
            m_potentialFieldSubModuleImpl = new PotentialFieldSubModuleImpl(*m_context);
            m_context->AStarModule = m_aStarSubModuleImpl;
            m_context->PFModule = m_potentialFieldSubModuleImpl;
            m_context->Module = this;
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