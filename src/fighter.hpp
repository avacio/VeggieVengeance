#pragma once

#include "common.hpp"
#include <random>

// Salmon enemy
class Fighter : public Renderable
{
	// Shared between all bubbles, no need to load one for each instance
	static Texture fighter_texture;

  public:
	 Fighter(unsigned int id);
	// Creates all the associated render resources and default transform
	bool init(int init_position);

	// Releases all the associated resources
	void destroy();

	// Update bubble due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the salmon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3 &projection) override;

	// Returns the current bubble position
	vec2 get_position() const;

	// Sets the new bubble position
	void set_position(vec2 position);

	void move(vec2 off);

	// Set fighter's movements
	void set_movement(int mov);

	void set_hurt(bool hurt);

	void decrease_health(unsigned int damage);

	// Returns the current health
	int get_health() const;

	int get_lives() const;

	// Returns the bubble' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box() const;

	JumpState get_jumpstate() const;

	unsigned int get_id() const;

  private:
	vec2 m_position;  // Window coordinates
	vec2 m_scale;	 // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	int m_health;
	int m_speed; // each fighter has different speed and strength stats
	int m_strength;

	bool m_is_alive = true;
	bool m_is_idle = true;
	bool m_facing_front = true;
	bool m_moving_forward = false;
	bool m_moving_backward = false;
	bool m_is_punching = false;
	bool m_is_hurt = false;

	//lives system
	int m_lives; //counter for lives/stock remaining
	int m_respawn_timer = 0;
	bool m_respawn_flag = false;

	JumpState m_jump_state = GROUNDED;
	int m_jump_counter = 0;
	CrouchState m_crouch_state = NOT_CROUCHING;
	//float angle;

	//CONST VALUES
	const int MAX_JUMP = 20;
	const int RESPAWN_TIME = 1000; //in ms
	const int STARTING_LIVES = 3;
	const int MAX_HEALTH = 100;

	const unsigned int m_id; //unique identifier given when created

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};