#pragma once

namespace Doremi
{
    namespace Core
    {
        /**
        Contains different timers that the AI needs. Everything is in seconds
        */
        struct AIAgentComponent
        {
            float attackFrequency;
            float attackTimer = 0;
            float pathfindingFrequency;
            float pathfindingTimer = 0;

            AIAgentComponent(const float& p_attackFrequency, const float& p_pathfindingFrequency)
                : attackFrequency(p_attackFrequency), pathfindingFrequency(p_pathfindingFrequency)
            {
            }
            AIAgentComponent() : attackFrequency(0), pathfindingFrequency(0) {}
        };
    }
}