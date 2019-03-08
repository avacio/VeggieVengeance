#pragma once

#include "common.hpp"
#include "damageEffect.hpp"
#include "platform.hpp"
#include "textRenderer.hpp"
#include <random>

class Fighter : public Renderable
{
	// Shared between all bubbles, no need to load one for each instance
	static Texture fighter_texture;

  public:
	 Fighter(unsigned int id);
	// Creates all the associated render resources and default transform
	bool init(int init_position, std::string name);

	// Releases all the associated resources
	void destroy();

	// Update bubble due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	DamageEffect * update(float ms, std::vector<Platform> platforms);

	// projection is the 2D orthographic projection matrix
	void draw(const mat3 &projection) override;

	//get collision object for punch
	DamageEffect * punch();

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

	void jump_update(float ms);

	bool is_hurt() const;

	bool is_jumping() const;

	bool is_punching() const;

	bool is_crouching() const;

	bool is_idle() const;

	int get_crouch_state();

	void set_crouch_state(CrouchState cs);

	void reset(int init_position);

	void platform_collision(std::vector<Platform> platforms, vec2 oldPosition);

	void updatePosition(float ms);

  protected:
  	const int MAX_HEALTH = 100;
	const int STARTING_LIVES = 3;

	int m_health;
	int m_lives; //counter for lives/stock remaining
	vec2 m_position;  // Window coordinates

	std::string m_name;
	TextRenderer* m_nameplate;

  private:
	vec2 m_scale;	 // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	
	int m_speed; // each fighter has different speed and strength stats
	int m_strength;
	float m_vertical_velocity;
	vec2 m_velocity;

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
	const float MOVEMENT_SPEED = 5.0;
	const float ACCELERATION = -INITIAL_VELOCITY / 20.0;
	const vec2 GRAVITY = {0.0, 0.0};

	const unsigned int m_id; //unique identifier given when created

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};