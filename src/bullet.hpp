#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "attack.hpp"

class Bullet : public Attack {
	static Texture bullet_texture;
public:
	Bullet(int id, vec2 pos, unsigned int damage, bool direction);
	~Bullet();
	bool init();
	void update();
	void draw(const mat3 &projection) override;
private:
	
};