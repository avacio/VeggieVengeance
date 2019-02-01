#pragma once

// internal
#include "common.hpp"
#include "water.hpp"
#include "fighter.hpp"
#include "player.hpp"
#include "ai.hpp"

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
	bool init(vec2 screen);

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over()const;

private:
	// Generates a new fighter
	bool spawn_ai(aiType type);

	//INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

private:
	// Window handle
	GLFWwindow* m_window;

	// Screen texture
	// The draw loop first renders to this texture, then it is used for the water shader
	GLuint m_frame_buffer;
	Texture m_screen_tex;

	// Water effect
	Water m_water;

	// GUI text renderer
	


	// Game entities
	Player m_player1;
	Player m_player2;
	std::vector<AI> m_ais;

	float m_current_speed;
	
	Mix_Music* m_background_music;
	Mix_Chunk* m_salmon_dead_sound;
	Mix_Chunk* m_salmon_eat_sound;


	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};
