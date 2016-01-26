#include <Doremi/Core/Include/NetworkPriorityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/RigidBodyComponent.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <algorithm>


namespace Doremi
{
    namespace Core
    {
        NetworkPriorityHandler::NetworkPriorityHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            // Copy all current network objects
            uint32_t t_numEntities = EntityHandler::GetInstance().GetLastEntityIndex();
            memcpy(m_netPriorityObjects, GetComponent<NetworkObjectComponent>(0), sizeof(NetworkObjectComponent) * t_numEntities);

            // Reserve memory for the maximum number of elements
            m_idByPriorityList.reserve(t_numEntities);
        }

        NetworkPriorityHandler::~NetworkPriorityHandler() {}

        void NetworkPriorityHandler::Update(EntityID p_playerID)
        {
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();
            DoremiEngine::Physics::RigidBodyManager& t_rigidManager = m_sharedContext.GetPhysicsModule().GetRigidBodyManager();

            size_t lastEntityID = t_entityHandler.GetLastEntityIndex();
            DirectX::XMVECTOR t_playerPosVec = DirectX::XMLoadFloat3(&(GetComponent<TransformComponent>(p_playerID)->position));

            // Update all priorities
            for(size_t i = 0; i < lastEntityID; i++)
            {
                if(t_entityHandler.HasComponents(i, (int)ComponentType::NetworkObject))
                {
                    NetworkObjectComponent* netObject = &m_netPriorityObjects[i];

                    // Update priority
                    netObject->UpdatePriorityFromPlayer();

                    // How will we see if it's active or not, we can see if position changed, velocity changed?
                    if(t_entityHandler.HasComponents(i, (int)ComponentType::RigidBody))
                    {
                        if(t_rigidManager.IsSleeping(GetComponent<RigidBodyComponent>(i)->p_bodyID))
                        {
                            // If we're sleeping set to sleepiong
                            netObject->SetFramePriorityBySleep();
                            continue;
                        }
                    }

                    // Get position of object
                    DirectX::XMVECTOR t_objectPosVec = DirectX::XMLoadFloat3(&(GetComponent<TransformComponent>(i)->position));

                    // Get a distance
                    DirectX::XMFLOAT3 t_length;
                    DirectX::XMStoreFloat3(&t_length, DirectX::XMVector3Length(DirectX::XMVectorSubtract(t_objectPosVec, t_playerPosVec)));

                    // If we're not sleeping set frame priority
                    netObject->SetFramePriorityByLength(t_length.x);
                }
            }

            // TODOCM might save old and only add new objects into list as they come
            // Clear old
            m_idByPriorityList.clear();

            // Fill with ID
            for(size_t i = 0; i < lastEntityID; i++)
            {
                if(t_entityHandler.HasComponents(i, (int)ComponentType::NetworkObject))
                {
                    m_idByPriorityList.push_back(i);
                }
            }

            // Sort by priority
            FunctorPriority functorPriority(m_netPriorityObjects);
            std::sort(m_idByPriorityList.begin(), m_idByPriorityList.end(), functorPriority);
        }

        void NetworkPriorityHandler::UpdateNetworkObject(const EntityID& p_entityID)
        {
            memcpy(&m_netPriorityObjects[p_entityID], GetComponent<NetworkObjectComponent>(p_entityID), sizeof(NetworkObjectComponent));
        }
    }
}