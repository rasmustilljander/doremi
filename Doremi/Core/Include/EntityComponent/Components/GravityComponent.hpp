#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        Only intended as an example of what a component might look like
        Should not be used anywhere. Could be removed once exmaples are no longer necessary*/
        struct GravityComponent
        {
            // Current speed at which the entity is falling due to gravity
            float travelSpeed = 0;
            float maxFallSpeed = 200; // Hard coded to avoid rocket speed
        };
    }
}