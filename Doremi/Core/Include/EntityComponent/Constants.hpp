#pragma once

// Standard libraries
#include <map>

typedef int EntityID;
#define MAX_NUM_ENTITIES 100000

enum class ComponentType : int
{
    None = 0x00,
    Example = 0x01,
};

enum class Blueprints : int
{
    ExampleEntity,
};

typedef std::map<ComponentType, void*> EntityBlueprint;