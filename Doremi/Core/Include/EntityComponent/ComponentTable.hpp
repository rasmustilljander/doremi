#pragma once
// Project specific
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

// Standard libraries
#include <string>

using namespace std;
// bit lines for each entity
// 1 if have component, 0 if not
namespace Doremi
{
    namespace Core
    {
        class ComponentTable
        {
        public:
            static ComponentTable* GetInstance();

            /** Checks if entity has components specified in the bitmask*/
            bool HasComponent(EntityID pEntityID, int pMask);
            /** Adds components in bitmask to entity*/
            void AddComponent(EntityID pEntityID, int pMask);
            /** Removeds components in bitmask from entity*/
            void RemoveComponent(EntityID pEntityID, int pMask);
            /** Clears bitmask for entity (data remains, but is unused)*/
            void RemoveEntity(EntityID pEntityID);


        private:
            ComponentTable();
            ~ComponentTable();
            static ComponentTable* mSingleton;

            int mComponentTable[MAX_NUM_ENTITIES];
        };
    }
}
