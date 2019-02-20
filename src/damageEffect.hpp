#pragma once

#include "common.hpp"
#include "boundingbox.hpp"
class DamageEffect
{
public:
	DamageEffect(int xpos, int ypos, int width, int height, unsigned int damage, int id);
	BoundingBox bounding_box;
	unsigned int damage;
	int id;
};