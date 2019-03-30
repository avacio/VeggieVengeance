// Header
#include "common.hpp"
#include "world.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>

#define HEAT_WAVE_RATE 80
#define HEAT_WAVE_LENGTH 1

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

	m_bgms.emplace_back(Mix_LoadMUS(audio_path("/bgm/Abandoned Hopes.wav")));
	m_bgms.emplace_back(Mix_LoadMUS(audio_path("/bgm/Crimson Drive.wav")));
	m_bgms.emplace_back(Mix_LoadMUS(audio_path("/bgm/Fields of Ice.wav")));
	m_bgms.emplace_back(Mix_LoadMUS(audio_path("/bgm/Mecha Collection.wav")));
	m_bgms.emplace_back(Mix_LoadMUS(audio_path("/bgm/Sunstrider.wav")));
	m_bgms.emplace_back(Mix_LoadMUS(audio_path("/bgm/The Monarch's Rule.wav")));
	m_bgms.emplace_back(Mix_LoadMUS(audio_path("/bgm/The Way You Love.wav")));
	m_bgms.emplace_back(Mix_LoadMUS(audio_path("/bgm/Zero Respect.wav")));
	m_grunt_audio.emplace_back(Mix_LoadWAV(audio_path("grunt0.wav")));
	m_grunt_audio.emplace_back(Mix_LoadWAV(audio_path("grunt1.wav")));
	m_grunt_audio.emplace_back(Mix_LoadWAV(audio_path("grunt2.wav")));
	m_grunt_audio.emplace_back(Mix_LoadWAV(audio_path("grunt3.wav")));
	m_background_track = get_random_number(m_bgms.size() - 1);
	m_charging_up_audio = Mix_LoadWAV(audio_path("charging_up.wav"));
	m_charging_up_audio->volume *= 0.3;
	m_charged_punch_audio = Mix_LoadWAV(audio_path("charged_punch.wav"));
	m_broccoli_uppercut_audio = Mix_LoadWAV(audio_path("uppercut.wav"));

	if (m_bgms[m_background_track] == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
				audio_path("/bgm/Sunstrider.wav"));
		return false;
	}

	// Playing background music undefinitely
	Mix_PlayMusic(m_bgms[m_background_track], -1);
	fprintf(stderr, "Loaded music\n");
	
	load_fighter_templates();

	m_screen = screen; // to pass on screen size to renderables

	bool initSuccess = load_all_sprites_from_file() && set_mode(mode);
	spawn_platform(0, 650, 1200, 10);
	spawn_platform(-2, 551, 100, 10);
	spawn_platform(1117, 551, 115, 10);
	spawn_platform(137, 440, 220, 10);
	spawn_platform(847, 440, 220, 10);
	spawn_platform(375, 308, 453, 10);

	//return m_menu.init(m_screen, fighterMap) && m_water.init() && initSuccess;
	return m_water.init() && initSuccess;
}

