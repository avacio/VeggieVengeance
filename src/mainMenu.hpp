#pragma once

#include "common.hpp"
#include "textRenderer.hpp"

class MainMenu : public Renderable
{
	// Background texture
	static Texture m_texture;

	// GUI text renderer
	TextRenderer* title;

public:
	// Creates all the associated render resources and default transform
	bool init(vec2 screen);

	// Releases all the associated resources
	void destroy();

	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current bubble position
	vec2 get_position()const;

	// Sets the new bubble position
	void set_position(vec2 position);

	void init_buttons();
	void change_selection(bool goDown); // goDown is true when down direction
	//void change_selection();
	GameMode get_selected();

private:
	vec2 screen;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians

	int selectedButtonIndex = -1;
	std::vector<TextRenderer*> buttons;
	const vec3 selectedColor = { 0.7f,0.2f,0.2f };
	const vec3 defaultColor = { 0.4f,0.4f,0.4f };
};