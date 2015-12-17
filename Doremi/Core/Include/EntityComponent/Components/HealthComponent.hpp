#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        Contains information on position and rotation
        */
        struct HealthComponent
        {
            float currentHealth;
            float maxHealth;
            HealthComponent() : currentHealth(0), maxHealth(0) {} // TODOJB rotation cannot be zero-vector. Is now 0,0,1,0
        };
    }
}