// Releases all the associated resources
void World::destroy()
{
	glDeleteFramebuffers(1, &m_frame_buffer);
	
	if (m_bgms.size() > 0) {
		for (auto &music : m_bgms)
			Mix_FreeMusic(music);
	}

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

	for (auto &platform : m_platforms) {
		platform.destroy();
	}
	m_platforms.clear();

	if (m_bg.m_initialized) {
		m_bg.destroy();
	}
	if (m_menu.m_initialized) {
		m_menu.destroy();
	}
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

	//if (m_paused) {
	if (m_paused || m_game_over) { // TODO || m_mode == MENU || m_mode == CHARSELECT
		return true;
	}

	if (!m_game_over && is_game_over()) {
		m_game_over = is_game_over();
		//m_water.set_game_over(m_winner_name);
		m_bg.set_game_over(true, m_winner_name);
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

		attack_collision();
		//damage effect removal loop
		attack_deletion();
		
		//update players + ai
		Attack * attack = NULL;
		if (m_player1.get_in_play())
		{
			attack = m_player1.update(elapsed_ms, m_platforms);
			if (attack != NULL) {
				attack->init();
				m_attacks.push_back(attack);
			}
		}
		if (m_player2.get_in_play())
		{
			attack = m_player2.update(elapsed_ms, m_platforms);
			if (attack != NULL) {
				attack->init();
				m_attacks.push_back(attack);
			}
		}

		if (m_player1.get_in_play())
		{
			for (auto &ai : m_ais) {
				attack = ai.update(elapsed_ms, m_platforms, m_player1.get_position());
				if (attack != NULL) {
					attack->init();
					m_attacks.push_back(attack);
				}
			}
		}
		if (m_mode != MENU && m_mode != CHARSELECT) {
			if (get_heat_wave_time() > HEAT_WAVE_LENGTH) {
				set_heat_wave(false);
			} else if (m_heat_wave_on) {
				apply_stage_fx_dmg();
			} else if ((int)glfwGetTime() % HEAT_WAVE_RATE == 0) {
				set_heat_wave(true);
			}
		}
	}
	attack_update(elapsed_ms);
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
		m_bg.setPlayerInfo(m_player1.get_lives(), m_player1.get_health(), m_player1.get_block_tank(), m_player2.get_lives(), m_player2.get_health(), m_player2.get_block_tank());
	}
	else if (m_mode == TUTORIAL || m_mode == PVC)
	{
		AI ai = m_ais.front();
		m_bg.setPlayerInfo(m_player1.get_lives(), m_player1.get_health(), m_player1.get_block_tank(), ai.get_lives(), ai.get_health(), ai.get_block_tank());
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
	if (m_mode == MENU || m_mode == CHARSELECT) { 
		m_menu.draw(projection_2D); 
		for (auto &fighter : m_ais)
			fighter.draw(projection_2D);
	} else {
		m_bg.draw(projection_2D);

		if (m_player1.get_in_play())
		{
			m_player1.draw(projection_2D);
		}
		if (m_player2.get_in_play())
		{
			m_player2.draw(projection_2D);
		}
		for (auto &fighter : m_ais)
			fighter.draw(projection_2D);

		for (auto &attack : m_attacks)
			attack->draw(projection_2D);

		for (auto &platform : m_platforms)
			platform.draw(projection_2D);

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
	//////////////////
	// Presenting
	glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool World::is_over() const
{
	return glfwWindowShouldClose(m_window) || m_over;
}

// Creates a ai and if successful, adds it to the list of ai
bool World::spawn_ai(AIType type)
{
	//intialize ai with next ID and provided type
	AI ai(idCounter, type);
	if (ai.init(3, "AI", BROCCOLI)) // TODO: RANDOMIZE BUT NOT INCLUDING BLANK PLACEHOLDER 
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

bool World::spawn_ai(AIType type, FighterCharacter fc)
{
	AI ai(idCounter, type);
	if (ai.init(3, "AI", fc))
	{
		idCounter++;
		m_ais.emplace_back(ai);
		m_fighters.emplace_back(ai);
		return true;
	}
	fprintf(stderr, "Failed to spawn fighter");
	return false;
}
// Creates a platform and if successful, adds it to the list of platform
bool World::spawn_platform(float xpos, float ypos, float width, float height)
{
	Platform platform(xpos, ypos, width, height);
	if (platform.init())
	{
		m_platforms.emplace_back(platform);
		return true;
	}
	fprintf(stderr, "Failed to spawn platform");
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
	//if ((m_mode == MENU || m_mode == CHARSELECT) && m_menu.m_initialized)
	if (m_mode == MENU || m_mode == CHARSELECT)
	{
		if (action == GLFW_RELEASE && (key == GLFW_KEY_W || key == GLFW_KEY_UP)) {
			m_menu.change_selection(false);
			if (m_mode == CHARSELECT && m_menu.get_selected_char() != BLANK) {
				spawn_char_select_AI(m_menu.get_selected_char());
			}
		}
		if (action == GLFW_RELEASE && (key == GLFW_KEY_S || key == GLFW_KEY_DOWN)) {
			m_menu.change_selection(true);
			if (m_mode == CHARSELECT && m_menu.get_selected_char() != BLANK) {
				spawn_char_select_AI(m_menu.get_selected_char());
			}
		}
		if (action == GLFW_RELEASE && (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE)) // TODO UX okay?
		{
			if (m_mode == MENU) {
				m_menu.set_selected_mode();
				set_mode(CHARSELECT);
			} else if (m_mode == CHARSELECT) {
				FighterCharacter result = m_menu.get_selected_char();
				GameMode selMode = m_menu.get_selected_mode();
				if (result == BLANK) {
					set_mode(MENU);
				} else if (selMode == PVP) {
					if (!m_menu.is_player_1_chosen) {
						selectedP1 = result;
						m_menu.is_player_1_chosen = true;
					} else {
						selectedP2 = result;
						m_menu.is_player_1_chosen = false;
						set_mode(selMode);
					}
				} else {
					selectedP1 = m_menu.get_selected_char();
					set_mode(m_menu.get_selected_mode());
				}
			}
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
			if (action == GLFW_PRESS && key == GLFW_KEY_W && m_player1.get_fc() == BROCCOLI && m_player1.broccoli_get_jump_left() == 1)
				m_player1.broccoli_set_double_jump();
			if (action == GLFW_PRESS && key == GLFW_KEY_S)
				m_player1.set_movement(CROUCHING);
			if (action == GLFW_PRESS && key == GLFW_KEY_C) {
				m_player1.set_movement(PUNCHING);
				play_grunt_audio();
			}
			if (action == GLFW_PRESS && key == GLFW_KEY_V) {
				m_player1.set_movement(ABILITY_1);
				if (m_player1.get_fc() == BROCCOLI && !m_player1.broccoli_is_uppercut_on_cooldown())
					Mix_PlayChannel(-1, m_broccoli_uppercut_audio, 0);
			}
			if (action == GLFW_PRESS && key == GLFW_KEY_B)
				m_player1.set_movement(ABILITY_2);
			if (action == GLFW_REPEAT && key == GLFW_KEY_B)
				m_player1.set_movement(HOLDING_ABILITY_2);
			if (action == GLFW_RELEASE && key == GLFW_KEY_B && m_player1.potato_is_holding_wedges())
				m_player1.set_movement(CHARGED_ABILITY_2);
			if (action == GLFW_REPEAT && key == GLFW_KEY_C) {
				m_player1.set_movement(HOLDING_POWER_PUNCH);
				Mix_PlayChannel(1, m_charging_up_audio, 0);
			}	
			if (action == GLFW_RELEASE && key == GLFW_KEY_C && m_player1.is_holding_power_punch()) {
				m_player1.set_movement(POWER_PUNCHING);
				Mix_PlayChannel(1, m_charged_punch_audio, 0);
			}
			if (action == GLFW_PRESS && key == GLFW_KEY_LEFT_SHIFT)
				m_player1.set_movement(BLOCKING);
			if (action == GLFW_RELEASE && key == GLFW_KEY_D)
				m_player1.set_movement(STOP_MOVING_FORWARD);
			if (action == GLFW_RELEASE && key == GLFW_KEY_A)
				m_player1.set_movement(STOP_MOVING_BACKWARD);
			if (action == GLFW_RELEASE && key == GLFW_KEY_S && (m_player1.get_crouch_state() == CROUCH_PRESSED || m_player1.get_crouch_state() == IS_CROUCHING))
				m_player1.set_movement(RELEASE_CROUCH);
			if (action == GLFW_RELEASE && key == GLFW_KEY_C && !m_player1.is_holding_power_punch())
				m_player1.set_movement(STOP_PUNCHING);
			if (action == GLFW_RELEASE && (key == GLFW_KEY_B || key == GLFW_KEY_V) && !m_player1.potato_is_holding_wedges())
				m_player1.set_movement(STOP_ABILITIES);
			if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT_SHIFT)
				m_player1.set_movement(STOP_BLOCKING);
		}

		if (m_player2.get_in_play() && !m_paused && m_player2.get_alive())
		{
			if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT)
				m_player2.set_movement(MOVING_FORWARD);
			if (action == GLFW_PRESS && key == GLFW_KEY_LEFT)
				m_player2.set_movement(MOVING_BACKWARD);
			if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_UP)
				m_player2.set_movement(START_JUMPING);
			if (action == GLFW_PRESS && key == GLFW_KEY_UP && m_player2.get_fc() == BROCCOLI && m_player2.broccoli_get_jump_left() == 1)
				m_player2.broccoli_set_double_jump();
			if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
				m_player2.set_movement(CROUCHING);
			if (action == GLFW_PRESS && (key == GLFW_KEY_KP_1 || key == GLFW_KEY_SLASH)) {
				m_player2.set_movement(PUNCHING);
				play_grunt_audio();
			}
			if (action == GLFW_PRESS && (key == GLFW_KEY_KP_2 || key == GLFW_KEY_PERIOD)) {
				m_player2.set_movement(ABILITY_1);
				if (m_player2.get_fc() == BROCCOLI && !m_player2.broccoli_is_uppercut_on_cooldown())
					Mix_PlayChannel(-1, m_broccoli_uppercut_audio, 0);
			}
			if (action == GLFW_PRESS && (key == GLFW_KEY_KP_3 || key == GLFW_KEY_COMMA))
				m_player2.set_movement(ABILITY_2);
			if (action == GLFW_REPEAT && (key == GLFW_KEY_KP_3 || key == GLFW_KEY_COMMA))
				m_player2.set_movement(HOLDING_ABILITY_2);
			if (action == GLFW_RELEASE && (key == GLFW_KEY_KP_3 || key == GLFW_KEY_COMMA) && m_player2.potato_is_holding_wedges())
				m_player2.set_movement(CHARGED_ABILITY_2);
			if (action == GLFW_REPEAT && (key == GLFW_KEY_KP_1 || key == GLFW_KEY_SLASH)) {
				m_player2.set_movement(HOLDING_POWER_PUNCH);
				Mix_PlayChannel(2, m_charging_up_audio, 0);
			}
			if (action == GLFW_RELEASE && (key == GLFW_KEY_KP_1 || key == GLFW_KEY_SLASH) && m_player2.is_holding_power_punch()) {
				m_player2.set_movement(POWER_PUNCHING);			
				Mix_PlayChannel(2, m_charged_punch_audio, 0);
			}
			if (action == GLFW_RELEASE && key == GLFW_KEY_RIGHT)
				m_player2.set_movement(STOP_MOVING_FORWARD);
			if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT_SHIFT)
				m_player2.set_movement(BLOCKING);
			if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT)
				m_player2.set_movement(STOP_MOVING_BACKWARD);
			if (action == GLFW_RELEASE && key == GLFW_KEY_DOWN && (m_player2.get_crouch_state() == CROUCH_PRESSED || m_player2.get_crouch_state() == IS_CROUCHING))
				m_player2.set_movement(RELEASE_CROUCH);
			if (action == GLFW_RELEASE && (key == GLFW_KEY_KP_1 || key == GLFW_KEY_SLASH) && !m_player2.is_holding_power_punch())
				m_player2.set_movement(STOP_PUNCHING);
			if (action == GLFW_RELEASE && (key == GLFW_KEY_KP_2 || GLFW_KEY_KP_3 || key == GLFW_KEY_PERIOD || key == GLFW_KEY_COMMA) && !m_player2.potato_is_holding_wedges())
				m_player2.set_movement(STOP_ABILITIES);
			if (action == GLFW_RELEASE && key == GLFW_KEY_RIGHT_SHIFT)
				m_player2.set_movement(STOP_BLOCKING);
		}
		//if (m_paused) {
		if (m_paused || m_game_over) {
			m_player1.set_movement(STOP_MOVING_FORWARD);
			m_player1.set_movement(STOP_MOVING_BACKWARD);
			m_player1.set_movement(STOP_PUNCHING);
			m_player1.set_movement(STOP_ABILITIES);
			m_player1.set_movement(PAUSED);
			m_player2.set_movement(STOP_MOVING_FORWARD);
			m_player2.set_movement(STOP_MOVING_BACKWARD);
			m_player2.set_movement(STOP_PUNCHING);
			m_player2.set_movement(STOP_ABILITIES);
			m_player2.set_movement(PAUSED);

			for (auto &fighter : m_ais)
				fighter.set_movement(PAUSED);


			if (action == GLFW_RELEASE && (key == GLFW_KEY_W || key == GLFW_KEY_UP))
			{
				m_bg.change_selection(false);
			}
			if (action == GLFW_RELEASE && (key == GLFW_KEY_S || key == GLFW_KEY_DOWN))
			{
				m_bg.change_selection(true);
			}
			if (action == GLFW_RELEASE && (key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE)) // TODO UX okay?
			{
				PauseMenuOption selectedOption = m_bg.get_selected();
				if (selectedOption == RESUME) {
					m_bg.setHelp(false);
					set_paused(!m_paused);
				}
				else if (selectedOption == MAINMENU) {
					set_mode(MENU);
				}
				else if (selectedOption == QUIT) {
					m_over = true;
				}
				else if (selectedOption == RESTART) {
					reset();
				}
			}
		}

		if (!m_paused)
		{
			m_player1.set_movement(UNPAUSED);
			m_player2.set_movement(UNPAUSED);

			for (auto &fighter : m_ais)
				fighter.set_movement(UNPAUSED);
		}

		// Pausing and resuming game
		if (action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_H)) {
			if (m_player1.get_in_play() && m_player1.get_crouch_state() == IS_CROUCHING) {
				m_player1.set_crouch_state(CROUCH_RELEASED);
			}

			if (m_player2.get_in_play() && m_player2.get_crouch_state() == IS_CROUCHING) {
				m_player2.set_crouch_state(CROUCH_RELEASED);
			}
			if (key == GLFW_KEY_H) { m_bg.setHelp(!m_bg.getHelp()); }
			set_paused(!m_paused);
		}

		// Resetting game
		if (action == GLFW_RELEASE && key == GLFW_KEY_F5)
		{
			reset();
		}
	}

	// Music control
	if (action == GLFW_PRESS && key == GLFW_KEY_PAGE_UP)
		Mix_VolumeMusic(Mix_VolumeMusic(-1) + 20);
	if (action == GLFW_PRESS && key == GLFW_KEY_PAGE_DOWN)
		Mix_VolumeMusic(Mix_VolumeMusic(-1) - 20);
	if (action == GLFW_PRESS && key == GLFW_KEY_INSERT && !Mix_PausedMusic()) {
		Mix_PauseMusic();
	} else if (action == GLFW_PRESS && key == GLFW_KEY_INSERT && Mix_PausedMusic()) {
		Mix_ResumeMusic();
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_END) {
		// next song
		m_background_track++;
		if (m_background_track >= m_bgms.size() - 1) {
			m_background_track = 0;
		}
		Mix_FadeInMusic(m_bgms[m_background_track], -1, 1000);
	 } else if (action == GLFW_PRESS && key == GLFW_KEY_DELETE) {
		 // previous song
		if (m_background_track == 0) {
			m_background_track = m_bgms.size();
		}
		m_background_track--;
		Mix_FadeInMusic(m_bgms[m_background_track], -1, 1000);
	} else if (action == GLFW_PRESS && key == GLFW_KEY_HOME) {
		// random song
		m_background_track = get_random_number(m_bgms.size() - 1);
		Mix_FadeInMusic(m_bgms[m_background_track], -1, 1000);
	}
}


