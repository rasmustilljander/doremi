#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        Contains information on the range of the entity, useful for checking proximity
        */
        struct RangeComponent
        {
            float range;
            RangeComponent() : range(0) {}
        };
    }
}