#pragma once
#include <stdint.h>
#include <minmax.h>

#define PRIORITY_DISTANCE_DIV_FACTOR 10.0f
#define PRIORITY_FROM_SPAWN 100000.0f

namespace Doremi
{
    namespace Core
    {
        /**
            TODOCM doc
        */
        struct NetworkObjectComponent
        {
            NetworkObjectComponent() : PriorityStart(0), PriorityIncrement(0), Priority(0), FramePriority(0) {}
            NetworkObjectComponent(float p_priorityStart, float p_priorityIncrement)
                : PriorityStart(p_priorityStart), PriorityIncrement(p_priorityIncrement), Priority(p_priorityStart), FramePriority(0)
            {
            }

            /**
                TODOCM doc
            */
            const float& GetFramePriority() { return FramePriority; }

            /**
                TODOCM doc
            */
            void UpdatePriorityFromPlayer() { Priority += PriorityIncrement; }

            /**
                TODOCM doc
            */
            void UpdatePriorityGlobal() {}

            /**
                Sets priority to 1, 1 above zero because excluding
            */
            void SetFramePriorityBySleep() { FramePriority = 1; }

            /**
                Calculate priority by distance and set it as frame priority, will be 2 if too far
            */
            void SetFramePriorityByLength(float p_length)
            {
                if(p_length)
                {
                    FramePriority = max(Priority / (p_length * PRIORITY_DISTANCE_DIV_FACTOR), 2.0f);
                }
                else
                {
                    FramePriority = 0;
                }
            }

            void AddPriorityForNewMovement() { Priority += PRIORITY_FROM_SPAWN; }

            /**
                TODOCM doc
            */
            void ResetPriority() { Priority = PriorityStart; }

        private:
            /**
                TODOCM doc
            */
            float FramePriority;

            /**
                TODOCM doc
            */
            float Priority;

            /**
                TODOCM doc
            */
            float PriorityIncrement;

            /**
                TODOCM doc
            */
            float PriorityStart;
        };
    }
}