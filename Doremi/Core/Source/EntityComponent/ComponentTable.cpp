// Project specific
#include <EntityComponent/ComponentTable.hpp>

namespace Doremi
{
    namespace Core
    {
        ComponentTable* ComponentTable::GetInstance()
        {
            static ComponentTable componentTable;
            return &componentTable;
        }

        ComponentTable::ComponentTable()
        {
            // Sets memory for the component bitmask to 0. No components for any entity
            memset(mComponentTable, 0, sizeof(mComponentTable));
        }


        ComponentTable::~ComponentTable() {}

        bool ComponentTable::HasComponent(EntityID pEntityID, int pMask)
        {
            int tLineToCheck = mComponentTable[pEntityID];

            if((tLineToCheck & pMask) == pMask) // then there are the components from mask
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        void ComponentTable::AddComponent(EntityID pEntityID, int pMask)
        {
            // Adds a new component to the bit mask in the component table
            int tLineToAddTo = mComponentTable[pEntityID];

            tLineToAddTo = tLineToAddTo | pMask;
            mComponentTable[pEntityID] = tLineToAddTo;
        }

        void ComponentTable::RemoveComponent(EntityID pEntityID, int pMask)
        {
            int tLineToRemoveFrom = mComponentTable[pEntityID];

            int tAnd = tLineToRemoveFrom & pMask;

            tLineToRemoveFrom = tLineToRemoveFrom ^ tAnd;
            mComponentTable[pEntityID] = tLineToRemoveFrom;
        }

        void ComponentTable::RemoveEntity(EntityID pEntityID) { mComponentTable[pEntityID] = 0; }
    }
}