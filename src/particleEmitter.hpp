#pragma once
#include "common.hpp"
#include "particle.hpp"

class ParticleEmitter : public Renderable {
public:
	ParticleEmitter(vec2 position, int max_particles, vec3 m_color, bool random, float angle);
	~ParticleEmitter();

	bool init();
	void update(float ms);
	void draw(const mat3 &projection);
	int get_alive_particles();

private:
	Particle * particle_pool;
	vec2 m_position;
	int m_max_particles;
	int m_num_alive_particles;
	vec3 m_color;
	bool m_is_random;
	float m_set_angle;
};