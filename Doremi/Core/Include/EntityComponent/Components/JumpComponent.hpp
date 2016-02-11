#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        Contains information that allows entities to jump. Component can always be active on an
        entity, even when it is not currently jumping. StartJump-method can be used to start
        a jump, or manually setting the variables to sensible things.*/
        struct JumpComponent
        {
            // States whether a jump is currently under way
            bool active = false;
            // Remaining movement upwards of the jump. This is updated during the jump
            float movementRemaining = 0;
            // Intensity of the jump. This is constant across the jump
            float intensity = 0;

            void StartJump()
            {
                if(!active)
                {
                    active = true;
                    movementRemaining = intensity;
                }
            }
        };
    }
}