#pragma once

#include "common.hpp"
#include "boundingBox.hpp"
class Platform: public Renderable
{
public:
	static Texture fighter_texture;
	Platform(float xpos, float ypos, float width, float height);
	bool check_collision(BoundingBox b);
	bool check_collision_outer_y(BoundingBox b);
	bool check_collision_outer_x(BoundingBox b);
	bool init();
	void destroy();
	void draw(const mat3 &projection);
private:
	BoundingBox m_bounding_box;
	
	vec2 m_position; // Window coordinates
	vec2 m_size; //width x height
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture

	const float OUTER_DEPTH = 0.001;
};