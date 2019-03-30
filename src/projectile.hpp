#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "attack.hpp"

class Projectile : public Attack {
	static Texture projectile_texture;
public:
	Projectile(int id, vec2 pos, float velo, unsigned int damage, bool direction);
	~Projectile();
	bool init();
	void update(float ms);
	void draw(const mat3 &projection) override;
private:
	float m_acceleration;
	float m_bounce_loss;
};