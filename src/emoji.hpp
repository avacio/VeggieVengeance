#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "attack.hpp"
#include "fighterInfo.hpp"

enum EmojiState {
	CIRCLING = 0,
	FIRED = 1
};

class Emoji : public Attack {
public:
	Emoji(int id, vec2 pos, unsigned int damage, bool direction);
	~Emoji();
	bool init();
	void update(float ms);
	void draw(const mat3 &projection) override;
	void fire_emoji(bool direction);
	void set_fighter_pos(vec2 pos);
	bool is_circling();
private:
	void set_texture();

	Texture * emoji_texture;
	EmojiState state = CIRCLING;
	vec2 fighter_position;
	float angle = 0;
	const float angle_increment = PI / 32;
	const float radius = 40.0;
};