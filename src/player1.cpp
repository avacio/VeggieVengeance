// Header
#include "player1.hpp"
#include "fighter.hpp"

//implement player specific functions here

Texture Player1::player1_texture;

Player1::Player1(unsigned int id) : Fighter(id) {
}

void Player1::draw(const mat3 &projection)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(get_position());
	transform_rotate(get_rotation());
	transform_scale(get_scale());
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
	GLint is_blocking_uloc = glGetUniformLocation(effect.program, "is_blocking");
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
	if (get_alive() && is_punching()) {
		player1_texture = POTATO_PUNCH_TEXTURE;
	} else {
		if (get_alive() && is_idle()) {
			m_idle_counter++;
			if (m_idle_counter < 25) {
				player1_texture = POTATO_IDLE_TEXTURE;
			}

			else if (m_idle_counter > 25 && m_idle_counter < 50) {
				player1_texture = POTATO_TEXTURE;
			}

			else if (m_idle_counter >= 50)
				m_idle_counter = 0;
		} else if(!get_alive()) {
			player1_texture = POTATO_TEXTURE;
		}
	}
	glBindTexture(GL_TEXTURE_2D, player1_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);
	glUniform1i(is_hurt_uloc, is_hurt());
	glUniform1i(is_blocking_uloc, is_blocking());
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

	int sWidth = m_nameplate->get_width_of_string(m_name);
	m_nameplate->setPosition({ m_position.x - sWidth * .45f, m_position.y - 70.0f });
}

bool Player1::get_in_play() const
{
	return m_in_play;
}

void Player1::set_in_play(bool value)
{
	m_in_play = value;
}