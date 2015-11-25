#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        Only intended as an example of what a component might look like
        Should not be used anywhere. Could be removed once exmaples are no longer necessary*/
        struct ExampleComponent
        {
            float posX;
            float posY;
            ExampleComponent(float p_posX, float p_posY) : posX(p_posX), posY(p_posY)
            {
            }
            ExampleComponent() : posX(0), posY(0)
            {
            }
        };
    }
}