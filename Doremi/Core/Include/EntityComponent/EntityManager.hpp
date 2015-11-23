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

            EntityID AddEntity();
            void RemoveEntity(int pEntityID);
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