#pragma once

#include "common.hpp"
#include "damageEffect.hpp"

class Bullet : public Renderable {
	static Texture bullet_texture;
public:
	Bullet(vec2 pos, bool dir, float dmg, int id);
	~Bullet();
	bool init();
	void moveBullet();
	DamageEffect * bulletDmg();
	vec2 get_bounding_box() const;
	void draw(const mat3 &projection) override;
	vec2 getPosition();
private:
	vec2 scale;
	vec2 position;
	int m_id;
	float dx;
	float damage;
};