#pragma once
namespace Doremi
{
    namespace Core
    {
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