// Header
#include "world.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>

// Same as static in c, local to compilation unit
namespace
{
const size_t MAX_FIGHTERS = 4;
//NOTE: Since we do not currently have a menu, these constants determine the amount of
//fighters that will be spawned in at the beginning of a game
const int MAX_PLAYERS = 2;
const int MAX_AI = 2;

namespace
{
void glfw_err_cb(int error, const char *desc)
{
	fprintf(stderr, "%d: %s", error, desc);
}
} // namespace
} // namespace

//set up player values
//give p1 id =1, p2 id =2 
World::World() : m_player1(1), m_player2(2)
{
	// Seeding rng with random device
	m_rng = std::default_random_engine(std::random_device()());
}

World::~World()
{
}

// World initialization
bool World::init(vec2 screen, GameMode mode)
{
	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization
	// Core Opengl 3.
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);
	m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "VEGGIE VENGEANCE", nullptr, nullptr);
	if (m_window == nullptr)
		return false;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	gl3w_init();

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(m_window, this);
	auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3) { ((World *)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) { ((World *)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1); };
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);

	// Create a frame buffer
	m_frame_buffer = 0;
	glGenFramebuffers(1, &m_frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// Initialize the screen texture
	m_screen_tex.create_from_screen(m_window);

	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	m_background_music = Mix_LoadMUS(audio_path("Abandoned Hopes.wav"));
	m_grunt_audio.emplace_back(Mix_LoadWAV(audio_path("grunt0.wav")));
	m_grunt_audio.emplace_back(Mix_LoadWAV(audio_path("grunt1.wav")));
	m_grunt_audio.emplace_back(Mix_LoadWAV(audio_path("grunt2.wav")));
	m_grunt_audio.emplace_back(Mix_LoadWAV(audio_path("grunt3.wav")));


	if (m_background_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
				audio_path("Abandoned Hopes.wav"));
		return false;
	}

	// Playing background music undefinitely
	Mix_PlayMusic(m_background_music, -1);

	fprintf(stderr, "Loaded music\n");

	m_screen = screen; // to pass on screen size to renderables
	bool initSuccess = set_mode(mode);

	return m_water.init() && initSuccess;
}

