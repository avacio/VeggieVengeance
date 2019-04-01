#pragma once
#pragma once

#include "common.hpp"
#include "attack.hpp"
#include "fighter.hpp"
#include "boundingBox.hpp"

//class Knife : public Renderable
class Knife : public Attack
{
public:
	void spawn_knife(unsigned int damage, vec2 pos);

	// Creates all the associated render resources and default transform
	//bool init(unsigned int damage, vec2 pos);
	bool init();

	// Releases all associated resources
	void destroy();

	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	void draw(const mat3& projection)override;

	vec2 get_position()const;

	// Set salmon rotation in radians
	void set_rotation(float radians);

	bool collides_with(const Fighter& f);

	BoundingBox * boundingBox;
	bool m_is_on_ground = false;
	bool m_done = false;

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements
};
