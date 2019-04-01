#include "player1.hpp"
#include "fighter.hpp"

//implement player specific functions here

Texture Player1::p_texture;

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
	GLint blocking_tank_uloc = glGetUniformLocation(effect.program, "blocking_tank");
	GLint heal_animation_uloc = glGetUniformLocation(effect.program, "heal_animation");
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

	if (get_alive())
	{
		if (is_paused()) {

		}

		else if (is_tired_out()) 
		{
			set_power_punch(false);
			m_anim_counter++;
			if (m_anim_counter < 15)
			{
				set_sprite(TIRED_1);
			}

			else if (m_anim_counter > 15 && m_anim_counter < 75)
			{
				set_sprite(TIRED_2);
			}

			else if (m_anim_counter >= 75)
				m_anim_counter = 0;
		}

		else if (is_uppercutting())
		{
			m_punch_counter++;
			set_sprite(UPPERCUT);

			if (m_punch_counter > 30)
			{
				m_punch_counter = 0;
				set_uppercut(false);
			}
		}

		else if (is_punching())
		{
			if (!is_crouching()) { set_sprite(PUNCH); }
			else if (is_crouching()) { set_sprite(CROUCH_PUNCH); }
		}

		else if (!is_punching() && is_crouching()) { set_sprite(CROUCH); }

		else if (is_holding_power_punch() || (m_fc == POTATO && potato_is_holding_fries())) { set_sprite(CHARGING); }


		else if (is_power_punching())
		{
			m_punch_counter++;
			set_sprite(POWER_PUNCH);

			if (m_punch_counter > 20)
			{
				m_punch_counter = 0;
				set_power_punch(false);
			}
		}
		else if (!is_punching())
		{
			if (get_alive() && !is_crouching())
			{
				m_anim_counter++;
				if (m_anim_counter < 25) { set_sprite(IDLE); }
				else if (m_anim_counter > 25 && m_anim_counter < 50) { set_sprite(ORIGINAL); }
				else if (m_anim_counter >= 50)
					m_anim_counter = 0;
			}
		}
	}

	else if (!get_alive())
	{ set_sprite(DEATH); }

	glBindTexture(GL_TEXTURE_2D, p_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);
	glUniform1i(is_hurt_uloc, is_hurt());
	glUniform1i(is_blocking_uloc, is_blocking());
	glUniform1f(blocking_tank_uloc, (float) get_blocking_tank());
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	glUniform1f(heal_animation_uloc, get_heal_animation());

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

void Player1::set_sprite(SpriteType st) const {
	if (m_fc == POTATO) {
		switch (st) {
		default: p_texture = POTATO_TEXTURE; break;
		case IDLE: p_texture = POTATO_IDLE_TEXTURE; break;
		case PUNCH: p_texture = POTATO_PUNCH_TEXTURE; break;
		case POWER_PUNCH: p_texture = POTATO_POWER_PUNCH_TEXTURE; break;
		case CROUCH_PUNCH: p_texture = POTATO_CROUCH_PUNCH_TEXTURE; break;
		case CROUCH: p_texture = POTATO_CROUCH_TEXTURE; break;
		case CHARGING: p_texture = POTATO_CHARGING_TEXTURE; break;
		case DEATH: p_texture = POTATO_DEATH_TEXTURE; break;
		case TIRED_1: p_texture = POTATO_TIRED_1_TEXTURE; break;
		case TIRED_2: p_texture = POTATO_TIRED_2_TEXTURE; break;
		}
	}
	else if (m_fc == BROCCOLI) {
		switch (st) {
		default: p_texture = BROCCOLI_TEXTURE; break;
		case IDLE: p_texture = BROCCOLI_IDLE_TEXTURE; break;
		case PUNCH: p_texture = BROCCOLI_PUNCH_TEXTURE; break;
		case POWER_PUNCH: p_texture = BROCCOLI_POWER_PUNCH_TEXTURE; break; // TODO: STUB
		case CROUCH_PUNCH: p_texture = BROCCOLI_CROUCH_PUNCH_TEXTURE; break;
		case CROUCH: p_texture = BROCCOLI_CROUCH_TEXTURE; break;
		case DEATH: p_texture = BROCCOLI_DEATH_TEXTURE; break;
		case UPPERCUT: p_texture = BROCCOLI_UPPERCUT_TEXTURE; break;
		case TIRED_1: p_texture = BROCCOLI_TIRED_1_TEXTURE; break;
		case TIRED_2: p_texture = BROCCOLI_TIRED_2_TEXTURE; break;
		case CHARGING: p_texture = BROCCOLI_CHARGING_TEXTURE; break;
		}
	}
	else if (m_fc == EGGPLANT) { // TODO: STUB
		switch (st) {
		default: p_texture = EGGPLANT_TEXTURE; break;
		case IDLE: p_texture = EGGPLANT_IDLE_TEXTURE; break;
		case PUNCH: p_texture = EGGPLANT_PUNCH_TEXTURE; break;
		case POWER_PUNCH: p_texture = EGGPLANT_POWER_PUNCH_TEXTURE; break;
		case CROUCH_PUNCH: p_texture = EGGPLANT_CROUCH_PUNCH_TEXTURE; break;
		case CROUCH: p_texture = EGGPLANT_CROUCH_TEXTURE; break;
		case CHARGING: p_texture = EGGPLANT_CHARGING_TEXTURE; break;
		case DEATH: p_texture = EGGPLANT_DEATH_TEXTURE; break;
		case TIRED_1: p_texture = EGGPLANT_TIRED_1_TEXTURE; break;
		case TIRED_2: p_texture = EGGPLANT_TIRED_2_TEXTURE; break;
		}
	}
	else if (m_fc == YAM) {
		switch (st) {
		default: p_texture = YAM_TEXTURE; break;
		case IDLE: p_texture = YAM_IDLE_TEXTURE; break;
		case PUNCH: p_texture = YAM_PUNCH_TEXTURE; break;
		case POWER_PUNCH: p_texture = YAM_POWER_PUNCH_TEXTURE; break;
		case CROUCH_PUNCH: p_texture = YAM_CROUCH_PUNCH_TEXTURE; break;
		case CROUCH: p_texture = YAM_CROUCH_TEXTURE; break;
		case CHARGING: p_texture = YAM_CHARGING_TEXTURE; break;
		case DEATH: p_texture = YAM_DEATH_TEXTURE; break;
		case TIRED_1: p_texture = YAM_TIRED_1_TEXTURE; break;
		case TIRED_2: p_texture = YAM_TIRED_2_TEXTURE; break;
		}
	}
}