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
            // The total time a jump takes (constant across a jump)
            float jumpTime = 0;
            // How intense a jump is. That is the power behind it (constant across a jump)
            float jumpIntensity = 0;
            // Progress of the current jump (updates each frame until jumpTime is reached)
            float jumpTimeProgress = 0;

            // Starts a jump by setting active to true, and resetting jumpProgress
            void StartJump()
            {
                active = true;
                jumpTimeProgress = 0;
            }
        };
    }
}