#include "damageEffect.hpp"

DamageEffect::DamageEffect(int xpos, int ypos, int width, int height, unsigned int damage, int id) : bounding_box(xpos, ypos, width, height) {
	this->damage = damage;
	this->id = id;
}