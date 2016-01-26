#pragma once
#include <Doremi/Core/Include/EntityComponent/Components/NetworkObjectComponent.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <Doremi/Core/Include/Manager/Network/BitStreamer.h>
#include <list>
#include <vector>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        class FunctorPriority
        {
        public:
            FunctorPriority(NetworkObjectComponent* p_netPriorityObjects) : m_netPriorityObjects(p_netPriorityObjects) {}

            bool operator()(const EntityID& p_id1, const EntityID& p_id2)
            {
                // Get the objects
                NetworkObjectComponent& obj1 = m_netPriorityObjects[p_id1];
                NetworkObjectComponent& obj2 = m_netPriorityObjects[p_id2];

                if(obj1.GetFramePriority() > obj2.GetFramePriority())
                {
                    return true;
                }
                return false;
            }

            NetworkObjectComponent* m_netPriorityObjects;
        };

        class NetworkPriorityHandler
        {
        public:
            /**
                TODOCM doc
            */
            NetworkPriorityHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                TODOCM doc
            */
            ~NetworkPriorityHandler();

            /**
                TODOCM doc
            */
            void Update(EntityID p_playerIDEntityID);

            /**
                TODOCM doc
            */
            void UpdateNetworkObject(const EntityID& p_entityID);

            /**
                TODOCM doc
            */
            void WriteObjectsByPriority(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten);

        private:
            /**
                TODOCM doc
            */
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            /**
                TODOCM doc
            */
            NetworkObjectComponent m_netPriorityObjects[MAX_NUM_ENTITIES];

            /**
                TODOCM doc
            */
            std::vector<EntityID> m_idByPriorityList;
        };
    }
}
