#pragma once
namespace Doremi
{
    namespace Core
    {
        enum class CharacterEffect
        {
            None = 0x00,
            IceSliding = 0x10,
            CantStop = 0x20,
        };

        /**
        Stores information regarding which effects a character is currently
        affected by*/
        struct CharacterEffectComponent
        {
            CharacterEffect effect = CharacterEffect::None;
        };
    }
}