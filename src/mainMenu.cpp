// Header
#include "mainMenu.hpp"

Texture MainMenu::m_texture;

bool MainMenu::init(vec2 screen, std::map<FighterCharacter, FighterInfo> fighterMap)
{
	// Load shared texture
	m_texture = MAIN_MENU_TEXTURE;

	this->screen = screen;
	this->fighterMap = fighterMap;

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
	title = new TextRenderer(mainFontBold, 80);
	title->setColor({ 0.f,0.f,0.f });
	int width = title->get_width_of_string("VEGGIEVENGEANCE");
	//title->setPosition({ screen.x/2.f - width/2.f, 180.f });
	title->setPosition({ 80.f, 180.f });
	set_mode(MENU);

	return true;
}

// Call if init() was successful
// Releases all graphics resources
void MainMenu::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
	delete title;

	for (int i = 0; i < buttons.size(); i++) {
		TextRenderer *button = buttons[i];
		delete button;
	}

	buttons.clear();
	effect.release();
	std::cout << "Destroyed MainMenu." << std::endl;
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
	if (m_mode == MENU) {
		title->renderString(projection, "VEGGIE VENGEANCE");
		buttons[0]->renderString(projection, "ONE-PLAYER");
		buttons[1]->renderString(projection, "TWO-PLAYER");
		buttons[2]->renderString(projection, "TUTORIAL");
	}
	else {
		if (m_selected_mode == PVP) {
			if (!is_player_1_chosen) { title->renderString(projection, "P1 CHARACTER SELECT"); }
			//if (selectedP1 == BLANK) { title->renderString(projection, "P1 CHARACTER SELECT"); }
			else { title->renderString(projection, "P2 CHARACTER SELECT"); }
		} else { title->renderString(projection, "CHARACTER SELECT"); }
		buttons[0]->renderString(projection, "POTATO");
		buttons[1]->renderString(projection, "BROCCOLI");
		buttons[2]->renderString(projection, "return");
		draw_char_info(projection);
	}
}

vec2 MainMenu::get_position()const
{
	return m_position;
}

void MainMenu::set_position(vec2 position)
{
	m_position = position;
}

void MainMenu::init_menu_buttons()
{
	TextRenderer* b1 = new TextRenderer(mainFont, 50);
	TextRenderer* b2 = new TextRenderer(mainFont, 50);
	TextRenderer* b3 = new TextRenderer(mainFont, 50);

	b1->setColor(selectedColor);
	b2->setColor(defaultColor);
	b3->setColor(defaultColor);

	int width = b1->get_width_of_string("ONE-PLAYER");
	b1->setPosition({ screen.x / 2.f - width / 2.f, screen.y / 2.f-100.f });
	width = b2->get_width_of_string("TWO-PLAYER");
	b2->setPosition({ screen.x / 2.f - width / 2.f, (screen.y/2.f) });
	width = b3->get_width_of_string("TUTORIAL");
	b3->setPosition({ screen.x / 2.f - width / 2.f, (screen.y / 2.f) + 100.f });
	buttons.emplace_back(b1);
	buttons.emplace_back(b2);
	buttons.emplace_back(b3);

	reset_selection();
}

void MainMenu::init_select_char_buttons() {
	TextRenderer* t1 = new TextRenderer(mainFont, 40);
	TextRenderer* t2 = new TextRenderer(mainFont, 40);
	TextRenderer* t3 = new TextRenderer(mainFont, 40);

	t1->setColor({0.f,0.f,0.f});
	t2->setColor({ 0.f,0.f,0.f });
	t3->setColor({ 0.f,0.f,0.f });

	int width = t1->get_width_of_string("solanum tuberosum");
	t1->setPosition({ screen.x / 2.f, screen.y / 2.f - 100.f }); //  width / 4.f
	t2->setPosition({ screen.x / 2.f, (screen.y / 2.f) });
	t3->setPosition({ screen.x / 2.f, (screen.y / 2.f) + 100.f });
	text.emplace_back(t1);
	text.emplace_back(t2);
	text.emplace_back(t3);

	width = buttons[0]->get_width_of_string("POTATO");
	buttons[0]->setPosition({ width / 3.f, screen.y / 2.f - 100.f });
	buttons[1]->setPosition({ width / 3.f, (screen.y / 2.f) });
	buttons[2]->setPosition({ width / 3.f, (screen.y / 2.f) + 100.f });

	reset_selection();
}

GameMode MainMenu::set_selected_mode()
{
	switch (selectedButtonIndex) {
	case 0: {
		m_selected_mode = PVC;
		break; }
	case 1: {
		m_selected_mode = PVP; 
		break; }
	case 2: {
		m_selected_mode = TUTORIAL;
		break;
		}
	}
	return m_selected_mode;
}

FighterCharacter MainMenu::get_selected_char()
{
	switch (selectedButtonIndex) {
	case 0:
		return POTATO;
	case 1:
		return BROCCOLI;
	case 2: {
		return BLANK;
	}
	default:
		return POTATO;
	}
}

void MainMenu::draw_char_info(const mat3& projection)
{
	if (get_selected_char() == BLANK) { return; }
	text[0]->renderString(projection, fighterMap[get_selected_char()].sciName);
	text[1]->renderString(projection, "STR: " + int_to_stat_string(fighterMap[get_selected_char()].strength));
	text[2]->renderString(projection, "SPD: " + int_to_stat_string(fighterMap[get_selected_char()].speed));
}

std::string MainMenu::int_to_stat_string(int in) {
	std::string stat = "";
	for (int i = 0; i < in; i++) {
		stat += "X";
	}
	return stat;
}

bool MainMenu::set_mode(GameMode mode)
{
	m_mode = mode;
	switch (mode) {
	case MENU: {
		reset();
		init_menu_buttons();
	}
	case CHARSELECT:
		init_select_char_buttons();
		break;
	default:
		break;
	}
	return true;
}

void MainMenu::reset() {
	destroyButtons();
	for (int i = 0; i < text.size(); i++) {
		TextRenderer *t = text[i];
		delete t;
	}
	text.clear();
}


void Screen::change_selection(bool goDown)
{
	buttons[selectedButtonIndex]->setColor(defaultColor);
	if (selectedButtonIndex == buttons.size() - 1 && goDown) {
		buttons[0]->setColor(selectedColor);
		selectedButtonIndex = 0;
	}
	else if (selectedButtonIndex == 0 && !goDown) {
		buttons[buttons.size() - 1]->setColor(selectedColor);
		selectedButtonIndex = buttons.size() - 1;
	}
	else if (goDown) {
		selectedButtonIndex++;
		buttons[selectedButtonIndex]->setColor(selectedColor);
	}
	else {
		selectedButtonIndex--;
		buttons[selectedButtonIndex]->setColor(selectedColor);
	}
}

void Screen::reset_selection()
{
	buttons[selectedButtonIndex]->setColor(defaultColor);
	buttons[0]->setColor(selectedColor);
	selectedButtonIndex = 0;
}
