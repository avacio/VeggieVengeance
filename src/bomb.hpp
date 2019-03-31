#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "attack.hpp"
#include <SDL_mixer.h>


class Bomb : public Attack {
	static Texture bomb_texture;
public:
	Bomb(int id, vec2 pos, unsigned int damage, float vert_force, float ms);
	~Bomb();
	bool init();
	void update(float ms);
	void draw(const mat3 &projection) override;
private:
	float m_timer;
};