#pragma once

// Standard libraries
#include <map>

typedef uint32_t EntityID;

// Maximum number of entities allowed in the game
#define MAX_NUM_ENTITIES 100000

/** Lists all components. Follows bitmask pattern (2's power)*/
enum class ComponentType : int
{
    None = 0x00,
    Example = 0x01,
    Example2 = 0x02,
    Audio = 0x04,
    AudioActive = 0x08,
    FrequencyAffected = 0x10,
    Render = 0x20,
    Transform = 0x40,
    RigidBody = 0x80,
    PhysicalMaterial = 0x100,
    Player = 0x200,
    Movement = 0x400,
    AIAgent = 0x800,
    Health = 0x1000,
    Range = 0x2000,
    PotentialField = 0x4000,
    CharacterController = 0x8000,
    NetworkObject = 0x10000,
    AIGroup = 0x20000,
    Jump = 0x40000,
    Gravity = 0x80000,
    EntityType = 0x100000,
    PressureParticleSystem = 0x200000,
};

/**
Lists all available types of entities, as blueprints.
WARNING! Subject to change due to low scalability*/
enum class Blueprints : uint32_t
{
    EmptyEntity,
    ExampleEntity,
    VoiceRecordEntity,
    RenderExampleEntity,
    PlayerEntity,
    PlatformEntity,
    BulletEntity,
    EnemyEntity,
    JawsDebugEntity, // TODOCM remove
    JawsDebugEntity2, // TODOCM remove
    StaticEnviromentEntity,
    ExperimentalPressureParticleEntity,
};


/**
Maps a ComponentType to a void*, where the void* is an
instance of the desired component which will later be compied
when creating an instance of the said blueprint*/
typedef std::map<ComponentType, void*> EntityBlueprint;