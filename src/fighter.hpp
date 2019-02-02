#pragma once

#include "common.hpp"
#include <random>

enum jumpState {JUMPING, FALLING, GROUNDED};
enum crouchState {NOT_CROUCHING, CROUCH_PRESSED, IS_CROUCHING, CROUCH_RELEASED};

// Salmon enemy 
class Fighter : public Renderable
{
	// Shared between all bubbles, no need to load one for each instance
	static Texture fighter_texture;

public:
	// Creates all the associated render resources and default transform
	bool init(int init_position);

	// Releases all the associated resources
	void destroy();

	// Update bubble due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the salmon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

	// Returns the current bubble position
	vec2 get_position()const;

	// Sets the new bubble position
	void set_position(vec2 position);

	void move(vec2 off);

	// Set fighter's movements
	void set_movement(int mov);

	// Returns the current health
	int get_health()const;

	int get_lives()const;

	// Returns the bubble' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

	jumpState get_jumpstate()const;

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	int m_health;
	int m_speed;	// each fighter has different speed and strength stats
	int m_strength;

	bool is_alive = true;
	bool is_idle = true;
	bool facing_front = true;
	bool moving_forward = false;
	bool moving_backward = false;
	bool is_punching = false;
	bool is_hurt = false;

	//lives system
	int m_lives; //counter for lives/stock remaining
	int respawn_timer = 0;
	bool respawn_flag = false;

	jumpState jumpstate = GROUNDED;
	int jumpcounter = 0;
	crouchState crouchstate = NOT_CROUCHING;
	//float angle;

	//CONST VALUES
	int MAX_JUMP = 20;
	int RESPAWN_TIME = 1000;  //in ms
	int STARTING_LIVES = 3;
	int MAX_HEALTH = 100;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};