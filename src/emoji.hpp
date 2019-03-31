#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "attack.hpp"

class Emoji : public Attack {
	static Texture emoji_texture;
public:
	Emoji(int id, vec2 pos, unsigned int damage, bool direction);
	~Emoji();
	bool init();
	void update(float ms);
	void draw(const mat3 &projection) override;
};