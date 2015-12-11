#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        Only intended as an example of what a component might look like
        Should not be used anywhere. Could be removed once exmaples are no longer necessary*/
        struct PlayerComponent
        {
            bool isControllable;
            PlayerComponent() {}
            PlayerComponent(bool p_isControllable) : isControllable(p_isControllable) {}
        };
    }
}