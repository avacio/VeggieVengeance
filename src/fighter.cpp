// Header
#include "fighter.hpp"

// stlib
#include <vector>
#include <string>
#include <algorithm>

bool Fighter::init()
{
	return true;
}

// Releases all graphics resources
void Fighter::destroy()
{

}

// Called on each frame by World::update()
void Fighter::update(float ms)
{

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