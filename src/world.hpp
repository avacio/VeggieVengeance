#pragma once

// internal
#include "common.hpp"
#include "water.hpp"
#include "fighter.hpp"
#include "player1.hpp"
#include "player2.hpp"
#include "ai.hpp"
#include "background.hpp"
#include "damageEffect.hpp"
#include "boundingBox.hpp"
#include "textRenderer.hpp"
#include "mainMenu.hpp"


// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class World
{
  public:
	World();
	~World();

	// Creates a window, sets up events and begins the game
	bool init(vec2 screen, GameMode mode);

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over() const;

	bool set_mode(GameMode mode);
	void set_paused(bool isPaused);

  private:
	// Generates a new fighter
	bool spawn_ai(AIType type);

	void reset();

	//INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow *, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow *window, double xpos, double ypos);

	bool check_collision(BoundingBox b1, BoundingBox b2);


  private:
	// Window handle
	GLFWwindow *m_window;

	vec2 m_screen; // screen vector

	// Screen texture
	// The draw loop first renders to this texture, then it is used for the water shader
	GLuint m_frame_buffer;
	Texture m_screen_tex;

	// Water effect
	Water m_water;

	// Game entities
	MainMenu m_menu;
	Background m_bg;
	Player1 m_player1;
	Player2 m_player2;
	std::vector<AI> m_ais;
	std::vector<DamageEffect> m_damageEffects;
	//float m_current_speed;
	std::vector<Fighter> m_fighters; // all fighters including AIs

	Mix_Music *m_background_music;

	GameMode m_mode;
	bool m_paused;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1

	//determines next ID for fighter, incremented every time a fighter is added
	//id 1 and 2 are reserved for players 1 and 2
	unsigned int idCounter = 3;
};
