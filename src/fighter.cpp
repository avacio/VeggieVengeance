// Header
#include "fighter.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

Texture Fighter::fighter_texture;

bool Fighter::init(int init_position)
{
	// Load shared texture
	if (!fighter_texture.is_valid())
	{
		if (!fighter_texture.load_from_file(textures_path("potato.png")))
		{
			fprintf(stderr, "Failed to load bubble texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture
	float wr = fighter_texture.width * 3.5f;
	float hr = fighter_texture.height * 3.5f;

	TexturedVertex vertices[4];
	vertices[0].position = {-wr, +hr, -0.02f};
	vertices[0].texcoord = {0.f, 1.f};
	vertices[1].position = {+wr, +hr, -0.02f};
	vertices[1].texcoord = {1.f, 1.f};
	vertices[2].position = {+wr, -hr, -0.02f};
	vertices[2].texcoord = {1.f, 0.f};
	vertices[3].position = {-wr, -hr, -0.02f};
	vertices[3].texcoord = {0.f, 0.f};

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = {0, 3, 1, 1, 3, 2};

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;
	if (!effect.load_from_file(shader_path("fighter.vs.glsl"), shader_path("fighter.fs.glsl")))
		return false;

	m_rng = std::default_random_engine(std::random_device()());
	float rng = m_dist(m_rng);

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture

	m_is_alive = true;
	m_is_idle = true;
	m_facing_front = true;
	m_is_hurt = false;

	m_scale.x = 0.2f;
	m_scale.y = 0.2f;
	m_rotation = 0.f;
	m_health = MAX_HEALTH;
	m_speed = 5;
	m_strength = 5;
	m_lives = STARTING_LIVES;
	m_vertical_velocity = 0.0;
	if (init_position == 1)
	{
		m_position = {250.f, 525.f};
	}
	else if (init_position == 2)
	{
		m_position = {950.f, 525.f};
	}
	else
	{
		m_position = {550.f, 525.f};
	}

	return true;
}

// Call if init() was successful
// Releases all graphics resources
void Fighter::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Fighter::update(float ms)
{
	const float MOVING_SPEED = 5.0;

	//IF JUST DIED
	if (m_health <= 0 && m_is_alive)
	{
		m_is_alive = false;
		m_lives--;

		if (m_lives > 0)
		{
			m_respawn_timer = RESPAWN_TIME;
			m_respawn_flag = true;
		}
	}

	//If respawn pending
	if (m_respawn_flag)
	{
		if (m_respawn_timer > 0)
		{
			//count down by time passed
			m_respawn_timer -= ms;
		}
		else
		{
			//reset flags and revive
			m_respawn_flag = false;
			m_respawn_timer = 0;
			m_is_alive = true;
			m_is_hurt = false;
			m_health = MAX_HEALTH;
			//unrotate the potate
			m_rotation = 0;
		}
	}

	jump_update();

	if (m_is_alive)
	{
		if (m_moving_forward)
		{
			if (!m_facing_front)
			{
				m_scale.x = -m_scale.x;
				m_facing_front = true;
			}
			move({MOVING_SPEED, 0.0});
		}
		if (m_moving_backward)
		{
			if (m_facing_front)
			{
				m_scale.x = -m_scale.x;
				m_facing_front = false;
			}
			move({-MOVING_SPEED, 0.0});
		}

		if (m_crouch_state == CROUCH_PRESSED)
		{
			m_scale.y = 0.1f;
			m_position.y += 25.f;
			m_crouch_state = IS_CROUCHING;
		}
		if (m_crouch_state == CROUCH_RELEASED)
		{
			m_scale.y = 0.2f;
			m_position.y -= 25.f;
			m_crouch_state = NOT_CROUCHING;
		}

		if (m_is_punching)
		{
			m_is_hurt = true;

			m_health -= 1;
		}
		else
		{
			m_is_hurt = false;
		}
	}
	else
	{
		if (m_facing_front)
			m_rotation = -M_PI / 2;
		else
			m_rotation = M_PI / 2;
	}
}

void Fighter::draw(const mat3 &projection)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(get_position());
	transform_rotate(m_rotation);
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint is_hurt_uloc = glGetUniformLocation(effect.program, "is_hurt");
	GLuint time_uloc = glGetUniformLocation(effect.program, "time");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fighter_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform);
	float color[] = {1.f, 1.f, 1.f};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);
	glUniform1i(is_hurt_uloc, m_is_hurt);
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Fighter::get_position() const
{
	return m_position;
}

void Fighter::set_position(vec2 position)
{
	m_position = position;
}

void Fighter::move(vec2 off)
{
	m_position.x += off.x;
	m_position.y += off.y;
}

//// Returns the local bounding coordinates scaled by the current size of the bubble
vec2 Fighter::get_bounding_box() const
{
	// fabs is to avoid negative scale due to the facing direction
	return {std::fabs(m_scale.x) * fighter_texture.width, std::fabs(m_scale.y) * fighter_texture.height};
}

// set fighter's movements
void Fighter::set_movement(int mov)
{
	switch (mov)
	{
	case MOVING_FORWARD:
		m_moving_forward = true;
		m_is_idle = false;
		break;
	case MOVING_BACKWARD:
		m_moving_backward = true;
		m_is_idle = false;
		break;
	case START_JUMPING:
		start_jumping();
		break;
	case CROUCHING:
		m_crouch_state = CROUCH_PRESSED;
		m_is_idle = false;
		break;
	case PUNCHING:
		m_is_punching = true;
		m_is_idle = false;
		break;
	case STOP_MOVING_FORWARD:
		m_moving_forward = false;
		m_is_idle = true;
		break;
	case STOP_MOVING_BACKWARD:
		m_moving_backward = false;
		m_is_idle = true;
		break;
	case RELEASE_CROUCH:
		m_crouch_state = CROUCH_RELEASED;
		m_is_idle = true;
		break;
	case STOP_PUNCHING:
		m_is_punching = false;
		m_is_idle = true;
		break;
	}
}

int Fighter::get_health() const
{
	return m_health;
}

int Fighter::get_lives() const
{
	return m_lives;
}

void Fighter::start_jumping()
{
	if (!m_is_jumping && m_is_alive)
	{
		m_is_jumping = true;
		m_is_idle = false;
		m_vertical_velocity = INITIAL_VELOCITY;
	}
}

void Fighter::jump_update()
{
	if (m_is_jumping)
	{
		move({0.0, -m_vertical_velocity});
		m_vertical_velocity += ACCELERATION;
	}

	if (m_vertical_velocity < -INITIAL_VELOCITY)
	{
		m_is_jumping = false;
		m_vertical_velocity = 0.0;
	}
}

bool Fighter::is_jumping() const
{
	return m_is_jumping;
}

void Fighter::reset(int init_position)
{
	m_health = MAX_HEALTH;
	m_lives = STARTING_LIVES;
	m_is_alive = true;
	m_rotation = 0;
	m_is_jumping = false;
	m_vertical_velocity = 0;

	if (init_position == 1)
	{
		m_position = {250.f, 525.f};
	}
	else if (init_position == 2)
	{
		m_position = {950.f, 525.f};
	}
	else
	{
		m_position = {550.f, 525.f};
	}
}