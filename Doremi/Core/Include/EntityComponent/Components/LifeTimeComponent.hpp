#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        After the given seconds the entity will be removed
        */
        struct LifeTimeComponent
        {
            float LifeTime = 0; // In seconds
            LifeTimeComponent(){};
        };
    }
}