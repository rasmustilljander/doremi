#pragma once
// Project specific
#include <EntityComponent/Constants.hpp>

// Standard libraries
#include <list>
namespace Doremi
{
    namespace Core
    {
        class EntityManager
        {
        public:
            static EntityManager* GetInstance();

            /** Adds entity and returns id of newly added entity*/
            EntityID AddEntity();
            /** Removes specified entity*/
            void RemoveEntity(int pEntityID);
            /** Returns last entity ID*/
            const int GetLastEntity()
            {
                return mNextSlot;
            }

        private:
            EntityManager();
            ~EntityManager();
            static EntityManager* mSingleton;

            std::list<EntityID> mFreeEntitySlots;
            EntityID mNextSlot;
        };
    }
}