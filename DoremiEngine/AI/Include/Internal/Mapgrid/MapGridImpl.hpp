#pragma once
#include <Interface/Mapgrid/MapGrid.hpp>

namespace DoremiEngine
{
    namespace AI
    {
        class MapGridImpl : public MapGrid
        {
        public:
            MapGridImpl();
            virtual ~MapGridImpl();
            void SetMapGrid();
        };
    }
}
