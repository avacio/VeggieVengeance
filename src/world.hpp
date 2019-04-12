#pragma once

// internal
#include "common.hpp"
#include "fighterInfo.hpp"
#include "water.hpp"
#include "fighter.hpp"
#include "player1.hpp"
#include "player2.hpp"
#include "ai.hpp"
#include "background.hpp"
#include "damageEffect.hpp"
#include "boundingBox.hpp"
#include "textRenderer.hpp"
#include "textureRenderer.hpp"
#include "mainMenu.hpp"
#include "platform.hpp"
#include "knife.hpp"
#include "particleEmitter.hpp"

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
	bool is_game_over(); // only for fight levels

	bool set_mode(GameMode mode);
	void set_paused(bool isPaused);

	void init_stage(Stage stage);

	void play_grunt_audio();
	void draw_rectangle();

	void set_heat_wave(bool on);
	float get_stage_fx_time();
	void apply_stage_fx_dmg();
	void set_falling_knives(bool on);

  private:
	// Generates a new fighter
	bool spawn_ai(AIType type);
	bool spawn_ai(AIType type, FighterCharacter fc);
	bool spawn_platform(float xpos, float ypos, float width, float height);

	void reset();

	//INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow *, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow *window, double xpos, double ypos);
	void attack_collision();
	void attack_deletion();
	void attack_update(float ms);

	bool check_collision(BoundingBox b1, BoundingBox b2);
	bool check_collision_world(BoundingBox b1);

	void clear_all_fighters();
	void init_char_select_ais();
	void emit_particles(vec2 position, vec3 color, int maxParticles);

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
	std::vector<Attack*> m_attacks;
	std::vector<Platform> m_platforms;
	//float m_current_speed;
	std::vector<Fighter> m_fighters; // all fighters including AIs
	std::vector<AI> m_char_select_ais;

	std::vector<Knife> m_knives;

	std::vector<ParticleEmitter*> m_particle_emitters;

	unsigned int m_background_track;
	std::vector<Mix_Music*> m_bgms;
	std::vector<Mix_Chunk*> m_grunt_audio;
	Mix_Chunk *m_broccoli_uppercut_audio;
	Mix_Chunk *m_charging_up_audio;
	Mix_Chunk *m_charged_punch_audio;

	GameMode m_mode;
	bool m_paused;
	bool m_over;
	bool m_game_over = false;
	std::string m_winner_name;
	bool m_heat_wave_on = false;
	float m_stage_fx_time = -1;
	bool m_falling_knives_on = false;

	/////////
	FighterCharacter selectedP1 = POTATO; // POTATO
	FighterCharacter selectedP2 = POTATO; // POTATO
	Stage selected_stage = KITCHEN;
	GameMode selected_fight_mode = PVP;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1

	//determines next ID for fighter, incremented every time a fighter is added
	//id 1 and 2 are reserved for players 1 and 2
	unsigned int idCounter = 3;
};
