#include "emoji.hpp"

Emoji::Emoji(int id, vec2 pos, unsigned int damage, bool direction) {
	//variable bullet attributes
	this->m_fighter_id = id;
	this->m_position = pos;
	this->fighter_position = pos;
	this->m_damage = damage;

	//pre-determined bullet attributes
	this->m_scale = vec2({ 0.5f, 0.5f });
	this->m_velocity = vec2({ 7.0f, 0.0f });
	
	set_texture();
	//this->emoji_texture = &EMOJI_SWEAT_TEXTURE;
	this->m_width = std::fabs(this->m_scale.x) * emoji_texture->width;
	this->m_height = std::fabs(this->m_scale.y) * emoji_texture->height;
	this->m_delete_when = AFTER_HIT;
	this->m_damageEffect = new DamageEffect(this->m_position.x, this->m_position.y, this->m_width, this->m_height, this->m_damage, this->m_fighter_id, this->m_delete_when, 0);
}

Emoji::~Emoji() {
	delete m_damageEffect;

	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	glDisableVertexAttribArray(0);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
	effect.release();
}


bool Emoji::init() {
	// The position corresponds to the center of the texture
	float wr = emoji_texture->width * 0.5;
	float hr = emoji_texture->height * 0.5;

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
}

void Emoji::update(float ms) {
	if (state == FIRED) {
		m_position.x += m_velocity.x;
		m_damageEffect->m_bounding_box.xpos = m_position.x;
		m_damageEffect->m_bounding_box.ypos = m_position.y;
	}
	else if (state == CIRCLING){
		m_position.x  = fighter_position.x +  cos(angle)*radius;
		m_position.y = fighter_position.y + sin(angle)*radius;
		m_damageEffect->m_bounding_box.xpos = m_position.x;
		m_damageEffect->m_bounding_box.ypos = m_position.y;
		angle += angle_increment;
		if (angle >= 2 * PI) {
			//to prevent overflow
			angle = 0;
		}
	}
}

void Emoji::draw(const mat3 &projection) {
	transform_begin();
	transform_translate(m_position);
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
	glBindTexture(GL_TEXTURE_2D, emoji_texture->id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

}

void Emoji::set_texture() {
	int texture_no = get_random_number(3);
	switch (texture_no) {
		case 0 :
			this->emoji_texture = &EMOJI_MOUTH_TEXTURE;
			break;
		case 1 :
			this->emoji_texture = &EMOJI_SWEAT_TEXTURE;
			break;
		case 2:
			this->emoji_texture = &EMOJI_100_TEXTURE;
			break;
		case 3:
			this->emoji_texture = &EMOJI_OKHAND_TEXTURE;
			break;
	}
}

void Emoji::fire_emoji(bool direction) {
	state = FIRED;
	//flip velocity if moving left
	if (!direction) {
		this->m_velocity.x *= -1.0;
	}
}

void Emoji::set_fighter_pos(vec2 pos) {
	fighter_position = pos;
}

bool Emoji::is_circling() {
	return state == CIRCLING;
}