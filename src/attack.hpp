#pragma once

#include "common.hpp"
#include "damageEffect.hpp"

class Attack : public Renderable {
public:
	virtual bool init() = 0;
	virtual void update(float ms) = 0;
	BoundingBox get_bounding_box() const;
	DamageEffect * get_damage_effect() const;
	virtual void draw(const mat3 &projection) override = 0;
	vec2 getPosition();

	int m_fighter_id;
	vec2 m_scale;
	vec2 m_position;
	vec2 m_velocity;
	DamageEffect * m_damageEffect;
	unsigned int m_damage;
	int m_width;
	int m_height;
	DeletionTime m_delete_when;

};