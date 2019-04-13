#include "particle.hpp"
#include <math.h>

Particle::Particle() {
	m_position = {0.0,0.0};
	m_lifespan = 0;
	m_velocity = { 0.0,0.0 };
	m_is_alive = true;
	m_color = { 0.0,0.0,0.0 };
}

void Particle::init(vec2 position, float lifespan, float angle, float speed, vec3 color, float scale) {
	m_position = position;
	m_lifespan = lifespan;
	float radians = angle * PI / 180.f;
	m_velocity.x = speed * cos(radians);
	m_velocity.y = -speed * sin(radians);
	m_scale = scale; //5.F
	m_color = color;
}

static const float p = 1.293f;
static const float A = 0.1f;
static const float Cd = 0.47f;
static const float m = 1.f;

void Particle::update(float ms) {
	m_lifespan -= ms;
	if (m_lifespan < 0) {
		m_is_alive = false;
		return;
	}

	float fd_x = -m_velocity.x * 0.5f * p * Cd * A * pow(m_velocity.x, 2);
	float fd_y = -m_velocity.y * 0.5f * p * Cd * A * pow(m_velocity.y, 2);
	float a_x = fd_x / m;
	float a_y = fd_y / m;

	m_velocity.x += a_x * (ms / 1000);
	m_velocity.y += a_y * (ms / 1000);
	m_position.x += m_velocity.x;
	m_position.y += m_velocity.y;
}

void Particle::copy_from(Particle& particle) {
	m_position = particle.m_position;
	m_velocity = particle.m_velocity;
	m_is_alive = particle.m_is_alive;
	m_lifespan = particle.m_lifespan;
}

vec2 Particle::get_position() {
	return m_position;
}
bool Particle::get_is_alive() {
	return m_is_alive;
}

void Particle::draw(const mat3& projection) {
	return; // TODO
}

void Particle::draw(const mat3 &projection, Effect& emitter_effect) {
	transform_begin();
	transform_translate(m_position);
	transform_scale({ m_scale, m_scale });
	transform_end();

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(emitter_effect.program, "in_position");
	GLint in_color_loc = glGetAttribLocation(emitter_effect.program, "in_color");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

	// Getting uniform locations
	GLint transform_uloc = glGetUniformLocation(emitter_effect.program, "transform");
	GLint projection_uloc = glGetUniformLocation(emitter_effect.program, "projection");

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	/// TODO TRYING TO COLOR
	GLint color_uloc = glGetUniformLocation(emitter_effect.program, "fcolor");
	float color[] = { m_color.x, m_color.y, m_color.z };
	glUniform3fv(color_uloc, 1, color);

	// Drawing!
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
}