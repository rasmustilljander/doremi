#pragma once
namespace DoremiEngine
{
    namespace AI
    {
        /**
        TODOKO docs
        */
        class MapGrid;
        class AStarSubModule
        {
            public:
            virtual void GetPath(int p_startPos, const MapGrid& p_map) = 0;
            virtual MapGrid* BuildMapGrid() = 0;
        };
    }
}
