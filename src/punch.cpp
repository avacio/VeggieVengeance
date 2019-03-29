#include "punch.hpp"

Punch::Punch(int id, vec2 pos, vec2 size, unsigned int damage, bool direction, float vert_force) {
	//variable bullet attributes
	this->m_fighter_id = id;
	this->m_position = pos;
	this->m_damage = damage;

	//no sprite for punch, not relevant
	this->m_scale = vec2({ 0.0, 0.0 });
	this->m_velocity = vec2({ 0.0, 0.0 });
	
	this->m_width = size.x;
	this->m_height = size.y;
	this->m_delete_when = AFTER_UPDATE;
	this->m_damageEffect = new DamageEffect(this->m_position.x, this->m_position.y, this->m_width, this->m_height, this->m_damage, this->m_fighter_id, this->m_delete_when, vert_force);
}

Punch::~Punch() {
	delete m_damageEffect;
}

bool Punch::init() {
	return true;
}
void Punch::update() {
	return;
}

void Punch::draw(const mat3 &projection){
	return;
}