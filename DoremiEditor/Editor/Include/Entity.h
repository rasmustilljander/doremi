#pragma once

#ifndef ENTITY_H
#define ENTITY_H
#endif

#include "ObjectData.h"

class Entity {
public:
	void *nameP = nullptr;

	void NameMessageChange(NameMessage *mm)
	{
		if (nameP != nullptr)
		{
			free(nameP);
		}
		nameP = mm;
	}
};