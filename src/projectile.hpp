#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "attack.hpp"

class Projectile : public Attack {
	static Texture projectile_texture;
public:
	Projectile(int id, vec2 pos, unsigned int damage, bool direction);
	~Projectile();
	bool init();
	void update();
	void draw(const mat3 &projection) override;
private:
	float m_acceleration;
};