#include "damageEffect.hpp"

DamageEffect::DamageEffect(int xpos, int ypos, int width, int height, unsigned int damage, int id, DeletionTime delete_when) : m_bounding_box(xpos, ypos, width, height) {
	this->m_damage = damage;
	this->m_fighter_id = id;
	this->m_delete_when = delete_when;
	m_hit_fighter = false;
}