#include "damageEffect.hpp"

DamageEffect::DamageEffect(int xpos, int ypos, int width, int height, unsigned int damage, int id, DeletionTime delete_when) : bounding_box(xpos, ypos, width, height) {
	this->damage = damage;
	this->id = id;
	this->delete_when = delete_when;
	hit_fighter = false;
}