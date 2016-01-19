// Project specific
#include <Manager/FrequencyAffectedObjectManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>
#include <Doremi/Core/Include/Helper/ProximityChecker.hpp>

#include <DirectXMath.h>
// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        FrequencyAffectedObjectManager::FrequencyAffectedObjectManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "FrequencyAffectedObjectManager")
        {
        }

        FrequencyAffectedObjectManager::~FrequencyAffectedObjectManager() {}


        void FrequencyAffectedObjectManager::Update(double p_dt)
        {
            // First get the playermap from player handler
            std::map<uint32_t, Player*> t_playerMap = PlayerHandler::GetInstance()->GetPlayerMap();

            for(auto playerID = t_playerMap.begin(); playerID != t_playerMap.end(); playerID++)
            {
                // Get the players current frequency
                // playerID->second->GetFrequency

                // Loop through all entities
                const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
                // Loop over all entities to perform various functions on enteties that have sound components
                for(size_t j = 0; j < length; j++)
                {
                    // Check if entity will be affected by the frequencyanalyser.
                    if(EntityHandler::GetInstance().HasComponents(j, (int)ComponentType::FrequencyAffected))
                    {
                        if(ProximityChecker::GetInstance().CheckProximityToEntity(playerID->second->m_playerEntityID, j))
                        {
                            // Update j transform component in some way using the frequency we got earlier
                        }
                    }
                }
            }
        }
    }
}
