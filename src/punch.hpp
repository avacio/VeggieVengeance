#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "attack.hpp"

class Punch : public Attack {
public:
	Punch(int id, vec2 pos, vec2 size, unsigned int damage, bool direction, float vert_force);
	~Punch();
	bool init();
	void update(float ms);
	void draw(const mat3 &projection) override;
private:
};