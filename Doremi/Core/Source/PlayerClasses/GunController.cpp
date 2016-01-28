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
#include <AudioHandler.hpp>
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
            /// This is very temporary code to show that we can use frequency to affect beam width
            float width;
            // Get current frequency (seems laggy?)
            float freq = AudioHandler::GetInstance()->GetFrequency();
            // If there is no frequency, we set the beam to be singular
            if(freq == 0)
            {
                width = 0;
            }
            else
            {
                // We want a width between 0 and 3.14/2 (90 degrees) sortof. This calculation is VERY non-finished
                width = (400 - freq) / 200;
            }
            GetComponent<ParticlePressureComponent>(p_playerID)->data.m_position = GetComponent<TransformComponent>(p_playerID)->position;
            GetComponent<ParticlePressureComponent>(p_playerID)->data.m_direction = GetComponent<TransformComponent>(p_playerID)->rotation;
            GetComponent<ParticlePressureComponent>(p_playerID)->data.m_emissionAreaDimensions.x = width;
            GetComponent<ParticlePressureComponent>(p_playerID)->data.m_active = true;
            p_sharedContext.GetPhysicsModule().GetFluidManager().SetParticleEmitterData(p_playerID, GetComponent<ParticlePressureComponent>(p_playerID)->data);
        }
    }
}