// This class
#include <PlayerClasses/GunController.hpp>
/// Engine
// Core
#include <DoremiEngine/Core/Include/SharedContext.hpp>
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/FluidManager.hpp>
/// Game
// Handlers
#include <EntityComponent/EntityHandler.hpp>
// Components
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/EntityTypeComponent.hpp>
#include <EntityComponent/Components/PressureParticleComponent.hpp>

// Third party
#include <DirectXMath.h>

using namespace DirectX;
namespace Doremi
{
    namespace Core
    {
        GunController::GunController() {}
        GunController::~GunController() {}

        void GunController::FireGun(int p_playerID, const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            GetComponent<ParticlePressureComponent>(p_playerID)->data.m_position = GetComponent<TransformComponent>(p_playerID)->position;
            GetComponent<ParticlePressureComponent>(p_playerID)->data.m_direction = GetComponent<TransformComponent>(p_playerID)->rotation;
            GetComponent<ParticlePressureComponent>(p_playerID)->data.m_active = true;
            p_sharedContext.GetPhysicsModule().GetFluidManager().SetParticleEmitterData(p_playerID, GetComponent<ParticlePressureComponent>(p_playerID)->data);
        }
    }
}