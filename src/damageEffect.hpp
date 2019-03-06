#pragma once

#include "common.hpp"
#include "boundingBox.hpp"
class DamageEffect
{
public:
	DamageEffect(int xpos, int ypos, int width, int height, unsigned int damage, int id, DeletionTime delete_when);
	BoundingBox bounding_box;
	unsigned int damage; //the amount of damage this will deal
	int id; //the fighter who used this move
	DeletionTime delete_when;
	bool hit_fighter; //whether or not a fighter has been hit by this move
};