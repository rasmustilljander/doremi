#pragma once

// Standard libraries
#include <map>

typedef int EntityID;

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
    VoiceRecording = 0x10,
    Render = 0x20,
    Transform = 0x40
};

/**
Lists all available types of entities, as blueprints.
WARNING! Subject to change due to low scalability*/
enum class Blueprints : int
{
    ExampleEntity,
    VoiceRecordEntity,
    RenderExampleEntity,
};

/**
Maps a ComponentType to a void*, where the void* is an
instance of the desired component which will later be compied
when creating an instance of the said blueprint*/
typedef std::map<ComponentType, void*> EntityBlueprint;