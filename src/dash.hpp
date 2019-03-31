#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "attack.hpp"

class Dash : public Attack {
public:
	Dash(int id, vec2 pos, vec2 size, unsigned int damage, float vert_force);
	~Dash();
	bool init();
	void update(float ms);
	void draw(const mat3 &projection) override;
private:
};