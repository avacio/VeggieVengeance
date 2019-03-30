// Header
#include "background.hpp"

Texture Background::bg_texture;

bool Background::init(vec2 screen, GameMode mode)
{
	m_mode = mode;

	// Load shared texture
	bg_texture = BACKGROUND_TEXTURE;
	this->screen = screen;

	// The position corresponds to the center of the texture
	float wr = bg_texture.width * 1.f;
	float hr = bg_texture.height * 1.f;

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

	m_scale.x = 0.4f;
	m_scale.y = 0.4f;
	m_rotation = 0.f;
	m_position = { 600.f, 400.f };

	m_initialized = true;

	//////////////
	// TEXT
	health1 = new TextRenderer(mainFont, 44);
	health2 = new TextRenderer(mainFont, 44);
	int width = health1->get_width_of_string("HP: 100"); // TODO
	health1->setPosition({ 50.f, 100.f });
	health2->setPosition({ screen.x-(width*1.15f), 100.f });
	block1 = new TextRenderer(mainFont, 44);
	block2 = new TextRenderer(mainFont, 44);
	int width1 = block1->get_width_of_string("BLOCK: 100"); // TODO
	block1->setPosition({ 50.f, 250.f });
	block2->setPosition({ screen.x - (width1*1.15f), 250.f });

	lives1 = new TextRenderer(mainFont, 70);
	lives2 = new TextRenderer(mainFont, 70);
	width = lives2->get_width_of_string("XXX");
	lives1->setPosition({ 50.f, 180.f });
	lives2->setPosition({ screen.x-(width*1.05f), 180.f });

	isPausedText = new TextRenderer(mainFontBold, 60);
	width = isPausedText->get_width_of_string("PAUSED");
	isPausedText->setPosition({ screen.x/2.f - width / 2.f, 100.f }); // screen.x/2.f - width*0.4f
	isPausedText->setColor({ 0.f,0.f,0.f });
	
	unsigned int tutorialTextSize = 30;
	jump = new TextRenderer(mainFont, tutorialTextSize);
	jump->setPosition({50.f, 230.f});
	left = new TextRenderer(mainFont, tutorialTextSize);
	left->setPosition({50.f, 280.f});
	right = new TextRenderer(mainFont, tutorialTextSize);
	right->setPosition({50.f, 330.f});
	crouch = new TextRenderer(mainFont, tutorialTextSize);
	crouch->setPosition({50.f, 380.f});
	reset = new TextRenderer(mainFont, tutorialTextSize);
	reset->setPosition({50.f, 430.f});
	pause = new TextRenderer(mainFont, tutorialTextSize);
	pause->setPosition({50.f, 480.f});
	ability1 = new TextRenderer(mainFont, tutorialTextSize);
	width = ability1->get_width_of_string("VNUM2:SpecialAbility");
	ability1->setPosition({screen.x-(width*1.15f), 230.f});
	ability2 = new TextRenderer(mainFont, tutorialTextSize);
	width = ability2->get_width_of_string("BNUM3:SpecialAbility");
	ability2->setPosition({ screen.x - (width*1.15f), 280.f });
	shield = new TextRenderer(mainFont, tutorialTextSize);
	width = shield->get_width_of_string("LShift/RShift:Sheild");
	shield->setPosition({screen.x-(width*1.15f), 330.f});
	punch = new TextRenderer(mainFont, tutorialTextSize);
	width = punch->get_width_of_string("C/NUM1Punch");
	punch->setPosition({screen.x-(width*1.15f), 380.f});
	pauseMusic = new TextRenderer(mainFont, tutorialTextSize);
	width = pauseMusic->get_width_of_string("InsertPauseResumeSong");
	pauseMusic->setPosition({screen.x-(width*1.15f), 430.f});
	randomSong = new TextRenderer(mainFont, tutorialTextSize);
	width = randomSong->get_width_of_string("HomeRandomSong");
	randomSong->setPosition({screen.x-(width*1.15f), 480.f});
	changeSong = new TextRenderer(mainFont, tutorialTextSize);
	width = changeSong->get_width_of_string("Delete/EndPrevNextsong");
	changeSong->setPosition({ screen.x - (width*1.15f), 530.f});
	changeVolume = new TextRenderer(mainFont, tutorialTextSize);
	width = changeVolume->get_width_of_string("PageUpDownInc./Dec.volume");
	changeVolume->setPosition({screen.x-(width*1.15f), 580.f});

	winnerText = new TextRenderer(mainFontBold, 38);
	winnerText->setColor({ 0.4f,0.1f,0.1f });
	width = winnerText->get_width_of_string("CREAM OF CROP:aaaaaaaaaaaaaaa");
	winnerText->setPosition({ screen.x / 2.f - width / 2.f, 100.f });

	init_buttons();

	return true;
}

