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

unsigned int Attack::get_pointer_references() {
	return m_pointer_references;
}
void Attack::deincrement_pointer_references() {
	m_pointer_references--;
}
void Attack::increment_pointer_references() {
	m_pointer_references++;
}