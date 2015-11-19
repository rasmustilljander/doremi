#pragma once
#include <map>
typedef int EntityID;

#define MAX_NUM_ENTITIES 100000

enum class ComponentType : int
{
	None = 0x00,
	Transform = 0x01,
	Render = 0x02,
	Physics = 0x04,
	Sound = 0x08,
	Velocity = 0x10,
};

enum class Blueprints : int
{
	FirstEntity,
};

typedef std::map<ComponentType, void*> EntityBlueprint;