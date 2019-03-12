#include "attack.hpp"

BoundingBox Attack::get_bounding_box() const {
	return this->m_damageEffect->m_bounding_box;
}

DamageEffect * Attack::get_damage_effect() const {
	return this->m_damageEffect;
}

vec2 Attack::getPosition() {
	return this->m_position;
}