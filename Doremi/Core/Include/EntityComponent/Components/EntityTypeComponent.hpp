#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        Used for specifying what type of entity it is, if needed
        */
        enum class EntityType
        {
            None = 0x0,
            Enemy = 0x1,
            EnemyBullet = 0x2,
            FriendlyBullet = 0x4,
        };

        struct EntityTypeComponent
        {
            EntityType type = EntityType::None;
        };
    }
}