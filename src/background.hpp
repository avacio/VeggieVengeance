#pragma once

#include "common.hpp"
#include "textRenderer.hpp"

// Background
class Background : public Renderable
{
	// Background texture
	static Texture bg_texture;

	// GUI text renderer
	TextRenderer* health1;
	TextRenderer* health2;
	TextRenderer* lives1;
	TextRenderer* lives2;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all the associated resources
	void destroy();

	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current bubble position
	vec2 get_position()const;

	// Sets the new bubble position
	void set_position(vec2 position);

	void setPlayerInfo(int p1Lives, int p1HP, int p2Lives, int p2HP); // set to -1 if player is not initialized
	void drawPlayerInfo(const mat3& projection);


private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians

	int p1Lives = -1;
	int p1HP = -1;
	int p2Lives = -1;
	int p2HP = -1;
};