// Releases all the associated resources
void World::destroy()
{
	glDeleteFramebuffers(1, &m_frame_buffer);

	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);

	Mix_CloseAudio();

	if (m_player1.get_in_play())
	{
		m_player1.destroy();
	}
	if (m_player2.get_in_play())
	{
		m_player2.destroy();
	}
	for (auto &ai : m_ais)
		ai.destroy();
	m_ais.clear();
	m_fighters.clear();
	m_bg.destroy();
	glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = {(float)w, (float)h};

	// Updating all entities, making the entities
	// faster based on current

	if (m_paused) {
		return true;
	}
	if (!m_paused) {
		//mark alive players + ai as not having a collision applied before collision check
		if (m_player1.get_in_play() && m_player1.get_alive()) {
			m_player1.set_hurt(false);
		}
		if (m_player2.get_in_play() && m_player2.get_alive()) {
			m_player2.set_hurt(false);
		}
		for (int i = 0; i < m_ais.size(); i++) {
			if (m_ais[i].get_alive()) {
				m_ais[i].set_hurt(false);
			}
		}

		//damage effect collision loop
		for (int i = 0; i < m_damageEffects.size(); i++) {
			if (m_player1.get_in_play()) {
				BoundingBox* b1 = new BoundingBox(m_player1.get_position().x, m_player1.get_position().y, m_player1.get_bounding_box().x, m_player1.get_bounding_box().y);
				if (m_damageEffects[i].id != m_player1.get_id() && check_collision(m_damageEffects[i].bounding_box, *b1)) {
					//incur damage
					m_player1.decrease_health(m_damageEffects[i].damage);
					m_player1.set_hurt(true);
				}
				delete b1;
			}
			if (m_player2.get_in_play()) {
				BoundingBox* b2 = new BoundingBox(m_player2.get_position().x, m_player2.get_position().y, m_player2.get_bounding_box().x, m_player2.get_bounding_box().y);
				if (m_damageEffects[i].id != m_player2.get_id() && check_collision(m_damageEffects[i].bounding_box, *b2)) {
					//incur damage
					m_player2.decrease_health(m_damageEffects[i].damage);
					m_player2.set_hurt(true);
				}
				delete b2;
			}
			for (int j = 0; j < m_ais.size(); j++) {
				BoundingBox* b3 = new BoundingBox(m_ais[j].get_position().x, m_ais[j].get_position().y, m_ais[j].get_bounding_box().x, m_ais[j].get_bounding_box().y);
				if (m_damageEffects[i].id != m_ais[j].get_id() && check_collision(m_damageEffects[i].bounding_box, *b3)) {
					//incur damage
					m_ais[j].decrease_health(m_damageEffects[i].damage);
					m_ais[j].set_hurt(true);
				}
				delete b3;
			}
		}

		//damage effect removal loop
		for (int i = 0; i < m_damageEffects.size(); i++) {
			if (m_damageEffects[i].delete_when == AFTER_UPDATE ||
				(m_damageEffects[i].delete_when == AFTER_HIT && m_damageEffects[i].hit_fighter)) {
				//remove from list
				m_damageEffects.erase(m_damageEffects.begin() + i);
				i--;
			}
		}

		
		//update players + ai
		DamageEffect * d = NULL;
		if (m_player1.get_in_play())
		{
			d = m_player1.update(elapsed_ms);
			if (d != NULL) {
				m_damageEffects.push_back(*d);
			}
		}
		if (m_player2.get_in_play())
		{
			d = m_player2.update(elapsed_ms);
			if (d != NULL) {
				m_damageEffects.push_back(*d);
			}
		}

		if (m_player1.get_in_play())
		{
			for (auto &ai : m_ais) {
				d = ai.update(elapsed_ms * 0.5, m_player1.get_position());
				if (d != NULL) {
					m_damageEffects.push_back(*d);
				}
			}
		}
	}
	

	return true;
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void World::draw()
{
	// Clearing error buffer
	gl_flush_errors();

	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);

	
	if (m_mode == DEV || m_mode == PVP)
	{
		m_bg.setPlayerInfo(m_player1.get_lives(), m_player1.get_health(), m_player2.get_lives(), m_player2.get_health());
	}
	else if (m_mode == TUTORIAL || m_mode == PVC)
	{
		AI ai = m_ais.front();
		m_bg.setPlayerInfo(m_player1.get_lives(), m_player1.get_health(), ai.get_lives(), ai.get_health());
	}

	/////////////////////////////////////
	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = {1.f, 1.f, 1.f};
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Fake projection matrix, scales with respect to window coordinates
	// PS: 1.f / w in [1][1] is correct.. do you know why ? (:
	float left = 0.f;		 // *-0.5;
	float top = 0.f;		 // (float)h * -0.5;
	float right = (float)w;  // *0.5;
	float bottom = (float)h; // *0.5;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	mat3 projection_2D{{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};

	// Drawing entities
	if (m_mode == MENU) { 
		m_menu.draw(projection_2D); 
		for (auto &fighter : m_ais)
			fighter.draw(projection_2D);
	} else {
		m_bg.draw(projection_2D);

		if (m_player1.get_in_play())
		{
			m_player1.draw(projection_2D);
			m_player1.drawProjectile(projection_2D);
		}
		if (m_player2.get_in_play())
		{
			m_player2.draw(projection_2D);
			m_player2.drawProjectile(projection_2D);
		}
		for (auto &fighter : m_ais)
			fighter.draw(projection_2D);

	}
	/////////////////////
	// Truly render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(0, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);

	////////////////////////////


	m_water.draw(projection_2D);
	//text->renderString(mat3{}, "Health: 100");
	//text->renderString(projection_2D, "Health: 100");

	//////////////////
	// Presenting
	glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool World::is_over() const
{
	return glfwWindowShouldClose(m_window);
}

