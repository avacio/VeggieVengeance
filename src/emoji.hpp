#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "attack.hpp"
#include "fighterInfo.hpp"

class Emoji : public Attack {
public:
	Emoji(int id, vec2 pos, unsigned int damage, bool direction);
	~Emoji();
	bool init();
	void update(float ms);
	void draw(const mat3 &projection) override;
private:
	Texture * emoji_texture;
	void set_texture();
};