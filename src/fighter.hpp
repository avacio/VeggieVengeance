#pragma once

#include "common.hpp"
#include <random>

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

	// projection is the 2D orthographic projection matrix
	void draw(const mat3 &projection) override;

	// Returns the current bubble position
	vec2 get_position() const;

	// Sets the new bubble position
	void set_position(vec2 position);

	void move(vec2 off);

	// Set fighter's movements
	void set_movement(int mov);

	// Returns the current health
	int get_health() const;

	int get_lives() const;

	// Returns the bubble' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box() const;

	void start_jumping();

	void jump_update();

	bool is_jumping() const;

	void reset(int init_position);

  protected:
  	const int MAX_HEALTH = 100;
	const int STARTING_LIVES = 3;

	int m_health;
	int m_lives; //counter for lives/stock remaining
	vec2 m_position;  // Window coordinates

  private:
	vec2 m_scale;	 // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	
	int m_speed; // each fighter has different speed and strength stats
	int m_strength;
	float m_vertical_velocity;

	bool m_is_alive = true;
	bool m_is_idle = true;
	bool m_facing_front = true;
	bool m_moving_forward = false;
	bool m_moving_backward = false;
	bool m_is_punching = false;
	bool m_is_hurt = false;
	bool m_is_jumping = false;
	
	int m_respawn_timer = 0;
	bool m_respawn_flag = false;

	CrouchState m_crouch_state = NOT_CROUCHING;

	//CONST VALUES
	const int MAX_JUMP = 20;
	const int RESPAWN_TIME = 1000; //in ms
	
	const float INITIAL_VELOCITY = 10.0;
	const float ACCELERATION = -INITIAL_VELOCITY / 20.0;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};