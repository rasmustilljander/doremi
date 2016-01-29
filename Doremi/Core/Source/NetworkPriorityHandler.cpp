#include <Doremi/Core/Include/NetworkPriorityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/RigidBodyComponent.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <algorithm>
#include <iostream>


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


            // Check new sleeping objects
            std::vector<int>& t_objectIDSleeping = t_rigidManager.GetRecentlySleepingObjects();
            size_t t_numOfObjects = t_objectIDSleeping.size();
            for(size_t i = 0; i < t_numOfObjects; i++)
            {
                m_netPriorityObjects[t_objectIDSleeping[i]].SetToStartSleep();
            }


            // Update all priorities
            for(size_t i = 0; i < lastEntityID; i++)
            {
                if(t_entityHandler.HasComponents(i, (int)ComponentType::NetworkObject))
                {
                    NetworkObjectComponent* netObject = &m_netPriorityObjects[i];

                    // Get position of object
                    DirectX::XMVECTOR t_objectPosVec = DirectX::XMLoadFloat3(&(GetComponent<TransformComponent>(i)->position));

                    // Get a distance
                    DirectX::XMFLOAT3 t_length;
                    DirectX::XMStoreFloat3(&t_length, DirectX::XMVector3Length(DirectX::XMVectorSubtract(t_objectPosVec, t_playerPosVec)));

                    // How will we see if it's active or not, we can see if position changed, velocity changed?
                    if(t_entityHandler.HasComponents(i, (int)ComponentType::RigidBody))
                    {
                        if(t_rigidManager.IsSleeping(i))
                        {
                            // If we're sleeping set to sleepiong
                            netObject->SetFramePriorityBySleep(t_length.x);
                            continue;
                        }
                    }

                    // Update priority
                    netObject->UpdatePriorityFromPlayer();


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
                // TODOCM might put in !t_rigidManager.IsSleeping(i) here ,
                // however if we spawn a lot of objects those objects who were excluded during this time might not recieve their final position update
                // TODOCM if we want ever lasting objects on the ground we will need to exclude probobly
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
            m_netPriorityObjects[p_entityID].SetToStartAlive();
        }

        void NetworkPriorityHandler::WriteObjectsByPriority(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten)
        {
            // Set position so we can get here later
            uint32_t positionToWriteNumOfObjects = op_BytesWritten;

            // Move forward for header (14 byte + numOfObjects to be written later
            p_streamer.SetReadWritePosition(op_BytesWritten + sizeof(uint8_t));
            op_BytesWritten += sizeof(uint8_t);

            EntityHandler& EntityHandler = EntityHandler::GetInstance();

            size_t t_NumOfObjects = m_idByPriorityList.size();
            size_t t_SizeOfOneObject = sizeof(uint32_t) + sizeof(float) * 3 + sizeof(float) * 4; // TODOCM update this when we update the quat writer

            // We want to count the objects as well so we set this outside
            size_t objectNum = 0;

            // Loop over list and add as many as possible
            for(; objectNum < t_NumOfObjects && p_bufferSize > (op_BytesWritten + t_SizeOfOneObject); objectNum++)
            {
                EntityID entityID = m_idByPriorityList[objectNum];

                // Get transform and write it
                TransformComponent* TransComponent = EntityHandler.GetComponentFromStorage<TransformComponent>(entityID);
                p_streamer.WriteUnsignedInt32(entityID); // 4 byte
                p_streamer.WriteFloat3(TransComponent->position); // 4x3 = 12 byte
                p_streamer.WriteRotationQuaternion(TransComponent->rotation); // 4x4 = 16 byte

                // Reset sent objects priority
                m_netPriorityObjects[entityID].ResetPriority();

                // Increase offset
                op_BytesWritten += t_SizeOfOneObject;
            }

            // Write how many objects there is in the snapshot
            p_streamer.SetReadWritePosition(positionToWriteNumOfObjects);
            p_streamer.WriteUnsignedInt8(objectNum);

            // Set position again
            p_streamer.SetReadWritePosition(op_BytesWritten);
        }
    }
}