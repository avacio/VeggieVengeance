#pragma once

#include "common.hpp"
#include <random>

// Salmon enemy 
class Fighter : public Renderable
{
	// Shared between all bubbles, no need to load one for each instance
	static Texture fighter_texture;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();

	// Update bubble due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the salmon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current bubble position
	vec2 get_position()const;

	// Sets the new bubble position
	void set_position(vec2 position);

	void move(vec2 off);

	// Set fighter's movements
	void set_movement(int mov);

	// Returns the current health
	int get_health()const;

	// Returns the bubble' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	int m_health;
	bool is_alive;
	bool is_idle;
	bool facing_front;
	bool moving_forward;
	bool moving_backward;
	bool is_jumping;
	bool is_crouching;
	bool is_punching;
	//float angle;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};