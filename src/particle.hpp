#pragma once
#include "common.hpp"

class Particle : public Renderable {
	static Texture bomb_texture;
public:
	Particle();
	void init(vec2 position, float lifespan, float angle, float speed, vec3 color, float scale);
	void update(float ms);
	vec2 get_position();
	bool get_is_alive();

	void copy_from(Particle & particle);

	void draw(const mat3 &projection);
	void draw(const mat3 &projection, Effect& emitter_effect);
private:
	vec2 m_position;
	bool m_is_alive;
	vec2 m_velocity;
	float m_lifespan;
	float m_scale;
	vec3 m_color;
};