// Creates a ai and if successful, adds it to the list of ai
bool World::spawn_ai(AIType type)
{
	//intialize ai with next ID and provided type
	AI ai(idCounter, type);
	if (ai.init(3, "AI"))
	{
		//assure the next ID given is unique
		idCounter++;
		m_ais.emplace_back(ai);
		m_fighters.emplace_back(ai);
		return true;
	}
	fprintf(stderr, "Failed to spawn fighter");
	return false;
}

// On key callback
void World::on_key(GLFWwindow *, int key, int, int action, int mod)
{
	////////////// TEST MODES
	if (action == GLFW_RELEASE && key == GLFW_KEY_1) // TEST
	{
		set_mode(DEV);
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_2) // TEST
	{
		set_mode(PVC);
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_3) // TEST
	{
		set_mode(PVP);
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_4) // TEST
	{
		m_player1.set_in_play(false);
		m_player2.set_in_play(false);
		m_ais.clear();
		set_mode(TUTORIAL);
	}
	//////////////////////

	// MAIN MENU CONTROLS
	if (m_mode == MENU)
	{
		if (action == GLFW_RELEASE && (key == GLFW_KEY_W || key == GLFW_KEY_UP))
		{
			m_menu.change_selection(false);
		}
		if (action == GLFW_RELEASE && (key == GLFW_KEY_S || key == GLFW_KEY_DOWN))
		{
			m_menu.change_selection(true);
		}
		if (action == GLFW_RELEASE && (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE)) // TODO UX okay?
		{
			reset();
			//m_ais.clear();
			set_mode(m_menu.get_selected());
		}

	}
	else {
		// Handle player movement here
		if (m_player1.get_in_play() && !m_paused && m_player1.get_alive())
		{
			if (action == GLFW_PRESS && key == GLFW_KEY_D)
				m_player1.set_movement(MOVING_FORWARD);
			if (action == GLFW_PRESS && key == GLFW_KEY_A)
				m_player1.set_movement(MOVING_BACKWARD);
			if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_W)
				m_player1.set_movement(START_JUMPING);
			if (action == GLFW_PRESS && key == GLFW_KEY_S)
				m_player1.set_movement(CROUCHING);
			if (action == GLFW_PRESS && key == GLFW_KEY_E)
				m_player1.set_movement(PUNCHING);
			else if (action == GLFW_REPEAT && key == GLFW_KEY_E)
				m_player1.set_movement(HOLDING_POWER_PUNCH);
			if (action == GLFW_RELEASE && key == GLFW_KEY_E && m_player1.is_holding_power_punch())
					m_player1.set_movement(POWER_PUNCHING);
			if (action == GLFW_PRESS && key == GLFW_KEY_Q)
				m_player1.set_movement(SHOOTING);
			if (action == GLFW_RELEASE && key == GLFW_KEY_D)
				m_player1.set_movement(STOP_MOVING_FORWARD);
			if (action == GLFW_RELEASE && key == GLFW_KEY_A)
				m_player1.set_movement(STOP_MOVING_BACKWARD);
			if (action == GLFW_RELEASE && key == GLFW_KEY_S && (m_player1.get_crouch_state() == CROUCH_PRESSED || m_player1.get_crouch_state() == IS_CROUCHING))
				m_player1.set_movement(RELEASE_CROUCH);
			if (action == GLFW_RELEASE && key == GLFW_KEY_E && !m_player1.is_holding_power_punch()) {
				m_player1.set_movement(STOP_PUNCHING);
				play_grunt_audio();
			}
			if (action == GLFW_RELEASE && key == GLFW_KEY_Q)
				m_player1.set_movement(STOP_SHOOTING);
		}

		if (m_player2.get_in_play() && !m_paused && m_player2.get_alive())
		{
			if (action == GLFW_PRESS && key == GLFW_KEY_L)
				m_player2.set_movement(MOVING_FORWARD);
			if (action == GLFW_PRESS && key == GLFW_KEY_J)
				m_player2.set_movement(MOVING_BACKWARD);
			if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_I)
				m_player2.set_movement(START_JUMPING);
			if (action == GLFW_PRESS && key == GLFW_KEY_K)
				m_player2.set_movement(CROUCHING);
			if (action == GLFW_PRESS && key == GLFW_KEY_O)
				m_player2.set_movement(PUNCHING);
			if (action == GLFW_PRESS && key == GLFW_KEY_U)
				m_player2.set_movement(SHOOTING);
			if (action == GLFW_REPEAT && key == GLFW_KEY_O)
				m_player2.set_movement(HOLDING_POWER_PUNCH);
			if (action == GLFW_RELEASE && key == GLFW_KEY_O && m_player2.is_holding_power_punch())
				m_player2.set_movement(POWER_PUNCHING);
			if (action == GLFW_RELEASE && key == GLFW_KEY_L)
				m_player2.set_movement(STOP_MOVING_FORWARD);
			if (action == GLFW_RELEASE && key == GLFW_KEY_J)
				m_player2.set_movement(STOP_MOVING_BACKWARD);
			if (action == GLFW_RELEASE && key == GLFW_KEY_K && (m_player2.get_crouch_state() == CROUCH_PRESSED || m_player2.get_crouch_state() == IS_CROUCHING))
				m_player2.set_movement(RELEASE_CROUCH);
			if (action == GLFW_RELEASE && key == GLFW_KEY_O) {
				m_player2.set_movement(STOP_PUNCHING);
				play_grunt_audio();
			}
			if (action == GLFW_RELEASE && key == GLFW_KEY_U)
				m_player2.set_movement(STOP_SHOOTING);
		}

		if (m_paused) {
			m_player1.set_movement(STOP_MOVING_FORWARD);
			m_player1.set_movement(STOP_MOVING_BACKWARD);
			m_player1.set_movement(STOP_PUNCHING);
			m_player1.set_movement(STOP_SHOOTING);
			m_player2.set_movement(STOP_MOVING_FORWARD);
			m_player2.set_movement(STOP_MOVING_BACKWARD);
			m_player2.set_movement(STOP_PUNCHING);
			m_player2.set_movement(STOP_SHOOTING);
		}
		
		if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && !m_paused)
		{
			m_water.set_is_wavy(true); // STUB ENVIRONMENT EFFECT
		}
		if (action == GLFW_RELEASE && key == GLFW_KEY_ENTER && !m_paused)
		{
			m_water.set_is_wavy(false); // STUB ENVIRONMENT EFFECT
		}
		

		// Pausing and resuming game
		if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
			if (m_player1.get_in_play() && m_player1.get_crouch_state() == IS_CROUCHING) {
				m_player1.set_crouch_state(CROUCH_RELEASED);
			}

			if (m_player2.get_in_play() && m_player2.get_crouch_state() == IS_CROUCHING) {
				m_player2.set_crouch_state(CROUCH_RELEASED);
			}
			//m_paused = !m_paused;
			set_paused(!m_paused);
		}

		// Resetting game
		if (action == GLFW_RELEASE && key == GLFW_KEY_B)
		{
			reset();
		}
	}

	// Music control
	if (action == GLFW_PRESS && key == GLFW_KEY_PAGE_UP)
		Mix_VolumeMusic(Mix_VolumeMusic(-1) + 20);
	if (action == GLFW_PRESS && key == GLFW_KEY_PAGE_DOWN)
		Mix_VolumeMusic(Mix_VolumeMusic(-1) - 20);
	if (action == GLFW_PRESS && key == GLFW_KEY_END && !Mix_PausedMusic())
		Mix_PauseMusic();
	if (action == GLFW_PRESS && key == GLFW_KEY_HOME && Mix_PausedMusic())
		Mix_ResumeMusic();
}

