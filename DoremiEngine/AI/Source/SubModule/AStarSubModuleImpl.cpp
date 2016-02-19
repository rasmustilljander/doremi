#pragma once
#include <Internal/SubModule/AStarSubModuleImpl.hpp>
#include <Internal/Mapgrid/MapGridImpl.hpp>

namespace DoremiEngine
{
    namespace AI
    {
        AStarSubModuleImpl::AStarSubModuleImpl(AIContext& p_aiContext) : m_context(p_aiContext){};
        AStarSubModuleImpl::~AStarSubModuleImpl(){};
        void AStarSubModuleImpl::GetPath(int p_startPos, const MapGrid& p_map) {}
        MapGrid* AStarSubModuleImpl::BuildMapGrid()
        {
            MapGrid* t_map = new MapGridImpl();
            return t_map;
        }
    }
}
