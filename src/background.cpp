// Header
#include "background.hpp"

Texture Background::bg_texture;

bool Background::init(vec2 screen, GameMode mode)
{
	m_mode = mode;

	// Load shared texture
	if (!bg_texture.is_valid())
	{
		if (!bg_texture.load_from_file(textures_path("background.png")))
		{
			fprintf(stderr, "Failed to load background texture!");
			return false;
		}
	}
	this->screen = screen;

	// The position corresponds to the center of the texture
	float wr = bg_texture.width * 3.5f;
	float hr = bg_texture.height * 3.5f;

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

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture

	m_scale.x = 0.1725f;
	m_scale.y = 0.1725f;
	m_rotation = 0.f;
	m_position = { 595.f, 455.f };

	////////////////
	//// TEXT
	health1 = new TextRenderer(mainFont, 44);
	health2 = new TextRenderer(mainFont, 44);
	int width = health1->get_width_of_string("HP: 100"); // TODO
	health1->setPosition({ 50.f, 100.f });
	health2->setPosition({ screen.x-(width*1.15f), 100.f });
	//health2->setPosition({ screen.x - width, 100.f });


	lives1 = new TextRenderer(mainFont, 70);
	lives2 = new TextRenderer(mainFont, 70);
	width = lives2->get_width_of_string("XXX");
	lives1->setPosition({ 50.f, 180.f });
	lives2->setPosition({ screen.x-(width*1.05f), 180.f });

	jump = new TextRenderer(mainFont, 44);
	jump ->setPosition({50.f, 230.f});
	left = new TextRenderer(mainFont, 44);
	left ->setPosition({50.f, 280.f});
	right = new TextRenderer(mainFont, 44);
	right ->setPosition({50.f, 330.f});
	crouch = new TextRenderer(mainFont, 44);
	crouch ->setPosition({50.f, 380.f});
	reset = new TextRenderer(mainFont, 44);
	reset ->setPosition({50.f, 430.f});
	pause = new TextRenderer(mainFont, 44);
	pause ->setPosition({50.f, 480.f});

	fprintf(stderr, "Loaded text\n");

	return true;
}

// Call if init() was successful
// Releases all graphics resources
void Background::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Background::draw(const mat3& projection)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(get_position());
	//transform_rotate(m_rotation);
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
	glBindTexture(GL_TEXTURE_2D, bg_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));

	/////////////////////////

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	//if (p1Lives != -1 && p2Lives != -1) { drawPlayerInfo(projection); }
	drawPlayerInfo(projection);

	if (m_mode == TUTORIAL) {
		drawTutorialText(projection);
	}
}

vec2 Background::get_position()const
{
	return m_position;
}


void Background::set_position(vec2 position)
{
	m_position = position;
}

void Background::setPlayerInfo(int p1Lives, int p1HP, int p2Lives, int p2HP) {
	this->p1Lives = p1Lives;
	this->p1HP = p1HP;
	this->p2Lives = p2Lives;
	this->p2HP = p2HP;
}

void Background::drawPlayerInfo(const mat3& projection) {
	std::stringstream ss1, ss2;
	ss1 << "HP: " << p1HP;
	ss2 << "HP: " << p2HP;

	health1->renderString(projection, ss1.str());
	health2->renderString(projection, ss2.str());

	switch (p1Lives) {
	case 3:
		lives1->renderString(projection, "XXX");
		break;
	case 2:
		lives1->renderString(projection, "XX");
		break;
	case 1:
		lives1->renderString(projection, "X");
		break;
	}
	switch (p2Lives) {
	case 3:
		lives2->renderString(projection, "XXX");
		break;
	case 2:
		lives2->renderString(projection, "XX");
		break;
	case 1:
		lives2->renderString(projection, "X");
		break;
	}
}

void Background::drawTutorialText(const mat3& projection) {
	jump->renderString(projection, "W/I: Jump");
	left->renderString(projection, "A/J: Move left");
	right->renderString(projection, "D/L: Move right");
	crouch->renderString(projection, "S/K: Crouch");
	pause->renderString(projection, "Esc: Pause");
	reset->renderString(projection, "B: Reset");
}