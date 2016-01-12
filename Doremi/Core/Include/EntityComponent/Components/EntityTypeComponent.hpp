#pragma once
namespace Doremi
{
    namespace Core
    {
        /**
        The audio component contains the handle to the soundchannel and a handle to the sound
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