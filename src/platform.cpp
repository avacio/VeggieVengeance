
#include "platform.hpp"

Texture Platform::platform_texture;

Platform::Platform(float xpos, float ypos, float width, float height) : m_bounding_box(xpos, ypos, width, height) {
	m_position = { xpos, ypos };
	m_size = { width, height };
	m_scale = { 1.0, 1.0 };
}

BoundingBox Platform::get_bounding_box() {
	return m_bounding_box;
}

bool Platform::check_collision(BoundingBox b) {
	return m_bounding_box.check_collision(b);
}

/*bool Platform::check_collision_outer_y(BoundingBox b) {
	bool collides = false;
	vec2 centerPos = { b.xpos + (b.width * 0.5), b.ypos + (b.height * 0.5) };
	BoundingBox * topEdge = new BoundingBox(m_bounding_box.xpos, m_bounding_box.ypos, m_bounding_box.width, OUTER_DEPTH);
	BoundingBox * bottomEdge = new BoundingBox(m_bounding_box.xpos, m_bounding_box.ypos + m_bounding_box.height - OUTER_DEPTH,
		m_bounding_box.width, OUTER_DEPTH);

	if ((topEdge->check_collision(b) && topEdge->ypos > centerPos.y) || (bottomEdge->check_collision(b)) && bottomEdge->ypos < centerPos.y) {
		collides = true;
	}
	delete topEdge;
	delete bottomEdge;

	return collides;
}

bool Platform::check_collision_outer_x(BoundingBox b) {
	bool collides = false;
	vec2 centerPos = {b.xpos + (b.width * 0.5), b.ypos + (b.height * 0.5) };
	BoundingBox * leftEdge = new BoundingBox(m_bounding_box.xpos, m_bounding_box.ypos, OUTER_DEPTH, m_bounding_box.height);
	BoundingBox * rightEdge = new BoundingBox(m_bounding_box.xpos + m_bounding_box.width - OUTER_DEPTH, m_bounding_box.ypos,
		OUTER_DEPTH, m_bounding_box.height);

	if ((leftEdge->check_collision(b) && leftEdge->xpos > centerPos.x) || (rightEdge->check_collision(b)) && rightEdge->xpos < centerPos.x) {
		collides = true;
	}
	delete leftEdge;
	delete rightEdge;

	return collides;
}*/

bool Platform::check_collision_outer_top(BoundingBox b) {
	bool collides = false;
	vec2 centerPos = { b.xpos + (b.width * 0.5), b.ypos + (b.height * 0.5) };
	BoundingBox * topEdge = new BoundingBox(m_bounding_box.xpos, m_bounding_box.ypos, m_bounding_box.width, OUTER_DEPTH);

	if ((topEdge->check_collision(b) && topEdge->ypos > centerPos.y)) {
		collides = true;
	}
	delete topEdge;

	return collides;
}

bool Platform::check_collision_outer_bottom(BoundingBox b) {
	bool collides = false;
	vec2 centerPos = { b.xpos + (b.width * 0.5), b.ypos + (b.height * 0.5) };
	BoundingBox * bottomEdge = new BoundingBox(m_bounding_box.xpos, m_bounding_box.ypos + m_bounding_box.height - OUTER_DEPTH,
		m_bounding_box.width, OUTER_DEPTH);

	if ((bottomEdge->check_collision(b)) && bottomEdge->ypos < centerPos.y) {
		collides = true;
	}
	delete bottomEdge;

	return collides;
}

bool Platform::check_collision_outer_left(BoundingBox b) {
	bool collides = false;
	vec2 centerPos = { b.xpos + (b.width * 0.5), b.ypos + (b.height * 0.5) };
	BoundingBox * leftEdge = new BoundingBox(m_bounding_box.xpos, m_bounding_box.ypos, OUTER_DEPTH, m_bounding_box.height);

	if ((leftEdge->check_collision(b) && leftEdge->xpos > centerPos.x)) {
		collides = true;
	}
	delete leftEdge;

	return collides;
}

bool Platform::check_collision_outer_right(BoundingBox b) {
	bool collides = false;
	vec2 centerPos = { b.xpos + (b.width * 0.5), b.ypos + (b.height * 0.5) };
	BoundingBox * rightEdge = new BoundingBox(m_bounding_box.xpos + m_bounding_box.width - OUTER_DEPTH, m_bounding_box.ypos,
		OUTER_DEPTH, m_bounding_box.height);

	if ((rightEdge->check_collision(b)) && rightEdge->xpos < centerPos.x) {
		collides = true;
	}
	delete rightEdge;

	return collides;
}


bool Platform::init() {
	platform_texture = PLATFORM_TEXTURE;

	// The position corresponds to the center of the texture
	//float wr = platform_texture.width * 3.5f;
	//float hr = platform_texture.height * 3.5f;
	//float wr = m_size.x / 2;
	//float hr = m_size.y / 2;
	float width = m_size.x;
	float height = m_size.y;

	/*TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.02f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { 1.f, 1.f };
	vertices[2].position = { +wr, -hr, -0.02f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.02f };
	vertices[3].texcoord = { 0.f, 0.f };*/

	TexturedVertex vertices[4];
	vertices[0].position = { 0.0, height, -0.02f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { width, height, -0.02f };
	vertices[1].texcoord = { 1.f, 1.f };
	vertices[2].position = { width, 0.0, -0.02f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { 0.0, 0.0, -0.02f };
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

	return true;
}

void Platform::destroy() {
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);
	glDeleteVertexArrays(1, &mesh.vao);
	glDisableVertexAttribArray(0);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
	//delete &m_bounding_box;
	effect.release();
}

void Platform::draw(const mat3 &projection) {

	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(m_position);
	//transform_rotate(m_rotation);
	//transform_scale(m_scale);
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
	glBindTexture(GL_TEXTURE_2D, platform_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}