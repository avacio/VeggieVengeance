#pragma once

#include "common.hpp"

class Projectile : public Renderable {
	static Texture projectile_texture;
public:
	bool init();
	Projectile(vec2 pos, bool dir);
	void moveProjectile();
	void draw(const mat3 &projection) override;
	vec2 getPosition();

private:
	vec2 position;
	float dx;
};