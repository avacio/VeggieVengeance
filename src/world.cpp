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

World::World()
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

	if (m_background_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
				audio_path("Abandoned Hopes.wav"));
		return false;
	}

	// Playing background music undefinitely
	Mix_PlayMusic(m_background_music, -1);

	fprintf(stderr, "Loaded music\n");

	////// TEXT
	//text = new TextRenderer(mainFont, 40);

	//int text_width = text->get_width_of_string("HEALTH: 100"); // TODO
	////text->setPosition({ screen.x / 33.3f, screen.y - 10.f });
	//text->setPosition({ 50.f, 100.f});
	////text->setPosition({ screen.x / 2.f, screen.y/2.f });
	////text->setColor({ 255.0f,0.0f,0.0f });

	//fprintf(stderr, "Loaded text\n");

	//spawn fighters below
	//indicates success of initialization operations, if even one failure occurs it should be false
	bool initSuccess = true;

	m_mode = mode;
	if (mode == DEV)
	{
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
			initSuccess = initSuccess && m_player1.init(1);
		}

		if (m_player2.get_in_play())
		{
			initSuccess = initSuccess && m_player2.init(2);
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
	}
	else if (mode == PVP)
	{
		m_player1.set_in_play(true);
		m_player2.set_in_play(true);
		initSuccess = initSuccess && m_player1.init(1) && m_player2.init(2);
	}
	else if (mode == TUTORIAL)
	{
		m_player1.set_in_play(true);
		initSuccess = initSuccess && m_player1.init(1) && spawn_ai(AVOID);
	}
	m_paused = false;

	return m_water.init() && m_bg.init() && initSuccess;
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
		if (m_player1.get_in_play())
		{
			m_player1.update(elapsed_ms);
		}
		if (m_player2.get_in_play())
		{
			m_player2.update(elapsed_ms);
		}

		if (m_player1.get_in_play())
		{
			for (auto &ai : m_ais)
				ai.update(elapsed_ms * 0.5, m_player1.get_position());
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

	// Updating window title with points
	/*std::stringstream title_ss;
	int stock_display1 = 0;
	int stock_display2 = 0;
	int health_display1 = 0;
	int health_display2 = 0;
	if (m_player1.get_in_play())
	{
		health_display1 = m_player1.get_health();
		stock_display1 = m_player1.get_lives();
	}
	if (m_player2.get_in_play())
	{
		health_display2 = m_player2.get_health();
		stock_display2 = m_player2.get_lives();
	}
	title_ss << "Veggie Vengeance  -  Player 1's Stock: " << stock_display1 << " Health: " << health_display1 << " || Player 2's Stock: " << stock_display2 << " Health: " << health_display2;
	glfwSetWindowTitle(m_window, title_ss.str().c_str());
*/
	if (m_mode == DEV || m_mode == PVP)
	{
		m_bg.setPlayerInfo(m_player1.get_lives(), m_player1.get_health(), m_player2.get_lives(), m_player2.get_health());
	}
	else if (m_mode == TUTORIAL)
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

	//text->renderString(projection_2D, "Health: 100");

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
	AI ai(type);
	if (ai.init(3))
	{
		m_ais.emplace_back(ai);
		return true;
	}
	fprintf(stderr, "Failed to spawn fighter");
	return false;
}

// On key callback
void World::on_key(GLFWwindow *, int key, int, int action, int mod)
{
	// Handle player movement here
	if (m_player1.get_in_play() && !m_paused)
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
		if (action == GLFW_RELEASE && key == GLFW_KEY_D)
			m_player1.set_movement(STOP_MOVING_FORWARD);
		if (action == GLFW_RELEASE && key == GLFW_KEY_A)
			m_player1.set_movement(STOP_MOVING_BACKWARD);
		if (action == GLFW_RELEASE && key == GLFW_KEY_S)
			m_player1.set_movement(RELEASE_CROUCH);
		if (action == GLFW_RELEASE && key == GLFW_KEY_E)
			m_player1.set_movement(STOP_PUNCHING);
	}

	if (m_player2.get_in_play() && !m_paused)
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
		if (action == GLFW_RELEASE && key == GLFW_KEY_L)
			m_player2.set_movement(STOP_MOVING_FORWARD);
		if (action == GLFW_RELEASE && key == GLFW_KEY_J)
			m_player2.set_movement(STOP_MOVING_BACKWARD);
		if (action == GLFW_RELEASE && key == GLFW_KEY_K)
			m_player2.set_movement(RELEASE_CROUCH);
		if (action == GLFW_RELEASE && key == GLFW_KEY_O)
			m_player2.set_movement(STOP_PUNCHING);
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
		m_paused = !m_paused;
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_B)
	{
		reset();
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
	if (m_mode == DEV)
	{
		m_player1.reset(1);
		m_player2.reset(2);

		for (AI ai : m_ais)
		{
			ai.reset(3);
		}
	}
	else if (m_mode == PVP)
	{
		m_player1.reset(1);
		m_player2.reset(2);
	}
	else if (m_mode == TUTORIAL)
	{
		m_player1.reset(1);

		for (AI ai : m_ais)
		{
			ai.reset(3);
		}
	}
}

void World::on_mouse_move(GLFWwindow *window, double xpos, double ypos)
{
}