void World::reset()
{

	m_attacks.clear();
	m_game_over = false;
	m_bg.set_game_over(false, "");

	switch (m_mode) {
	case DEV:
		m_player1.reset();
		m_player2.reset();
		for (auto &ai : m_ais)
		{
			ai.reset();
		}
		break;
	case PVP:
		m_player1.reset();
		m_player2.reset();
		break;
	case PVC:
		m_player1.reset();

		for (auto &ai : m_ais)
		{
			ai.reset();
		}
		break;
	case TUTORIAL:
		m_player1.reset();

		for (auto &ai : m_ais)
		{
			ai.reset();
		}
		break;
	case MENU:
		m_player1.set_in_play(false);
		m_ais.clear();
		break;
	case CHARSELECT:
		break;
	default:
		break;
	}
}

bool World::set_mode(GameMode mode) {
	m_player1.set_in_play(false);
	m_player2.set_in_play(false);
	m_game_over = false;
	m_bg.set_game_over(false, "");

	clear_all_fighters();

	if (m_bg.m_initialized) {
		m_bg.destroy();
	}
	if (m_menu.m_initialized) {
		m_menu.destroy();
	}
	bool initSuccess = true;
	std::cout << "Mode set to: " << ModeMap[mode] << std::endl;

	switch (mode) {
		case MENU:
			m_player1.set_in_play(true); // needed to make AI respond
			spawn_ai(RANDOM, POTATO);
			set_paused(false);
			m_ais[0].set_position({ 250.f, m_screen.y*.85f}); // TODO
			initSuccess = initSuccess && m_menu.init(m_screen) && m_menu.set_mode(MENU);
			break;
		case CHARSELECT:
		{
			m_player1.set_in_play(true);
			spawn_ai(RANDOM, POTATO);
			m_ais[0].set_position({ 250.f, m_screen.y*.85f }); // TODO
			initSuccess = initSuccess && m_menu.init(m_screen) && m_menu.set_mode(CHARSELECT);
			break;
		}
		case DEV: {
			if (MAX_PLAYERS >= 1) { m_player1.set_in_play(true); }
			if (MAX_PLAYERS >= 2) { m_player2.set_in_play(true); }
			if (m_player1.get_in_play()) {
				initSuccess = initSuccess && m_player1.init(1, "Poe Tatum", selectedP1);
				m_fighters.emplace_back(m_player1);
			}
			if (m_player2.get_in_play()) {
				initSuccess = initSuccess && m_player2.init(2, "Spud", selectedP2);
				m_fighters.emplace_back(m_player2);
			}

			for (int i = 0; i < MAX_AI; i++)
			{
				AIType type = AVOID;
				if (i % 2 == 0) { type = CHASE; }
				initSuccess = initSuccess && m_bg.init(m_screen, mode);
				break;
			}
		}
		case PVP: // 2 player
			m_player1.set_in_play(true);
			m_player2.set_in_play(true);
			initSuccess = initSuccess && m_player1.init(1, fighterMap[selectedP1].getFCName(), selectedP1) && m_player2.init(2, fighterMap[selectedP2].getFCName(), selectedP2) && m_bg.init(m_screen, mode);
			m_fighters.emplace_back(m_player1);
			m_fighters.emplace_back(m_player2);
			break;
		case PVC: // single player
			m_player1.set_in_play(true);
			initSuccess = initSuccess && m_player1.init(1, fighterMap[selectedP1].getFCName(), selectedP1) && spawn_ai(AVOID) && m_bg.init(m_screen, mode);
			m_fighters.emplace_back(m_player1);
			break;
		case TUTORIAL:
			m_player1.set_in_play(true);
			initSuccess = initSuccess && m_player1.init(1, fighterMap[selectedP1].getFCName(), selectedP1) && spawn_ai(AVOID) && m_bg.init(m_screen, mode);
			m_fighters.emplace_back(m_player1);
			break;
		default:
			break;
	}

	if (mode != MENU && mode != CHARSELECT) {
		for (Fighter &f : m_fighters)
			m_bg.addNameplate(f.get_nameplate(), f.get_name());
	}

	m_mode = mode;
	return initSuccess;
}

