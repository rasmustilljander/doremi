#pragma once
#include <stdint.h>
#include <minmax.h>

#define PRIORITY_DISTANCE_DIV_FACTOR 0.001f
#define PRIORITY_FROM_SPAWN 100000000.0f
#define PRIORITY_FROM_SLEEP 10000000.0f
#define NUM_SENDS_BEFOR_SLEEP 30
#define NUM_SENDS_AFTER_SPAWN 120

namespace Doremi
{
    namespace Core
    {
        /**
            TODOCM doc
        */
        struct NetworkObjectComponent
        {
            NetworkObjectComponent() : PriorityStart(0), PriorityIncrement(0), Priority(0), FramePriority(0), SleepUpdatesLeft(0), SpawnUpdatesLeft(0)
            {
            }
            NetworkObjectComponent(float p_priorityStart, float p_priorityIncrement)
                : PriorityStart(p_priorityStart), PriorityIncrement(p_priorityIncrement), Priority(p_priorityStart), FramePriority(0), SleepUpdatesLeft(0)
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
            void SetFramePriorityBySleep(float p_length)
            {
                if(!SleepUpdatesLeft)
                {
                    FramePriority = 1;
                }
                else
                {
                    SetFramePriorityByLengthAndAddValue(p_length, PRIORITY_FROM_SLEEP);
                }
            }

            /**
                Calculate priority by distance and set it as frame priority, will be 2 if too far
            */
            void SetFramePriorityByLength(float p_length)
            {
                if(!SpawnUpdatesLeft)
                {
                    SetFramePriorityByLengthAndAddValue(p_length, 0.0f);
                }
                else
                {
                    SetFramePriorityByLengthAndAddValue(p_length, PRIORITY_FROM_SPAWN);
                }
            }

            void SetFramePriorityByLengthAndAddValue(float p_length, float p_value)
            {
                if(p_length > 1)
                {
                    FramePriority = max(Priority /*/ (p_length * PRIORITY_DISTANCE_DIV_FACTOR)*/, 2.0f);
                }
                else
                {
                    FramePriority = Priority /* / PRIORITY_DISTANCE_DIV_FACTOR*/;
                }
            }

            void SetToStartSleep()
            {
                SleepUpdatesLeft = NUM_SENDS_BEFOR_SLEEP;
                SpawnUpdatesLeft = 0;
            }

            void SetToStartAlive() { SpawnUpdatesLeft = NUM_SENDS_AFTER_SPAWN; }

            /**
                TODOCM doc
            */
            void ResetPriority()
            {
                Priority = PriorityStart;

                // Decrement sleepupdates if we have any
                if(SleepUpdatesLeft)
                {
                    SleepUpdatesLeft--;
                }
                if(SpawnUpdatesLeft)
                {
                    SpawnUpdatesLeft--;
                }
            }

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

            /**
                TODOCM doc
            */
            uint8_t SleepUpdatesLeft;

            /**
                TODOCM doc
            */
            uint8_t SpawnUpdatesLeft;
        };
    }
}