#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "attack.hpp"

class Punch : public Attack {
	static Texture bullet_texture;
public:
	Punch(int id, vec2 pos, vec2 size, unsigned int damage, bool direction);
	~Punch();
	bool init();
	void update();
	void draw(const mat3 &projection) override;
private:
};