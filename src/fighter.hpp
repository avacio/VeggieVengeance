#pragma once

#include "common.hpp"
#include "fighterInfo.hpp"
#include "damageEffect.hpp"
#include "platform.hpp"
#include "textRenderer.hpp"
#include "attack.hpp"
#include "punch.hpp"
#include "uppercut.hpp"
#include "bullet.hpp"
#include "projectile.hpp"
#include "bomb.hpp"
#include "dash.hpp"
#include "emoji.hpp"
#include <set>
#include <random>
#include <iostream>
#include <SDL_mixer.h>

class Fighter : public Renderable
{
	// Shared between all bubbles, no need to load one for each instance
	static Texture f_texture;

  public:
	 Fighter(unsigned int id);
	// Creates all the associated render resources and default transform
	bool init(int init_position, std::string name, FighterCharacter fc);

	// Releases all the associated resources
	void destroy();

	// ms represents the number of milliseconds elapsed from the previous update() call
	Attack * update(float ms, std::vector<Platform> platforms);

	// projection is the 2D orthographic projection matrix
	void draw(const mat3 &projection) override;

	//get collision object for punch
	Punch * punch();

	//get collision object for powerpunch
	Punch * powerPunch();

	//get collision object for Broccoli's uppercut ability
	Uppercut * broccoliUppercut();

	//get collision object for dash
	Dash * dash();

	//get collision object for emoji
	Emoji * emoji();

	// Returns the current bubble position
	vec2 get_position() const;

	// Sets the new bubble position
	void set_position(vec2 position);

	void move(vec2 off);

	// Set fighter's movements
	void set_movement(int mov);

	void set_hurt(bool hurt);

	void apply_damage(DamageEffect * damage_effect);
	void apply_damage(int damage); // for stage effects

	void set_blocking(bool blocking);

	// Returns the current health
	int get_health() const;
	int get_block_tank() const;
	int get_lives() const;
	TextRenderer* get_nameplate() const;
	std::string get_name() const;

	int get_alive() const;

	bool get_facing_front() const;

	// Returns the bubble' bounding box for collision detection, called by collides_with()
	BoundingBox * get_bounding_box() const;

	// Returns the current fighter scale
	vec2 get_scale() const;

	// Returns the current fighter rotation
	float get_rotation() const;

	void start_jumping();

	unsigned int get_id() const;

	FighterCharacter get_fc() const;
	void set_sprite(SpriteType st) const;


	//void jump_update();
	void apply_friction();

	void x_position_update(float added_speed, float ms, std::vector<Platform> platforms);

	void y_position_update(float ms);
	
	void crouch_update();

	void die();

	void check_respawn(float ms);

	bool is_hurt() const;

	bool is_jumping() const;

	bool is_punching() const;

	bool is_power_punching() const;

	bool is_punching_on_cooldown() const;

	bool is_holding_power_punch() const;

	bool is_crouching() const;

	bool is_idle() const;

	bool is_blocking() const;

	bool is_tired_out() const;

	bool is_paused() const;

	bool is_uppercutting() const;

	int get_blocking_tank() const;

	int get_crouch_state();

	void set_crouch_state(CrouchState cs);

	void set_power_punch(bool punch);

	void set_uppercut(bool uc);

	void reset();

	void platform_collision(std::vector<Platform> platforms, vec2 oldPosition);

	void platform_pass_through(std::vector<Platform> platforms, vec2 oldPosition);

	float get_heal_animation();

	FighterCharacter m_fc;
	// Potato
	bool potato_is_holding_fries() const;
	void potato_charging_up_fries();
	void reset_potato_flags();

	// Broccoli
	bool broccoli_get_jump_left();
	void broccoli_set_double_jump();
	bool broccoli_is_uppercut_on_cooldown();
	bool broccoli_is_holding_cauliflowers() const;
	void broccoli_charging_up_cauliflowers();
	void reset_broccoli_flags();

	// Helpers 
	void charging_up_power_punch();
	
	

  protected:
  	int MAX_HEALTH;
	const int STARTING_LIVES = 3;

	int m_health;
	int m_lives; //counter for lives/stock remaining
	vec2 m_position;  // Window coordinates

	std::string m_name;
	TextRenderer* m_nameplate;

  private:
	vec2 m_scale;	 // 1.f in each dimension. 1.f is as big as the associated texture
	vec2 m_sprite_appearance_size; //the apparent width and height of the sprite, without scaling (used for more intuitive bounding boxes)
	float m_rotation; // in radians
	vec2 m_initial_pos;
	bool m_initial_dir;
	float m_initial_scale_x;
	
