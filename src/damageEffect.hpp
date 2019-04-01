#pragma once

#include "common.hpp"
#include "boundingBox.hpp"
class DamageEffect
{
public:
	DamageEffect(int xpos, int ypos, int width, int height, unsigned int damage, int id, DeletionTime delete_when, float vert_force);
	DamageEffect(int xpos, int ypos, int width, int height, unsigned int damage, int id, DeletionTime delete_when, float vert_force, float ms);
	BoundingBox m_bounding_box;
	unsigned int m_damage; //the amount of damage this will deal
	int m_fighter_id; //the fighter who used this move
	DeletionTime m_delete_when;
	bool m_hit_fighter; //whether or not a fighter has been hit by this move
	float m_vert_force; //amount of vertical force the move applies to enemy hit by this
	float m_time_remain;  //time left before it is deleted, if applicable
};