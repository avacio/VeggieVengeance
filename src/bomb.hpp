#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "attack.hpp"


class Bomb : public Attack {
	static Texture bomb_texture;
public:
	Bomb(int id, vec2 pos, unsigned int damage, float vert_force, float ms);
	~Bomb();
	bool init();
	void update(float ms);
	void draw(const mat3 &projection) override;
private:
	float m_velocity_y = 0;
	const vec2 gravity = { 0.0, 800.0 };
	const float TERMINAL_VELOCITY_Y = 400.0;
};