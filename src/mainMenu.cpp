// Header
#include "mainMenu.hpp"

Texture MainMenu::m_texture;

///////////////
// NOTE: REPEAT FROM BACKGROUND.HPP MIGHT NOT BE NECESSARY
// if we only want a monocolour background (i.e. water) we can copy that format instead

bool MainMenu::init(vec2 screen)
{
	// Load shared texture
	if (!m_texture.is_valid())
	{
		if (!m_texture.load_from_file(textures_path("mainMenu.jpg")))
		{
			fprintf(stderr, "Failed to load background texture!");
			return false;
		}
	}
	this->screen = screen;

	// The position corresponds to the center of the texture
	float wr = m_texture.width * 3.5f;
	float hr = m_texture.height * 3.5f;

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

	//m_scale.x = 0.1725f;
	//m_scale.y = 0.1725f;
	m_scale.x = 0.04f;
	m_scale.y = 0.04f;
	m_rotation = 0.f;
	m_position = { 595.f, 455.f };

	////////////////
	//// TEXT
	title = new TextRenderer(mainFontBold, 90);
	title->setColor({ 0.f,0.f,0.f });
	int width = title->get_width_of_string("VEGGIEVENGEANCE");
	title->setPosition({ screen.x/2.f - width/2.f, 180.f });
	init_buttons();

	//fprintf(stderr, "Loaded text\n");

	return true;
}

// Call if init() was successful
// Releases all graphics resources
void MainMenu::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void MainMenu::draw(const mat3& projection)
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
	glBindTexture(GL_TEXTURE_2D, m_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));

	/////////////////////////

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	title->renderString(projection, "VEGGIE VENGEANCE");
	buttons[0]->renderString(projection, "ONE-PLAYER");
	buttons[1]->renderString(projection, "TWO-PLAYER");
}

vec2 MainMenu::get_position()const
{
	return m_position;
}

void MainMenu::set_position(vec2 position)
{
	m_position = position;
}

void MainMenu::init_buttons()
{
	selectedButtonIndex = 0; // default: first button selected
	TextRenderer* play1 = new TextRenderer(mainFont, 60);
	TextRenderer* play2 = new TextRenderer(mainFont, 60);

	play1->setColor(selectedColor);
	play2->setColor(defaultColor);

	int width = play1->get_width_of_string("ONE-PLAYER");
	play1->setPosition({ screen.x / 2.f - width / 2.f, screen.y / 2.f-50.f });
	width = play2->get_width_of_string("TWO-PLAYER");
	play2->setPosition({ screen.x / 2.f - width / 2.f, (screen.y/2.f) +50.f });
	buttons.emplace_back(play1);
	buttons.emplace_back(play2);
}

// TODO: may want to change design so that selection does not loop
//void MainMenu::change_selection(int direction) // -1 for down, 1 for up
void MainMenu::change_selection()
{
	if (selectedButtonIndex == 0) {
		selectedButtonIndex = 1;
		buttons[0]->setColor(defaultColor);
		buttons[1]->setColor(selectedColor);
	}
	else {
		selectedButtonIndex = 0;
		buttons[1]->setColor(defaultColor);
		buttons[0]->setColor(selectedColor);
	}
}

GameMode MainMenu::get_selected()
{
	switch (selectedButtonIndex) {
	case 0:
		return PVC;
		break;
	case 1:
		return PVP;
		break;
	}
}
