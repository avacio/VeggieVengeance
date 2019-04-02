#pragma once

#include "common.hpp"
#include "textRenderer.hpp"
#include "textureRenderer.hpp"
#include "fighterInfo.hpp"

class Screen
{
public:
	void destroyButtons() {
		for (int i = 0; i < buttons.size(); i++) {
			TextRenderer *button = buttons[i];
			delete button;
		}
		buttons.clear();
	}
	void change_selection(bool goDown); // goDown is true when down direction
	void reset_selection();
protected:
	const vec3 selectedColor = { 0.7f,0.2f,0.2f };
	const vec3 defaultColor = { 0.4f,0.4f,0.4f };
	int selectedButtonIndex = 0;
	std::vector<TextRenderer*> buttons;
};

class MainMenu : public Renderable, public Screen
{
	// Background texture
	static Texture m_texture;

	//// Stage textures
	//static Texture m_stage_1_tx;
	//static Texture m_stage_2_tx;

	// GUI text renderer
	TextRenderer* title;

public:
	// Creates all the associated render resources and default transform
	//bool init(vec2 screen);
	bool init(vec2 screen);

	// Releases all the associated resources
	void destroy();

	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current bubble position
	vec2 get_position()const;

	// Sets the new bubble position
	void set_position(vec2 position);

	void reset();

	bool set_mode(GameMode mode);
	GameMode get_mode() { return m_mode; }
	GameMode get_selected_mode() { return m_selected_mode; }
	//////// MAIN MENU FUNCTIONS
	void init_menu_buttons();
	GameMode set_selected_mode();
	//////// CHAR SELECT FUNCTIONS
	FighterCharacter get_selected_char();
	void init_select_char_buttons();
	void draw_char_info(const mat3& projection);
	std::string int_to_stat_string(int in);
	//////// STAGE SELECT FUNCTIONS
	void init_select_stage_buttons();
	void init_stage_textures();
	Stage get_selected_stage();

	FighterCharacter selectedP1 = BLANK;
	bool is_player_1_chosen = false; // for picking PVP
	GameMode m_selected_mode = MENU;

	bool m_initialized = false;

private:
	vec2 screen;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians

	//// STAGE TEXTURES
	//vec2 m_stage_position; // Window coordinates
	//vec2 m_stage_scale; // 1.f in each dimension. 1.f is as big as the associated texture

	GameMode m_mode = MENU;

	std::vector<TextRenderer*> text;
	std::vector<TextureRenderer> stage_textures;
};