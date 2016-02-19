#pragma once
#include <Interface/SubModule/AStarSubModule.hpp>
#include <Internal/AIContext.hpp>

namespace DoremiEngine
{
    namespace AI
    {
        class AStarSubModuleImpl : public AStarSubModule
        {
        public:
            AStarSubModuleImpl(AIContext& p_aiContext);
            ~AStarSubModuleImpl();
            void GetPath(int p_startPos, const MapGrid& p_map);
            MapGrid* BuildMapGrid();

        private:
            AIContext& m_context;
        };
    }
}