void World::reset()
{
	m_damageEffects.clear();
	switch (m_mode) {
	case DEV:
		m_player1.reset(1);
		m_player2.reset(2);

		for (AI ai : m_ais)
		{
			ai.reset(3);
		}
		break;
	case PVP:
		m_player1.reset(1);
		m_player2.reset(2);
		break;
	case PVC:
		m_player1.reset(1);

		for (AI ai : m_ais)
		{
			ai.reset(3);
		}
		break;
	case TUTORIAL:
		m_player1.reset(1);

		for (AI ai : m_ais)
		{
			ai.reset(3);
		}
		break;
	case MENU:	// TESTING TRANSITIONS, REDUNDANT NOW
		m_player1.set_in_play(false);
		m_ais.clear();
		//set_mode(DEV);
		//set_mode(PVC);
		break;
	}
}

bool World::set_mode(GameMode mode) {
	m_player1.set_in_play(false);
	m_player2.set_in_play(false);
	m_ais.clear();
	m_fighters.clear();
	m_bg.clearNameplates();
	
	m_mode = mode;
	bool initSuccess = true;
	std::cout << "Mode set to: " << ModeMap[mode] << std::endl;

	switch (mode) {
	case MENU:
		m_player1.set_in_play(true); // needed to make AI respond
		spawn_ai(RANDOM);
		m_ais[0].set_position({ 250.f, m_screen.y*.85f}); // TODO
		initSuccess = initSuccess && m_menu.init(m_screen);
		break;
	case DEV:
		if (MAX_PLAYERS >= 1)
		{
			m_player1.set_in_play(true);
		}
		if (MAX_PLAYERS >= 2)
		{
			m_player2.set_in_play(true);
		}

		if (m_player1.get_in_play())
		{
			initSuccess = initSuccess && m_player1.init(1, "Poe Tatum");
			m_fighters.emplace_back(m_player1);
		}

		if (m_player2.get_in_play())
		{
			initSuccess = initSuccess && m_player2.init(2, "Spud");
			m_fighters.emplace_back(m_player2);
		}

		for (int i = 0; i < MAX_AI; i++)
		{
			AIType type = AVOID;
			if (i % 2 == 0)
			{
				type = CHASE;
			}
			initSuccess = initSuccess && spawn_ai(type);
		}
		initSuccess = initSuccess && m_bg.init(m_screen, mode);
		break;
	case PVP: // 2 player
		m_player1.set_in_play(true);
		m_player2.set_in_play(true);
		initSuccess = initSuccess && m_player1.init(1, "Poe Tatum") && m_player2.init(2, "Spud") && m_bg.init(m_screen, mode);
		m_fighters.emplace_back(m_player1);
		m_fighters.emplace_back(m_player2);
		break;
	case PVC: // single player
		m_player1.set_in_play(true);
		initSuccess = initSuccess && m_player1.init(1, "Spud") && spawn_ai(AVOID) && m_bg.init(m_screen, mode);
		m_fighters.emplace_back(m_player1);
		break;
	case TUTORIAL:
		m_player1.set_in_play(true);
		initSuccess = initSuccess && m_player1.init(1, "Baby Tater") && spawn_ai(AVOID) && m_bg.init(m_screen, mode);
		m_fighters.emplace_back(m_player1);
		break;
	}

	if (mode != MENU)
		for (Fighter &f : m_fighters)
			m_bg.addNameplate(f.get_nameplate(), f.get_name());

	return initSuccess;
}

void World::on_mouse_move(GLFWwindow *window, double xpos, double ypos)
{
}


bool World::check_collision(BoundingBox b1, BoundingBox b2) {
	if (b1.xpos < b2.xpos + b2.width &&
		b1.xpos + b1.width > b2.xpos &&
		b1.ypos < b2.ypos + b2.height &&
		b1.ypos + b1.height > b2.ypos) {
		return true;
	}
	else {
		return false;
	}
}

void World::set_paused(bool isPaused) {
	m_paused = isPaused;
	m_bg.setPaused(isPaused);
}

void World::play_grunt_audio() {
	std::random_device rd; // non-deterministic generator
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, 3); //ADD FOR MORE ACTIONS
	int	randNum = dist(gen);
	Mix_PlayChannel(-1, m_grunt_audio[randNum], 0);
}
