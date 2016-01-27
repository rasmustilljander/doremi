namespace Doremi
{
    namespace Core
    {
        enum class DamageTypes : size_t
        {
            None = 1,
            FriendlyBullet = 2,
            EnemyBullet = 4,
            FallDamage = 8,
        };
        struct DamageInflictorsComponent
        {
            DamageTypes NoImmunityAgainst;
            DamageInflictorsComponent(DamageTypes p_notImmune) : NoImmunityAgainst(p_notImmune) {}
            DamageInflictorsComponent() : NoImmunityAgainst(DamageTypes::None) {}
        };
    }
}