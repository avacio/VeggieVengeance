#pragma once

#include "common.hpp"
#include "boundingBox.hpp"
class DamageEffect
{
public:
	DamageEffect(int xpos, int ypos, int width, int height, unsigned int damage, int id, DeletionTime delete_when);
	BoundingBox bounding_box;
	unsigned int damage;
	int id;
	DeletionTime delete_when;
	bool hit_fighter;
};