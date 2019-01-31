// Header
#include "fighter.hpp"

// stlib
#include <vector>
#include <string>
#include <algorithm>

bool Fighter::init()
{
	// setting initial values
	m_is_alive = true;
	is_idle = true;

	return true;
}

// Releases all graphics resources
void Fighter::destroy()
{

}

// Called on each frame by World::update()
void Fighter::update(float ms)
{
	float MOVING_SPEED = 3.0;
	if (m_is_alive) {
		if (moving_forward)
			move({ MOVING_SPEED, 0.0 });
		if (moving_backward)
			move({ -MOVING_SPEED, 0.0 });
		if (is_jumping)
			jump();
		if (is_crouching)
			crouch();
	}
}

void Fighter::draw(const mat3& projection)
{

}

vec2 Fighter::get_position()const
{
	return m_position;
}

void Fighter::change_velocity(vec2 off) {
	m_velocity.x += off.x; m_velocity.y += off.y;
}

void Fighter::move(vec2 off)
{
	m_position.x += off.x; m_position.y += off.y;
}

void Fighter::set_rotation(float radians)
{
	m_rotation = radians;
}

bool Fighter::is_alive()const
{
	return m_is_alive;
}

// Called when the salmon collides with a turtle
void Fighter::kill()
{
	m_is_alive = false;
}

// set fighter's movements
void Fighter::set_movement(int mov) {
	switch (mov) {
	case 0:
		moving_forward = true;
		is_idle = false;
		break;
	case 1:
		moving_backward = true;
		is_idle = false;
		break;
	case 2:
		is_jumping = true;
		is_idle = false;
		break;
	case 3:
		is_crouching = true;
		is_idle = false;
		break;
	case 4:
		moving_forward = false;
		is_idle = true;
		break;
	case 5:
		moving_backward = false;
		is_idle = true;
		break;
	case 6:
		is_crouching = false;
		is_idle = true;
		break;
	}
}

void Fighter::jump() {
	m_position.y += M_PI / 18;
	if ((roundf(abs(sin(m_position.y)) * 10) / 10) != 0.0)
		m_position.y += M_PI / 18;
	else
		m_position.y = 0.0;
}

void Fighter::crouch() {
	m_position.y - M_PI / 18;
	if ((roundf(abs(sin(m_position.y)) * 10) / 10) != 0.0)
		m_position.y -= M_PI / 18;
	else
		m_position.y = 0.0;
}