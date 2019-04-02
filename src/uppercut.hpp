#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "attack.hpp"

class Uppercut : public Attack {
public:
	Uppercut(int id, vec2 pos, vec2 size, unsigned int damage, bool direction, float vert_force, float ms);
	~Uppercut();
	bool init();
	void update(float ms);
	void draw(const mat3 &projection) override;
private:
};