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
        /**
            A function-class that helps using member variables when comparing elements, used for comparing priorty between objects
        */
        class FunctorPriority
        {
        public:
            FunctorPriority(NetworkObjectComponent* p_netPriorityObjects) : m_netPriorityObjects(p_netPriorityObjects) {}

            /**
                Compares two entityID and returns the one with the highest priority
            */
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


        /**
            Each player uses one NetworkPriorityHandler that contains priorities for each element in the game.
        */
        class NetworkPriorityHandler
        {
        public:
            NetworkPriorityHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~NetworkPriorityHandler();

            /**
                Update all priorities and check if relevant or not
            */
            void Update(EntityID p_playerIDEntityID, double p_dt);

            /**
                Update a specific object with a priority template
            */
            void UpdateNetworkObject(const EntityID& p_entityID);

            /**
                Write objects by priority to a buffer, also removes their priority if sent. Update should've been called between.
            */
            void WriteObjectsByPriority(BitStreamer& p_streamer, uint32_t p_bufferSize, uint32_t& op_BytesWritten);

        private:
            /**
                Sharedcontext for calling physicx module
            */
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            /**
                Component per object, per player, contains priority and timers
            */
            NetworkObjectComponent m_netPriorityObjects[MAX_NUM_ENTITIES];

            /**
                List of IDs that will be sorted by priority
            */
            std::vector<EntityID> m_idByPriorityList;

            /**
                Time for how long time untill a unrelevant object will be excluded
            */
            float RelevantTimer;

            /**
                Time to when a object is always included
            */
            float ShortRelevantTimer;

            /**
                Range of where we deem a object unrelevant
                TODOCM Crete a function to set CullRange
            */
            float CullRange;
        };
    }
}