void World::on_mouse_move(GLFWwindow *window, double xpos, double ypos)
{
}


bool World::check_collision_world(BoundingBox b1) {
	// !!! refactor so that this doesn't use magic numbers
	BoundingBox* b3 = new BoundingBox(0, 0, 1200, 800);
	bool collision = b1.check_collision(*b3);
	delete b3;
	return collision;
}

void World::set_paused(bool isPaused) {
	m_paused = isPaused;
	m_bg.setPaused(isPaused);
}

void World::play_grunt_audio() {
	Mix_PlayChannel(-1, m_grunt_audio[get_random_number(3)], 0);
}

void World::spawn_char_select_AI(FighterCharacter fc) {
	clear_all_fighters();
	spawn_ai(RANDOM, fc);
	m_ais[0].set_position({ 250.f, m_screen.y*.85f });
}

void World::clear_all_fighters() {
	for (AI& ai : m_ais) {
		ai.destroy();
	}
	m_ais.clear();

	for (Fighter& fighter : m_fighters) {
		fighter.destroy();
	}
	m_fighters.clear();

	std::map<FighterCharacter, FighterInfo>::iterator it;
	for (it = fighterMap.begin(); it != fighterMap.end(); it++)
	{
		it->second.clearTaken();
	}
}

void World::attack_collision() {
	//damage effect collision loop
	for (int i = 0; i < m_attacks.size(); i++) {
		if (m_player1.get_in_play() && m_player1.get_alive()) {
			BoundingBox* b1 = m_player1.get_bounding_box();
			if (m_attacks[i]->m_fighter_id != m_player1.get_id() && m_player1.is_blocking() == false && m_attacks[i]->m_damageEffect->m_bounding_box.check_collision(*b1)) {
				//incur damage
				m_player1.apply_damage(m_attacks[i]->m_damageEffect);
				m_player1.set_hurt(true);
				m_attacks[i]->m_damageEffect->m_hit_fighter = true;
			}
			delete b1;
		}
		if (m_player2.get_in_play() && m_player2.get_alive()) {
			BoundingBox* b2 = m_player2.get_bounding_box();
			if (m_attacks[i]->m_fighter_id != m_player2.get_id() && m_player2.is_blocking() == false && m_attacks[i]->m_damageEffect->m_bounding_box.check_collision(*b2)) {
				//incur damage
				m_player2.apply_damage(m_attacks[i]->m_damageEffect);
				m_player2.set_hurt(true);
				m_attacks[i]->m_damageEffect->m_hit_fighter = true;
			}
			delete b2;
		}
		for (int j = 0; j < m_ais.size(); j++) {
			if (m_ais[j].get_alive()) {
				BoundingBox* b3 = m_ais[j].get_bounding_box();
				if (m_attacks[i]->m_fighter_id != m_ais[j].get_id() && m_ais[j].is_blocking() == false && m_attacks[i]->m_damageEffect->m_bounding_box.check_collision(*b3)) {
					//incur damage
					m_ais[j].apply_damage(m_attacks[i]->m_damageEffect);
					m_ais[j].set_hurt(true);
					m_attacks[i]->m_damageEffect->m_hit_fighter = true;
				}
				delete b3;
			}
		}
	}
}

