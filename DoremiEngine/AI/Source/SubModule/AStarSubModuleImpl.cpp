#pragma once
#include <Internal/SubModule/AStarSubModuleImpl.hpp>
#include <Internal/Mapgrid/MapGridImpl.hpp>

namespace DoremiEngine
{
    namespace AI
    {
        AStarSubModuleImpl::AStarSubModuleImpl(){};
        AStarSubModuleImpl::~AStarSubModuleImpl(){};
        void AStarSubModuleImpl::GetPath(int p_startPos, const MapGrid& p_map) {}
        MapGrid* AStarSubModuleImpl::BuildMapGrid()
        {
            MapGrid* t_map = new MapGridImpl();
            return t_map;
        }
    }
}
