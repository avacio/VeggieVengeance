#include "bullet.hpp"

Texture Bullet::bullet_texture;

Bullet::Bullet(int id, vec2 pos, unsigned int damage, bool direction) {
	//variable bullet attributes
	this->m_fighter_id = id;
	this->m_position = pos;
	this->m_damage = damage;

	//pre-determined bullet attributes
	this->m_scale = vec2({ 0.1f, 0.15f });
	this->m_velocity = vec2({ 7.0f, 0.0f });
	//flip velocity if moving left
	if (!direction) {
		this->m_velocity.x *= -1.0;
	}
	this->m_width = std::fabs(this->m_scale.x) * bullet_texture.width;
	this->m_height = std::fabs(this->m_scale.y) * bullet_texture.height;
	this->m_delete_when = AFTER_HIT;
	this->m_damageEffect = new DamageEffect(this->m_position.x, this->m_position.y, this->m_width, this->m_height, this->m_damage, this->m_fighter_id, this->m_delete_when);
}

Bullet::~Bullet() {
	delete m_damageEffect;

	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	glDisableVertexAttribArray(0);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
	effect.release();
	printf("destroyed bullet\n");
}


bool Bullet::init() {
	if (!bullet_texture.is_valid()) {
		if (!bullet_texture.load_from_file(textures_path("french_fry.png"))) {
			fprintf(stderr, "Failed to load bullet texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture
	float wr = bullet_texture.width * 0.5;
	float hr = bullet_texture.height * 0.5;

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

void Bullet::update() {
	m_position.x += m_velocity.x;
	m_damageEffect->m_bounding_box.xpos = m_position.x;
	m_damageEffect->m_bounding_box.ypos = m_position.y;
}

void Bullet::draw(const mat3 &projection) {
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
	glBindTexture(GL_TEXTURE_2D, bullet_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	
}