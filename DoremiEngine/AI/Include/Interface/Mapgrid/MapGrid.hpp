#pragma once
#include <DirectXMath.h>

namespace DoremiEngine
{
    namespace AI
    {
        /**
        Contains information on a mapgrid of positions
        */
        class MapGrid
        {
        public:
            virtual void SetMapGrid() = 0;
        };
    }
}
