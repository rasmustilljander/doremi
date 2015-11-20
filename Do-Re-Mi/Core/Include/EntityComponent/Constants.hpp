#pragma once
#include <map>
typedef int EntityID;

#define MAX_NUM_ENTITIES 100000

enum class ComponentType : int
{
	None = 0x00,
	Physics = 0x01,
};

enum class Blueprints : int
{
	FirstEntity,
};

typedef std::map<ComponentType, void*> EntityBlueprint;