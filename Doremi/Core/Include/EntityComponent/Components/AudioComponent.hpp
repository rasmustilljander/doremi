#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        Only intended as an example of what a component might look like
        Should not be used anywhere. Could be removed once exmaples are no longer necessary*/
        struct AudioComponent
        {
            float posX;
            float posY;
            float x, y, z;
            size_t channelID;
            size_t soundID;
            AudioComponent(float p_posX, float p_posY, float p_x, float p_y, float p_z) : posX(p_posX), posY(p_posY), x(p_x), y(p_y), z(p_z)
            {
            }
            AudioComponent() : posX(0), posY(0)
            {
            }
        };
    }
}