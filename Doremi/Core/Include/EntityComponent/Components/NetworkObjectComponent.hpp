#pragma once
#include <stdint.h>


#define FORCE_UPDATE_VALUE 360.0f

namespace Doremi
{
    namespace Core
    {
        /**
            Component used for each object for each player, used for prioritizeing net objects
        */
        struct NetworkObjectComponent
        {
            NetworkObjectComponent() : Priority(0.0f), FramePriority(0.0f), LastUpdateTimer(0.0f), NotRelevantTimer(0.0f), RelevantTimer(0.0f) {}
            NetworkObjectComponent(float p_priorityStart)
                : Priority(p_priorityStart), FramePriority(0), LastUpdateTimer(0.0f), NotRelevantTimer(0.0f), RelevantTimer(0.0f)
            {
            }

            /**
                Get the priority for the current frame
            */
            const float& GetFramePriority() { return FramePriority; }

            /**
                Update priority, needed for starvation
            */
            void UpdateLastUpdateTimer(const double& p_dt) { LastUpdateTimer += p_dt; }

            /**
                Update not relevant timer, if higher then a value, will be exluded
            */
            void UpdateToNotRelevant(const double& p_dt)
            {
                RelevantTimer = 0.0f;
                NotRelevantTimer += p_dt;
            }

            /**
                Update relevant timer, if lower then a value, will always be included
            */
            void UpdateToRelevant(const double& p_dt)
            {
                NotRelevantTimer = 0.0f;
                RelevantTimer += p_dt;
            }

            /**
                Get NotRelevantTimer
            */
            float GetNotRelevantTimer() { return NotRelevantTimer; }

            /**
                Get RelevantTimer
            */
            float GetRelevantTimer() { return RelevantTimer; }

            /**
                Calculate priority by time it hasn't been send
            */
            void CalculateFramePriority() { FramePriority = LastUpdateTimer * Priority; }

            /**
                Calculate priority with a forced value, will try to include this in next package
            */
            void CalculateFramePriorityByShortRelevance()
            {
                LastUpdateTimer += FORCE_UPDATE_VALUE;
                CalculateFramePriority();
            }

            /**
                Reset the last update timer
            */
            void ResetLastUpdateTimer() { LastUpdateTimer = 0.0f; }

        private:
            /**
                Static priority for object
            */
            float Priority;

            /**
                Frame priority, based on time from last update and Priority
            */
            float FramePriority;

            /**
                Timer from last update, used for remove starvation
            */
            float LastUpdateTimer;

            /**
                Timer to check not relevant
            */
            float NotRelevantTimer;

            /**
                Timer to check relevant
            */
            float RelevantTimer;
        };
    }
}