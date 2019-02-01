// Header
#include "fighter.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

Texture Fighter::fighter_texture;

bool Fighter::init()
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
	vertices[0].position = { -wr, +hr, -0.02f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { 1.f, 1.f };
	vertices[2].position = { +wr, -hr, -0.02f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.02f };
	vertices[3].texcoord = { 0.f, 0.f };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

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

	is_alive = true;
	is_idle = true;
	facing_front = true;
	is_hurt = false;

	m_scale.x = 0.2f;
	m_scale.y = 0.2f;
	m_rotation = 0.f;
	m_health = 100;
	m_speed = 5;
	m_strength = 5;

	m_position = { 250.f, 550.f };
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
	float MOVING_SPEED = 5.0;
	//!!! cant use ms for jumping until we have collision since ms
	//is inconsistent per update and will result in Fighter ending u
	//at a different ypos than at initially
	float JUMP_SPEED = 5.0;

	//IF JUST DIED
	if (m_health <= 0) {
		is_alive = false;
		//fall to ground if dead
		if (jumpstate == JUMPING) {
			jumpstate = FALLING;
		}
	}

	//Fall regardless whether alive or not
	if (jumpstate == FALLING) {
		jumpcounter--;
		move({ 0.0, JUMP_SPEED });
		if (jumpcounter <= 0)
			jumpstate = GROUNDED;
	}
 
	if (is_alive) {
		if (moving_forward) {
			if (!facing_front) {
				m_scale.x = -m_scale.x;
				facing_front = true;
			}
			move({ MOVING_SPEED, 0.0 });
		}
		if (moving_backward) {
			if (facing_front) {
				m_scale.x = -m_scale.x;
				facing_front = false;
			}
			move({ -MOVING_SPEED, 0.0 });
		}		
		if (jumpstate == JUMPING) {
			jumpcounter++;
			move({ 0.0, -JUMP_SPEED });
			if (jumpcounter >= MAX_JUMP)
				jumpstate = FALLING;
		}
		

		if (crouchstate == CROUCH_PRESSED) {
			m_scale.y = 0.1f;
			m_position.y += 15.f;
			crouchstate = IS_CROUCHING;
		}
		if (crouchstate == CROUCH_RELEASED) {
			m_scale.y = 0.2f;
			m_position.y -= 15.f;
			crouchstate = NOT_CROUCHING;
		}
			

		if (is_punching) {
			is_hurt = true;

			m_health -= 1;
		}
		else { is_hurt = false; }
	}
	else {
		if (facing_front)
			m_rotation = -M_PI / 2;
		else
			m_rotation = M_PI / 2;
	}
}

void Fighter::draw(const mat3& projection)
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
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fighter_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniform1i(is_hurt_uloc, is_hurt);
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));


	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Fighter::get_position()const
{
	return m_position;
}


void Fighter::set_position(vec2 position)
{
	m_position = position;
}

void Fighter::move(vec2 off)
{
	m_position.x += off.x; m_position.y += off.y;
}

//// Returns the local bounding coordinates scaled by the current size of the bubble 
vec2 Fighter::get_bounding_box()const
{
	// fabs is to avoid negative scale due to the facing direction
	return { std::fabs(m_scale.x) * fighter_texture.width, std::fabs(m_scale.y) * fighter_texture.height };
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
		jumpstate = JUMPING;
		is_idle = false;
		break;
	case 3:
		crouchstate = CROUCH_PRESSED;
		is_idle = false;
		break;
	case 4:
		is_punching = true;
		is_idle = false;
		break;
	case 5:
		moving_forward = false;
		is_idle = true;
		break;
	case 6:
		moving_backward = false;
		is_idle = true;
		break;
	case 7:
		crouchstate = CROUCH_RELEASED;
		is_idle = true;
		break;
	case 8:
		is_punching = false;
		is_idle = true;
		break;
	}
}

int Fighter::get_health()const
{
	return m_health;
}

jumpState Fighter::get_jumpstate()const
{
	return jumpstate;
}