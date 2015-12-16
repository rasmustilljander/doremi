#pragma once
#include <Interface/SubModule/AStarSubModule.hpp>

namespace DoremiEngine
{
    namespace AI
    {
        class AStarSubModuleImpl : public AStarSubModule
        {
            public:
            AStarSubModuleImpl();
            ~AStarSubModuleImpl();
            void GetPath(int p_startPos, const MapGrid& p_map);
            MapGrid* BuildMapGrid();
        };
    }
}
