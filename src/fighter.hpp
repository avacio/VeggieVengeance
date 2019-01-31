#pragma once

#include "common.hpp"
#include <random>

// Salmon enemy 
class Fighter : public Renderable
{
	// Shared between all bubbles, no need to load one for each instance
	static Texture fighter_texture;

public:
	Fighter(std::string name, int maxHP, int speedStat, int strengthStat);

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

	// Returns the bubble' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	// decrements m_currentHP
	void take_damage(int dmg);

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians

	std::string m_name;
	int m_maxHP;
	int m_currentHP;
	int m_speedStat;	// int from range of 1 -> 5
	int m_strengthStat;	// int from range of 1 -> 5

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};