void World::attack_deletion() {
	for (int i = 0; i < m_attacks.size(); i++) {
		if (m_attacks[i]->m_damageEffect->m_delete_when == AFTER_UPDATE ||
			(m_attacks[i]->m_damageEffect->m_delete_when == AFTER_HIT && m_attacks[i]->m_damageEffect->m_hit_fighter) ||
			(m_attacks[i]->m_damageEffect->m_delete_when == AFTER_TIME && m_attacks[i]->m_damageEffect->m_time_remain <= 0) ||
			!check_collision_world(m_attacks[i]->m_damageEffect->m_bounding_box)) {
			//remove from list
			delete m_attacks[i];
			m_attacks.erase(m_attacks.begin() + i);
			i--;
		}
	}
}

void World::attack_update(float ms) {
	for (auto &attack : m_attacks)
		attack->update(ms);
}

void World::draw_rectangle() {
	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};
}

bool World::is_game_over() {
	if (m_mode == PVP) {
		if (!m_player1.get_alive() && m_player1.get_lives() == 0) {
			m_winner_name = m_player2.get_name();
			return true;
		}
		else if (!m_player2.get_alive() && m_player2.get_lives() == 0) {
			m_winner_name = m_player1.get_name();
			return true;
		}
		return false;
	}
	if (m_mode == PVC || m_mode == TUTORIAL) {
		bool ais_alive = false;
		for (int i = 0; i < m_ais.size(); i++) {
			if (m_ais[i].get_alive() || m_ais[i].get_lives() > 0) {
				m_winner_name = m_ais[i].get_name();
				ais_alive = true; }
		}
		if (!ais_alive) { m_winner_name = m_player1.get_name(); }
		return ((!m_player1.get_alive() && m_player1.get_lives() == 0) || !ais_alive);
	}
	return false;
}

