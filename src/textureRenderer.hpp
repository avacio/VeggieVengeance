#pragma once

#include "common.hpp"
#include "fighterInfo.hpp"
#include <string.h>

class TextureRenderer : public Renderable
{
public:
	// Creates all the associated render resources and default transform
	bool init(vec2 screen, Texture * texture, vec2 scale, vec2 position);

	// Releases all the associated resources
	void destroy();

	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current bubble position
	vec2 get_position()const;

	// Sets the new bubble position
	void set_position(vec2 position);
	Texture * m_texture;

private:
	vec2 screen;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
};