// Call if init() was successful
// Releases all graphics resources
void Background::destroy()
{
	m_initialized = false;
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);

	for (std::map<TextRenderer*, std::string>::iterator iter = nameplates.begin(); iter != nameplates.end(); ++iter) {
		TextRenderer* nameplate = iter->first;
		delete nameplate;
	}
	nameplates.clear();
	destroyButtons();
	winnerName = "";

	delete health1;
	delete health2;
	delete block1;
	delete block2;
	delete lives1;
	delete lives2;
	delete isPausedText;
	delete jump;
	delete left;
	delete right;
	delete crouch;
	delete pause;
	delete reset;
	delete ability1;
	delete ability2;
	delete shield;
	delete punch;
	delete pauseMusic;
	delete randomSong;
	delete changeSong;
	delete changeVolume;
	delete winnerText;
	effect.release();
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
	drawPlayerInfo(projection);

	if (m_help_on) {
		drawTutorialText(projection);
	}
	if (m_mode == TUTORIAL || m_paused || m_is_game_over) {
		handleText(projection);
	}
	drawNameplates(projection);
}

vec2 Background::get_position()const
{
	return m_position;
}


void Background::set_position(vec2 position)
{
	m_position = position;
}

void Background::setPaused(bool isPaused) {
	m_paused = isPaused;
}

void Background::setHelp(bool isHelpOn) {
	m_help_on = isHelpOn;
}

bool Background::getHelp() {
	return m_help_on;
}

void Background::setPlayerInfo(int p1Lives, int p1HP, int p1BL, int p2Lives, int p2HP, int p2BL) {
	this->p1Lives = p1Lives;
	this->p1HP = p1HP;
	this->p1BL = p1BL;
	this->p2Lives = p2Lives;
	this->p2HP = p2HP;
	this->p2BL = p2BL;
}

void Background::drawPlayerInfo(const mat3& projection) {
	std::stringstream ss1, ss2;
	ss1 << "HP: " << p1HP;
	ss2 << "HP: " << p2HP;

	health1->renderString(projection, ss1.str());
	health2->renderString(projection, ss2.str());

	/*std::stringstream blss1, blss2;
	blss1 << "BLOCK: " << p1BL;
	blss2 << "BLOCK: " << p2BL;

	block1->renderString(projection, blss1.str());
	block2->renderString(projection, blss2.str());*/

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

void Background::handleText(const mat3& projection) {
	if (m_is_game_over) {
		winnerText->renderString(projection, "CREAM OF THE CROP: " + winnerName);
		buttons[0]->renderString(projection, "RESTART");
		buttons[1]->renderString(projection, "MAIN MENU");
		buttons[2]->renderString(projection, "QUIT");
	}
	else if (m_paused) {
		isPausedText->renderString(projection, "PAUSED");
		buttons[0]->renderString(projection, "RESUME");
		buttons[1]->renderString(projection, "MAIN MENU");
		buttons[2]->renderString(projection, "QUIT");
	}
	else {
		drawTutorialText(projection);
	}
}

void Background::drawTutorialText(const mat3& projection) {
	jump->renderString(projection, "W/I: Jump");
	left->renderString(projection, "A/J: Move left");
	right->renderString(projection, "D/L: Move right");
	crouch->renderString(projection, "S/K: Crouch");
	pause->renderString(projection, "Esc: Pause");
	reset->renderString(projection, "F5: Reset");
	ability1->renderString(projection, "V/NUM2: Special Ability");
	ability2->renderString(projection, "B/NUM3: Special Ability");
	shield->renderString(projection, "L Shift/R Shift: Shield");
	punch->renderString(projection, "C/NUM1: Punch");
	pauseMusic->renderString(projection, "Insert: Pause/Resume song");
	randomSong->renderString(projection, "Home: Random song");
	changeSong->renderString(projection, "Delete/End: Prev/Next song");
	changeVolume->renderString(projection, "Page Up/Down: Inc./Dec. volume");
}

void Background::addNameplate(TextRenderer* td, std::string name) {
	nameplates[td] = name;
}
void Background::drawNameplates(const mat3& projection) {
	std::map<TextRenderer*, std::string>::iterator it = nameplates.begin();
	while (it != nameplates.end())
	{
		it->first->renderString(projection, it->second);
		it++;
	}
}

void Background::init_buttons()
{
	TextRenderer* resume = new TextRenderer(mainFont, 45);
	TextRenderer* mainMenu = new TextRenderer(mainFont, 45);
	TextRenderer* quit = new TextRenderer(mainFont, 45);

	resume->setColor(selectedColor);
	mainMenu->setColor(defaultColor);
	quit->setColor(defaultColor);

	int width = resume->get_width_of_string("RESUME");
	resume->setPosition({ screen.x / 2.f - width / 2.f, screen.y / 2.f-225.f });
	width = mainMenu->get_width_of_string("MAIN-MENU");
	mainMenu->setPosition({ screen.x / 2.f - width / 2.f, (screen.y/2.f) -175.f});
	width = quit->get_width_of_string("QUIT");
	quit->setPosition({ screen.x / 2.f - width / 2.f, (screen.y / 2.f) -125.f});
	buttons.emplace_back(resume);
	buttons.emplace_back(mainMenu);
	buttons.emplace_back(quit);

	reset_selection();
}

void Background::set_game_over(bool go, std::string wn) {
	m_is_game_over = go;
	winnerName = wn;
}


PauseMenuOption Background::get_selected()
{
	switch (selectedButtonIndex) {
	case 0:
		if (m_is_game_over) { return RESTART; }
		else { return RESUME; }
	case 1:
		return MAINMENU;
	case 2:
		return QUIT;
	}
}