void World::set_heat_wave(bool on) {
	if (m_heat_wave_on == on) { return; }
	if (on) {
		m_heat_wave_time = glfwGetTime();
		m_water.set_is_wavy(true);
		int dmg = 20;
		//for (int i = 0; i < m_ais.size(); i++) { // fighters does not work for some reason
		//	m_fighters[i].apply_damage(dmg);
		//	m_fighters[i].set_hurt(true);
		//	//std::cout << "HURT fighter: " << i << std::endl;
		//}
	} else {
		m_heat_wave_time = -1.f;
		m_water.set_is_wavy(false);
		for (int i = 0; i < m_ais.size(); i++) {
			m_fighters[i].set_hurt(false);
		}
		if (m_player1.get_in_play()) { m_player1.set_hurt(false); }
		if (m_player2.get_in_play()) { m_player2.set_hurt(false); }
	}
	m_heat_wave_on = on;
	std::cout << "HEAT WAVE: " << on << std::endl;
}

float World::get_heat_wave_time() {
	if (!m_heat_wave_on) { return -1; }
	return glfwGetTime() - m_heat_wave_time;
}

void World::apply_stage_fx_dmg() {
	bool cond = (int)glfwGetTime() % 2 == 0;
	if (m_player1.get_in_play()) {
		m_player1.set_hurt(true);
		if (cond && !m_player1.is_blocking())
			m_player1.apply_damage(1);
	}
	if (m_player2.get_in_play()) {
		m_player2.set_hurt(true);
		if (cond == 0 && !m_player2.is_blocking())
			m_player2.apply_damage(1);
	}
	for (int i = 0; i < m_ais.size(); i++) { // fighters does not work for some reason
		m_fighters[i].apply_damage(1);
		m_fighters[i].set_hurt(true);
	}
}

