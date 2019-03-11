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
	TextRenderer* block1;
	TextRenderer* block2;
	TextRenderer* lives1;
	TextRenderer* lives2;
	TextRenderer* isPausedText;
	TextRenderer* jump;
	TextRenderer* left;
	TextRenderer* right;
	TextRenderer* crouch;
	TextRenderer* pause;
	TextRenderer* reset;
	TextRenderer* ability1;
	TextRenderer* ability2;
	TextRenderer* punch;
	TextRenderer* pauseMusic;
	TextRenderer* resumeMusic;
	TextRenderer* increaseVolume;
	TextRenderer* decreaseVolume;

public:
	// Creates all the associated render resources and default transform
	bool init(vec2 screen, GameMode mode);

	// Releases all the associated resources
	void destroy();

	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current bubble position
	vec2 get_position()const;

	// Sets the new bubble position
	void set_position(vec2 position);

	void setPaused(bool isPaused);
	void setPlayerInfo(int p1Lives, int p1HP, int p1BL, int p2Lives, int p2HP, int p2BL); // set to -1 if player is not initialized
	void drawPlayerInfo(const mat3& projection);
	void drawTutorialText(const mat3& projection);

	void addNameplate(TextRenderer* td, std::string name);
	void drawNameplates(const mat3& projection);
	void clearNameplates();
	void init_buttons();
	void change_selection(bool goDown);
	PauseMenuOption get_selected();

	bool m_initialized = false;

private:
	vec2 screen;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	GameMode m_mode;
	bool m_paused = false;

	int p1Lives = -1;
	int p1HP = -1;
	int p1BL = -1;
	int p2Lives = -1;
	int p2HP = -1;
	int p2BL = -1;

	int selectedButtonIndex = -1;
	std::vector<TextRenderer*> buttons;
	const vec3 selectedColor = { 0.7f,0.2f,0.2f };
	const vec3 defaultColor = { 0.4f,0.4f,0.4f };

	std::map<TextRenderer*, std::string> nameplates;
};