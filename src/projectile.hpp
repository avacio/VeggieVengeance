#pragma once

#include "common.hpp"
#include "damageEffect.hpp"

class Projectile : public Renderable {
	static Texture projectile_texture;
public:
	Projectile(vec2 pos, bool dir, float velo, float dmg, int id);
	bool init();
	void moveProjectile();
	void accelerate(float acc);
	DamageEffect * projectileDmg();
	vec2 get_bounding_box() const;
	void draw(const mat3 &projection) override;
	vec2 getPosition();
private:
	vec2 position;
	vec2 velocity;
	vec2 scale;
	float damage;
	int m_id;
};