	float m_speed; // each fighter has different speed and strength stats
	int m_strength;

	bool m_is_alive = true;
	bool m_is_idle = true;
	bool m_tired_out = false;
	bool m_facing_front = true;
	bool m_moving_forward = false;
	bool m_moving_backward = false;
	bool m_is_holding_power_punch = false;
	bool m_is_power_punching = false;
	bool m_power_punch_sprite = false;
	bool m_is_punching = false;
	bool m_punch_on_cooldown = false;
	bool m_is_hurt = false;
	bool m_is_jumping = false;
	float punching_cooldown = 0;
	float m_tired_out_timer = 0;
	float m_holding_too_much_timer = 0;
	float m_holding_power_punch_timer = 0;
	int m_blocking_tank;
	bool m_is_blocking = false;
	int m_respawn_timer = 0;
	bool m_respawn_flag = false;
	int m_anim_counter = 0;
	int m_punch_counter = 0;
	bool m_is_paused = false;

	// POTATO states
	// ABILITY 1: Tater Tot (Bomb)
	bool m_potato_is_planting_bomb = false;
	bool m_potato_bomb_planted = false;
	bool m_potato_explode_planted_bomb = false;
	bool m_potato_bomb_ticking = false;
	bool m_potato_bomb_on_cooldown = false;
	float m_potato_bomb_selftimer = 0;
	float m_potato_bomb_cooldown = 0;
	Bomb * bomb_pointer = NULL;
	// ABILITY 2: Fries (Bullet)
	bool m_potato_is_shooting_fries = false;
	bool m_potato_is_holding_fries = false;
	bool m_potato_is_shooting_charged_fries = false;
	bool m_potato_fries_on_cooldown = false;
	float m_potato_holding_fries_timer = 0;
	float m_potato_fries_cooldown = 0;
	
	

	// broccoli states
	// PASSIVE: Double Jump
	bool m_broccoli_is_double_jumping = false;
	int m_broccoli_jump_left = 2;
	// ABILITY 1: Uppercut
	bool m_broccoli_is_uppercutting = false;
	bool m_broccoli_uppercut_on_cooldown = false;
	float m_broccoli_uppercut_cooldown = 0;
	// ABILITY 2: Cauliflower (Projectile)
	bool m_broccoli_is_shooting_cauliflowers = false;
	bool m_broccoli_is_holding_cauliflowers = false;
	bool m_broccoli_is_shooting_charged_cauliflowers = false;
	bool m_broccoli_cauliflowers_on_cooldown = false;
	float m_broccoli_holding_cauliflowers_timer = 0;
	float m_broccoli_cauliflowers_cooldown = 0;

	// yam states
	bool m_yam_is_healing = false;
	float m_yam_heal_cooldown_ms = 0;
	float m_yam_heal_animation_ms = 0.0;
	const float MAX_HEAL_COOLDOWN = 1000.0;
	const float MAX_HEAL_ANIMATION = 200.0;
	const int RECOVERY_POINTS = 5;
	bool m_yam_start_dashing = false;
	float m_yam_dash_cooldown_ms = 0.0;
	float m_yam_dash_timer_ms = 0.0;
	const float MAX_DASH_TIMER = 500.0;
	const float MAX_DASH_COOLDOWN = 1000.0;

	//eggplant states
	bool m_eggplant_spawn_emoji = false;
	bool m_eggplant_shoot_emoji = false;
	unsigned int m_eggplant_emoji_count = 0;
	float m_eggplant_spawn_cooldown = 0.0;
	float m_eggplant_shoot_cooldown = 0.0;
	const float MAX_EMOJI_SPAWN_COOLDOWN = 500.0;
	const unsigned int MAX_EMOJI_COUNT = 3;
	std::vector<Emoji*> m_eggplant_emojis;

	CrouchState m_crouch_state = NOT_CROUCHING;

	vec2 m_force;	// in Newtons
	float m_mass;	// mass in kg
	float m_friction;
	float m_velocity_y;

	//CONST VALUES
	const int RESPAWN_TIME = 1000; //in ms
	
	const float INITIAL_JUMP_VELOCITY = 400.0;
	const float TERMINAL_VELOCITY_Y = 400.0;
	const vec2 GRAVITY = {0.0, 800.0};

	const unsigned int m_id; //unique identifier given when created

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};