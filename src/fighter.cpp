// Header
#include "fighter.hpp"

#define _USE_MATH_DEFINES
#define MAX_POWER_PUNCH_DMG 49		// +1 (original strength)
#define POWER_PUNCH_CHARGE_RATE 0.5
#define MAX_PUNCH_COOLDOWN 20
#define FULL_BLOCK_TANK 4000
#define STATUS_TIRED_OUT_TIME 80
// POTATO
#define POTATO_MAX_FRIES_COOLDOWN 100
#define POTATO_MAX_BOMB_COOLDOWN 300
#define POTATO_MAX_BOMB_TIMER 500

// BROCCOLI
#define BROCCOLI_MAX_UPPERCUT_COOLDOWN 200
#define BROCCOLI_UPPERCUT_VERT_VELO 500
#define BROCCOLI_MAX_CAULIFLOWERS_COOLDOWN 100
#define BROCCOLI_MAX_CAULIFLOWERS_ON_SCREEN 5
#define MAX_CAULIFLOWERS_VELOCITY 20
#define CAULIFLOWERS_CHARGE_RATE 0.5


#include <math.h>
#include <cmath>

Texture Fighter::f_texture;

Fighter::Fighter(unsigned int id) : m_id(id) {
}

bool Fighter::init(int init_position, std::string name, FighterCharacter fc)
{	
	// Load shared texture
	m_fc = fc;
	set_sprite(ORIGINAL);

	// The position corresponds to the center of the texture
	//float wr = fighter_texture.width * 3.5f;
	//float hr = fighter_texture.height * 3.5f;
	float wr = f_texture.width * 0.5f;
	float hr = f_texture.height * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = {-wr, +hr, -0.02f};
	vertices[0].texcoord = {0.f, 1.f};
	vertices[1].position = {+wr, +hr, -0.02f};
	vertices[1].texcoord = {1.f, 1.f};
	vertices[2].position = {+wr, -hr, -0.02f};
	vertices[2].texcoord = {1.f, 0.f};
	vertices[3].position = {-wr, -hr, -0.02f};
	vertices[3].texcoord = {0.f, 0.f};

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = {0, 3, 1, 1, 3, 2};

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

	m_rng = std::default_random_engine(std::random_device()());
	float rng = m_dist(m_rng);

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture

	m_is_alive = true;
	m_is_idle = true;
	m_is_hurt = false;
	m_is_blocking = false;
	m_blocking_tank = FULL_BLOCK_TANK;

	m_scale.x = 1.2f;
	m_scale.y = 1.2f;
	m_sprite_appearance_size = {f_texture.width /2.0f, f_texture.height / 1.4f };
	m_rotation = 0.f;

	// initialized in fighterInfo.cpp
	MAX_HEALTH = fighterMap[m_fc].health;
	m_speed = fighterMap[m_fc].speed * 1.5f;
	m_strength = fighterMap[m_fc].strength;
	m_mass = 1.f;
	
	m_health = MAX_HEALTH;
	m_lives = STARTING_LIVES;
	m_velocity_y = 0.0;
	m_name = name;
	m_force = { 0.f, 0.f };
	m_friction = 1.f;
	
	switch (init_position) {
	case 1:
		m_position = { 250.f, 525.f };
		m_facing_front = true;
		break;
	case 2:
		m_position = { 950.f, 525.f };
		m_facing_front = false;
		m_scale.x = -m_scale.x;
		break;
	default:
		m_position = { 550.f, 525.f };
		break;
	}

	m_initial_pos = m_position;

	m_nameplate = new TextRenderer(mainFont, 25);
	m_nameplate->setColor({ 0.4f,0.4f,0.4f });

	return true;
}

// Call if init() was successful
// Releases all graphics resources
void Fighter::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
	effect.release();
}

Attack * Fighter::update(float ms, std::vector<Platform> platforms)
{
	vec2 oldPos = m_position;
	Attack * attack = NULL;

	//BLOCKING 
	//Deplete blocking tank if blocking
	if (m_is_blocking) {
		m_blocking_tank -= ms;
	}
	//Stop blocking if blocking tank is empty
	if (m_is_blocking && m_blocking_tank <= 0) set_blocking(false);
	//Recharche blocking tank
	if (m_is_alive && m_blocking_tank < FULL_BLOCK_TANK && !m_is_blocking) {
		m_blocking_tank += ms;
	}

	die();	
	check_respawn(ms);

	if(m_is_alive){
		if (m_crouch_state == CROUCH_PRESSED)
		{
			m_crouch_state = IS_CROUCHING;
		}
		if (m_crouch_state == CROUCH_RELEASED)
		{
			m_crouch_state = NOT_CROUCHING;
		}
		float added_speed = m_force.x / m_mass;
		apply_friction();
		x_position_update(added_speed, ms);
		crouch_update();

		// GENERAL
		// Punch
		if (m_is_punching && !m_punch_on_cooldown)
		{
			//save collision object from punch
			attack = punch();
			m_punch_on_cooldown = true;
		}
		if (m_punch_on_cooldown) {
			if (punching_cooldown >= MAX_PUNCH_COOLDOWN) {
				m_punch_on_cooldown = false;
				punching_cooldown = 0;
			}
			else
				punching_cooldown++;
		}
		// Power punch
		else if (m_is_holding_power_punch) 
			charging_up_power_punch();
		else if (m_is_power_punching) {
			attack = powerPunch();
			m_holding_power_punch_timer = 0;
		}
		// Tired out status
		if (m_tired_out) {
			if (m_tired_out_timer < STATUS_TIRED_OUT_TIME)
				m_tired_out_timer += 0.5;
			else {
				m_tired_out_timer = 0;
				m_tired_out = false;
			}
		}

		// POTATO
		// ABILTIY 1: Fries  (bullet) 
		else if (m_potato_is_holding_fries)
			potato_charging_up_fries();
		else if (m_potato_is_shooting_charged_fries && !m_potato_fries_on_cooldown) {
			attack = new Bullet(get_id(), m_position, 10 + m_potato_holding_fries_timer, m_facing_front);
			m_potato_fries_on_cooldown = true;
			m_potato_holding_fries_timer = 0;
			m_potato_is_shooting_charged_fries = false;
		}
		else if (m_potato_is_shooting_fries && !m_potato_fries_on_cooldown) {
			attack = new Bullet(get_id(), m_position, 10, m_facing_front);
			m_potato_fries_on_cooldown = true;
		}
		if (m_potato_fries_on_cooldown) {
			if (m_potato_fries_cooldown >= POTATO_MAX_FRIES_COOLDOWN) {
				m_potato_fries_on_cooldown = false;
				m_potato_fries_cooldown = 0;
			}
			else
				m_potato_fries_cooldown++;
		}

		// ABILITY 2: Tater Tot Bombs
		
		if (bomb_pointer != NULL) {
			if (bomb_pointer->get_pointer_references() == 1) {
				bomb_pointer->deincrement_pointer_references();
				Mix_PlayChannel(-1, Mix_LoadWAV(audio_path("bomb.wav")), 0);
				delete bomb_pointer;
				bomb_pointer = NULL;
				m_potato_bomb_on_cooldown = true;
				m_potato_bomb_planted = false;
			}
		}
		
		
		if (m_potato_is_planting_bomb && !m_potato_bomb_on_cooldown && !m_potato_bomb_planted) {
			bomb_pointer = new Bomb(get_id(), m_position, 40, 300, POTATO_MAX_BOMB_TIMER);
			attack = bomb_pointer;
			bomb_pointer->increment_pointer_references();
			m_potato_bomb_ticking = true;
			m_potato_bomb_planted = true;
		}
		
		if (m_potato_explode_planted_bomb && m_potato_bomb_planted) {
			Mix_PlayChannel(-1, Mix_LoadWAV(audio_path("bomb.wav")), 0);
			bomb_pointer->m_damageEffect->m_hit_fighter = true;
			bomb_pointer->deincrement_pointer_references();
			if (bomb_pointer->get_pointer_references() == 0) delete bomb_pointer;
			bomb_pointer = NULL;
			m_potato_bomb_on_cooldown = true;
			m_potato_bomb_planted = false;
			m_potato_bomb_ticking = false;
			m_potato_bomb_selftimer = 0;
		}
		
		if (m_potato_bomb_ticking) {
			if (m_potato_bomb_selftimer >= POTATO_MAX_BOMB_TIMER) {
				m_potato_bomb_ticking = false;
				m_potato_bomb_on_cooldown = true;
				m_potato_bomb_selftimer = 0;
			}
			else
				m_potato_bomb_selftimer++;
		}
		if (m_potato_bomb_on_cooldown) {
			if (m_potato_bomb_cooldown >= POTATO_MAX_BOMB_COOLDOWN) {
				m_potato_bomb_on_cooldown = false;
				m_potato_bomb_cooldown = 0;
			} else
				m_potato_bomb_cooldown++;
		}
		
		
		// BROCCOLI
		// Passive: Double Jump
		if (m_broccoli_is_double_jumping && m_broccoli_jump_left == 1) {
			m_velocity_y = -INITIAL_JUMP_VELOCITY;
			m_is_jumping = false;
			m_broccoli_jump_left = 0;
			m_broccoli_is_double_jumping = false;
		}
		// Ability 1: Uppercut
		if (m_broccoli_is_uppercutting && !m_broccoli_uppercut_on_cooldown) {
			m_velocity_y = -BROCCOLI_UPPERCUT_VERT_VELO;
			attack = broccoliUppercut();
			m_broccoli_uppercut_on_cooldown = true;
		}
		if (m_broccoli_uppercut_on_cooldown) {
			if (m_broccoli_uppercut_cooldown >= BROCCOLI_MAX_UPPERCUT_COOLDOWN) {
				m_broccoli_uppercut_on_cooldown = false;
				m_broccoli_uppercut_cooldown = 0;
			}
			else
				m_broccoli_uppercut_cooldown++;
		}

		// ABILITY 2: Cauliflower (projectile)
		if (m_broccoli_is_holding_cauliflowers)
			broccoli_charging_up_cauliflowers();
		if (m_broccoli_is_shooting_charged_cauliflowers && !m_broccoli_cauliflowers_on_cooldown) {
			attack = new Projectile(get_id(), m_position, m_broccoli_holding_cauliflowers_timer, 10 + m_broccoli_holding_cauliflowers_timer, m_facing_front);
			m_broccoli_cauliflowers_on_cooldown = true;
			m_broccoli_holding_cauliflowers_timer = 0;
			m_broccoli_is_shooting_charged_cauliflowers = false;
		}
		if (m_broccoli_is_shooting_cauliflowers && !m_broccoli_cauliflowers_on_cooldown) {
			attack = new Projectile(get_id(), m_position, 0, 10, m_facing_front);
			m_broccoli_cauliflowers_on_cooldown = true;
		}
		if (m_broccoli_cauliflowers_on_cooldown) {
			if (m_broccoli_cauliflowers_cooldown >= BROCCOLI_MAX_CAULIFLOWERS_COOLDOWN) {
				m_broccoli_cauliflowers_on_cooldown = false;
				m_broccoli_cauliflowers_cooldown = 0;
			}
			else
				m_broccoli_cauliflowers_cooldown++;
		}


		//YAM
		//Ability 1: Dash
		if (m_yam_start_dashing) {
			if (m_yam_dash_cooldown_ms <= 0.0) {
				m_yam_dash_timer_ms = MAX_DASH_TIMER;
			}
			m_yam_start_dashing = false;
		}
		if (m_yam_dash_cooldown_ms > 0.0) {
			m_yam_dash_cooldown_ms -= ms;
		}
		if (m_yam_dash_timer_ms > 0.0) {
			float target_ms_per_frame = 1000.f / 60.f;
			float speed_scale = ms / target_ms_per_frame;
			if (!m_moving_backward && !m_moving_forward)
				speed_scale *= 2.0;
			if (m_facing_front && m_position.x < 1150.f) {
				move({ m_speed * speed_scale, 0.0 });
			}
			else if (!m_facing_front && m_position.x > 50.f) {
				move({ -m_speed * speed_scale, 0.0 });
			}
			m_yam_dash_timer_ms -= ms;
			if (m_yam_dash_timer_ms <= 0.0) {
				m_yam_dash_cooldown_ms = MAX_DASH_COOLDOWN;
				attack = dash();
			}
		}

		//YAM
		//Ability 2: Heal
		if (m_yam_is_healing) {
			if (m_yam_heal_cooldown_ms <= 0.0) {
				// heal, but don't go over the health cap
				if (RECOVERY_POINTS + m_health < MAX_HEALTH) { m_health += RECOVERY_POINTS; }
				else { m_health = MAX_HEALTH; }
				//reset cooldown and state
				m_yam_heal_cooldown_ms = MAX_HEAL_COOLDOWN;
				m_yam_heal_animation_ms = MAX_HEAL_ANIMATION;
			}
			m_yam_is_healing = false;
		}
		if (m_yam_heal_cooldown_ms > 0.0) {
			m_yam_heal_cooldown_ms -= ms;
		}
		if(m_yam_heal_animation_ms > 0.0) {
			m_yam_heal_animation_ms -= ms;
		}


		//EGGPLANT
		//Ability 1: Circling emojis
		for (int i = 0; i < m_eggplant_emojis.size(); i++) {
			//emoji update function
			// check for out-of-play emojis and remove
			if (m_eggplant_emojis[i]->get_pointer_references() == 1) {
				// this means it has been removed in the collision loop and we should remove it too!
				m_eggplant_emojis[i]->deincrement_pointer_references();
				delete m_eggplant_emojis[i];
				m_eggplant_emojis[i] = NULL;
				m_eggplant_emojis.erase(m_eggplant_emojis.begin() + i);
				i--;
				m_eggplant_emoji_count--;
			}
			else {
				//otherwise, they are in play, and you will want to provide them your location
				m_eggplant_emojis[i]->set_fighter_pos(m_position);
			}
		}
		if (m_eggplant_spawn_emoji) {
			if (m_eggplant_spawn_cooldown <= 0.0 && m_eggplant_emoji_count < MAX_EMOJI_COUNT) {
				Emoji * e = emoji();
				attack = e;
				m_eggplant_emojis.push_back(e);
				e->increment_pointer_references();
				m_eggplant_spawn_cooldown = MAX_EMOJI_SPAWN_COOLDOWN;
				m_eggplant_emoji_count++;
			}
			m_eggplant_spawn_emoji = false;
		}
		if (m_eggplant_spawn_cooldown > 0.0) {
			m_eggplant_spawn_cooldown -= ms;
		}

		//EGGPLANT
		//Ability 2: Emoji projectile
		if (m_eggplant_shoot_emoji) {
			if (m_eggplant_emoji_count > 0) {
				Emoji * e = NULL;
				// search for the first circling emoji, if any
				for (int i = 0; i < m_eggplant_emojis.size(); i++) {
					if (m_eggplant_emojis[i]->is_circling()) {
						e = m_eggplant_emojis[i];
						break;
					}
				}
				if (e != NULL) 
					e->fire_emoji(m_facing_front);
			}
			m_eggplant_shoot_emoji = false;
		}
		if (m_eggplant_shoot_cooldown > 0.0) {
			m_eggplant_shoot_cooldown -= ms;
		}

	}
	else
	{
		if (m_facing_front)
			m_rotation = -M_PI / 2;
		else
			m_rotation = M_PI / 2;
	}
	
	y_position_update(ms);
	platform_collision(platforms, oldPos);

	//return null if not attacking, or the collision object if attacking
	return attack;
}

void Fighter::draw(const mat3 &projection)
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint is_hurt_uloc = glGetUniformLocation(effect.program, "is_hurt");
	GLint is_blocking_uloc = glGetUniformLocation(effect.program, "is_blocking");
	GLint heal_animation_uloc = glGetUniformLocation(effect.program, "heal_animation");
	GLint blocking_tank_uloc = glGetUniformLocation(effect.program, "blocking_tank");
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

	if (get_alive())
	{
		if (is_paused()) {

		}

		else if (is_uppercutting())
		{
			m_punch_counter++;
			set_sprite(UPPERCUT);

			if (m_punch_counter > 30)
			{
				m_punch_counter = 0;
				set_uppercut(false);
			}
		}

		else if (is_punching()) {
			if (!is_crouching()) { set_sprite(PUNCH); }
			else if (is_crouching()) { set_sprite(CROUCH_PUNCH); }
		}

		else if (!is_punching() && is_crouching()) { set_sprite(CROUCH); }

		else if (is_holding_power_punch()) { set_sprite(CHARGING); }

		else if (is_power_punching())
		{
			m_punch_counter++;
			set_sprite(POWER_PUNCH);

			if (m_punch_counter > 20)
			{
				m_punch_counter = 0;
				set_power_punch(false);
			}
		}
		else if (!is_punching())
		{
			if (get_alive() && !is_crouching())
			{
				m_anim_counter++;
				if (m_anim_counter < 25) { set_sprite(IDLE); }
				else if (m_anim_counter > 25 && m_anim_counter < 50) { set_sprite(ORIGINAL); }
				else if (m_anim_counter >= 50)
					m_anim_counter = 0;
			}
		}
	}

	else if (!get_alive())
	{
		set_sprite(DEATH);
	}

	glBindTexture(GL_TEXTURE_2D, f_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform);
	float color[] = {1.f, 1.f, 1.f};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);
	glUniform1i(is_hurt_uloc, m_is_hurt);
	glUniform1i(is_blocking_uloc, m_is_blocking);
	glUniform1f(blocking_tank_uloc, (float)  m_blocking_tank);
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	glUniform1f(heal_animation_uloc, m_yam_heal_animation_ms);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	
	int sWidth = m_nameplate->get_width_of_string(m_name);
	m_nameplate->setPosition({ m_position.x - sWidth*.45f, m_position.y - 70.0f });
}


float Fighter::get_rotation() const
{
	return m_rotation;
}


vec2 Fighter::get_scale() const
{
	return m_scale;
}

vec2 Fighter::get_position() const
{
	return m_position;
}

void Fighter::set_position(vec2 position)
{
	m_position = position;
}

void Fighter::move(vec2 off)
{
	m_position.x += off.x;
	m_position.y += off.y;
}

//// Returns the local bounding coordinates scaled by the current size of the bubble
BoundingBox * Fighter::get_bounding_box() const
{
	// fabs is to avoid negative scale due to the facing direction
	float width = std::fabs(m_scale.x) * m_sprite_appearance_size.x;
	float height = std::fabs(m_scale.y) * m_sprite_appearance_size.y;
	// get position gets center of texture, but we want top left corner position for bounding box
	float topLeftXpos = get_position().x - (width / 2);
	float topLeftYpos = get_position().y - (height / 2);
	return new BoundingBox(topLeftXpos, topLeftYpos, width, height);
}

// set fighter's movements
void Fighter::set_movement(int mov)
{
	switch (mov)
	{
	case MOVING_FORWARD:
		m_moving_forward = true;
		m_is_idle = false;
		break;
	case MOVING_BACKWARD:
		m_moving_backward = true;
		m_is_idle = false;
		break;
	case START_JUMPING:
		start_jumping();
		break;
	case CROUCHING:
		m_crouch_state = CROUCH_PRESSED;
		m_is_idle = false;
		break;
	case PUNCHING:
		if (!m_is_blocking && !m_tired_out) {
			m_is_punching = true;
			m_is_idle = false;
		}
		break;
	case ABILITY_1:
		if (m_fc == POTATO && !m_is_blocking && !m_tired_out) {
			if (!m_potato_bomb_planted) m_potato_is_planting_bomb = true;
			else m_potato_explode_planted_bomb = true;
			m_is_idle = false;
		}
		else if (m_fc == BROCCOLI && !m_is_blocking && !m_tired_out) {
			m_broccoli_is_uppercutting = true;
			m_is_idle = false;
		}
		else if (m_fc == YAM) {
			m_yam_start_dashing = true;
			m_is_idle = false;
		}
		else if (m_fc == EGGPLANT) {
			m_eggplant_spawn_emoji = true;
		}
		break;
	case ABILITY_2:
		if (m_fc == POTATO && !m_is_blocking && !m_tired_out) {
			m_potato_is_shooting_fries = true;
			m_is_idle = false;
		}
		else if (m_fc == BROCCOLI && !m_is_blocking && !m_tired_out) {
			m_broccoli_is_shooting_cauliflowers = true;
			m_is_idle = false;
		}
		else if (m_fc == YAM) {
			m_yam_is_healing = true;
			m_is_idle = false;
		}
		else if (m_fc == EGGPLANT) {
			m_eggplant_shoot_emoji = true;
			m_is_idle = false;
		}
		break;
	case HOLDING_ABILITY_2:
		if (m_fc == POTATO && !m_is_blocking && !m_tired_out) {
			m_potato_is_holding_fries = true;
			m_potato_is_shooting_fries = false;
			m_is_idle = false;
		}
		else if (m_fc == BROCCOLI && !m_is_blocking && !m_tired_out) {
			m_broccoli_is_holding_cauliflowers = true;
			m_broccoli_is_shooting_cauliflowers = false;
			m_is_idle = false;
		}
		break;
	case CHARGED_ABILITY_2:
		if (m_fc == POTATO && !m_is_blocking && !m_tired_out) {
			m_potato_is_holding_fries = false;
			m_potato_is_shooting_charged_fries = true;
			m_is_idle = false;
		}
		else if (m_fc == BROCCOLI && !m_is_blocking && !m_tired_out) {
			m_broccoli_is_holding_cauliflowers = false;
			m_broccoli_is_shooting_charged_cauliflowers = true;
			m_is_idle = false;
		}
		break;
	case HOLDING_POWER_PUNCH:
		m_is_holding_power_punch = true;
		m_is_punching = false;
		m_is_idle = false;
		break;
	case POWER_PUNCHING:
		m_is_holding_power_punch = false;
		m_is_power_punching = true;
		m_is_idle = true;
		break;
	case STOP_MOVING_FORWARD:
		m_moving_forward = false;
		if (m_moving_backward == false && !m_is_holding_power_punch)
			m_is_idle = true;
		break;
	case STOP_MOVING_BACKWARD:
		m_moving_backward = false;
		if (m_moving_forward == false && !m_is_holding_power_punch)
			m_is_idle = true;
		break;
	case RELEASE_CROUCH:
		m_crouch_state = CROUCH_RELEASED;
		m_is_idle = true;
		break;
	case STOP_PUNCHING:
		m_is_punching = false;
		m_is_idle = true;
		break;
	case STOP_ABILITIES:
		if (m_fc == POTATO) {
			m_potato_is_planting_bomb = false;
			m_potato_explode_planted_bomb = false;
			m_potato_is_shooting_fries = false;
			m_is_idle = true;
		}
		else if (m_fc == BROCCOLI) {
			m_broccoli_is_uppercutting = false;
			m_broccoli_is_shooting_cauliflowers = false;
			m_is_idle = true;
		}
		break;
	case BLOCKING:
		//CANNOT BLOCK UNTIL BLOCKING TANK IS ATLEAST 1000 (1second of recharge)
		if (!m_is_punching && m_blocking_tank >= 1000)
			m_is_blocking = true;
		break;
	case STOP_BLOCKING:
		m_is_blocking = false;
		break;
	case PAUSED:
		m_is_paused = true;
		break;
	case UNPAUSED:
		m_is_paused = false;
		break;
	}
}


void Fighter::set_hurt(bool hurt) {
	m_is_hurt = hurt;
}

void Fighter::apply_damage(DamageEffect * damage_effect) {
	if (damage_effect->m_damage <= m_health) {
		m_health -= damage_effect->m_damage;

		if (damage_effect->m_bounding_box.xpos + (damage_effect->m_bounding_box.width / 2) > m_position.x) {
			if (damage_effect->m_vert_force > 0) {
				m_force.x -= 0.3f * damage_effect->m_damage;
				m_velocity_y = -damage_effect->m_vert_force;
			} else 
				m_force.x -= 1.f * damage_effect->m_damage;
		}
		else {
			if (damage_effect->m_vert_force > 0) {
				m_force.x += 0.3f * damage_effect->m_damage;
				m_velocity_y = -damage_effect->m_vert_force;
			}
			else
				m_force.x += 1.f * damage_effect->m_damage;
		}
	} else {
		m_health = 0;
	}
}

void Fighter::apply_damage(int effect) { // for stage effects
	if (effect <= m_health) { m_health -= effect; }
	else { m_health = 0; }
}

void Fighter::set_blocking(bool blocking) {
	m_is_blocking = blocking;
}

int Fighter::get_health() const
{
	return m_health;
}

int Fighter::get_block_tank() const
{
	return m_blocking_tank;
}

int Fighter::get_lives() const
{
	return m_lives;
}
TextRenderer* Fighter::get_nameplate() const {
	return m_nameplate;
}
std::string Fighter::get_name() const {
	return m_name;
}

void Fighter::start_jumping()
{
	if (!m_is_jumping && m_is_alive)
	{
		m_is_jumping = true;
		m_is_idle = false;
		m_velocity_y = -INITIAL_JUMP_VELOCITY;
		if (m_fc == BROCCOLI)
			m_broccoli_jump_left--;
	}
}

void Fighter::apply_friction() {
	if (m_force.x > 0.f) {
		m_force.x = std::max(m_force.x - m_friction, 0.f);
	} else if (m_force.x < 0.f) {
		m_force.x = std::min(m_force.x + m_friction, 0.f);
	}
}

void Fighter::x_position_update(float added_speed, float ms) {

	//!!! need to include this before merge
	//if (m_is_holding_power_punch)
	//	MOVING_SPEED = POWER_PUNCHING_MOVING_SPEED;

	float target_ms_per_frame = 1000.f / 60.f;
	float speed_scale = ms / target_ms_per_frame;

	if (m_moving_forward)
	{
		if (!m_facing_front)
		{
			m_scale.x = -m_scale.x;
			m_facing_front = true;
		}
		if (m_position.x < 1150.f) {
			if (m_is_holding_power_punch || m_broccoli_is_holding_cauliflowers)
				move({ m_speed * 0.3f * speed_scale, 0.0 });
			else if (m_tired_out)
				move({ m_speed * 0.03f * speed_scale, 0.0 });
			else
				move({ m_speed * speed_scale, 0.0});
		}
	}
	if (m_moving_backward)
	{
		if (m_facing_front)
		{
			m_scale.x = -m_scale.x;
			m_facing_front = false;
		}
		if (m_position.x > 50.f) {
			if (m_is_holding_power_punch || m_broccoli_is_holding_cauliflowers)
				move({ -m_speed * 0.3f * speed_scale, 0.0 });
			else if (m_tired_out)
				move({ -m_speed * 0.03f * speed_scale, 0.0 });
			else
				move({ -m_speed * speed_scale, 0.0 });
		}
	}

	move({ added_speed * speed_scale, 0.f });
}

void Fighter::crouch_update() {
	if (m_crouch_state == CROUCH_PRESSED)
	{
		m_scale.y = 0.1f;
		m_position.y += 25.f;
		m_crouch_state = IS_CROUCHING;
	}
	if (m_crouch_state == CROUCH_RELEASED)
	{
		m_scale.y = 0.2f;
		m_position.y -= 25.f;
		m_crouch_state = NOT_CROUCHING;
	}
}

void Fighter::die() {
	if (m_health <= 0 && m_is_alive || (m_position.y > (800 + 100) && m_is_alive))
	{
		m_is_alive = false;
		m_lives--;
		set_movement(STOP_MOVING_BACKWARD);
		set_movement(STOP_MOVING_FORWARD);
		m_is_punching = false;
		m_is_jumping = false;

		//uncrouch in death
		if (m_crouch_state == IS_CROUCHING) {
			m_scale.y = 0.2f;
			m_position.y -= 25.f;
			m_crouch_state = NOT_CROUCHING;
		}
		else if (m_crouch_state == CROUCH_PRESSED) {
			m_crouch_state = NOT_CROUCHING;
		}

		if (m_eggplant_emojis.size() > 0) {
			for (auto &eggplant_emoji : m_eggplant_emojis) {
				eggplant_emoji->m_damageEffect->m_hit_fighter = true;
				eggplant_emoji->deincrement_pointer_references();
				if (eggplant_emoji->get_pointer_references() == 0) {
					delete eggplant_emoji;
					eggplant_emoji = NULL;
				}
			}
			m_eggplant_emojis.clear();
			m_eggplant_emoji_count = 0;
		}

		if (m_lives > 0)
		{
			m_respawn_timer = RESPAWN_TIME;
			m_respawn_flag = true;
		}
	}
}

void Fighter::check_respawn(float ms) {
	if (m_respawn_flag)
	{
		if (m_respawn_timer > 0)
		{
			//count down by time passed
			m_is_punching = false;
			m_respawn_timer -= ms;
		}
		else
		{
			//reset flags and revive
			m_respawn_flag = false;
			m_respawn_timer = 0;
			m_is_alive = true;
			m_is_hurt = false;
			m_health = MAX_HEALTH;
			m_rotation = 0;
			m_position = m_initial_pos;
			m_blocking_tank = FULL_BLOCK_TANK;
			m_tired_out = false;
			m_is_jumping = false;
			m_is_punching = false;
			m_is_holding_power_punch = false;
			m_is_power_punching = false;
			m_force.x = 0;
			m_force.y = 0;
			// Potato flags
			m_potato_is_shooting_fries = false;
			m_potato_is_holding_fries = false;
			m_potato_fries_cooldown = 0;
			m_potato_fries_on_cooldown = false;
			m_potato_bomb_planted = false;
			m_potato_bomb_selftimer = 0;
			m_potato_bomb_ticking = 0;
			m_potato_bomb_cooldown = 0;
			m_potato_bomb_on_cooldown = false;
			// Broccoli flags
			m_broccoli_is_double_jumping = false;
			m_broccoli_jump_left = 2;
			m_broccoli_is_holding_cauliflowers = false;
			m_broccoli_is_shooting_cauliflowers = false;
			m_broccoli_cauliflowers_cooldown = 0;
			m_broccoli_cauliflowers_on_cooldown = false;
			m_broccoli_is_uppercutting = false;
			m_broccoli_uppercut_on_cooldown = false;
			m_broccoli_uppercut_cooldown = 0;		
		}
	}
}

bool Fighter::is_paused() const
{
	return m_is_paused;
}

bool Fighter::is_hurt() const
{
	return m_is_hurt;
}

bool Fighter::is_jumping() const
{
	return m_is_jumping;
}

bool Fighter::is_punching() const
{
	return m_is_punching;
}

bool Fighter::is_power_punching() const
{
	return m_is_power_punching;
}

bool Fighter::is_punching_on_cooldown() const {
	return m_punch_on_cooldown;
}


bool Fighter::is_holding_power_punch() const {
	return m_is_holding_power_punch;
}

bool Fighter::potato_is_holding_fries() const {
	return m_potato_is_holding_fries;
}


bool Fighter::broccoli_is_holding_cauliflowers() const {
	return m_broccoli_is_holding_cauliflowers;
}

bool Fighter::is_crouching() const
{
	return m_crouch_state == IS_CROUCHING;
}

bool Fighter::is_idle() const
{
	return m_is_idle;
}

bool Fighter::is_blocking() const
{
	return m_is_blocking;
}

bool Fighter::is_tired_out() const {
	return m_tired_out;
}

int Fighter::get_blocking_tank() const{
	return m_blocking_tank;
}

int Fighter::get_crouch_state() {
	return m_crouch_state;
}

void Fighter::set_crouch_state(CrouchState cs) {
	m_crouch_state = cs;
}

void Fighter::set_power_punch(bool punch)
{
	m_is_power_punching = punch;
}

void Fighter::set_uppercut(bool uc)
{
	m_broccoli_is_uppercutting = uc;
}

int Fighter::get_alive() const
{
	return m_is_alive;
}

bool Fighter::get_facing_front() const
{
	return m_facing_front;
}

unsigned int Fighter::get_id() const
{
	return m_id;
}

FighterCharacter Fighter::get_fc() const {
	return m_fc;
}

void Fighter::broccoli_set_double_jump() {
	m_broccoli_is_double_jumping = true;
}

bool Fighter::broccoli_get_jump_left() {
	return m_broccoli_jump_left;
}

bool Fighter::broccoli_is_uppercut_on_cooldown() {
	return m_broccoli_uppercut_on_cooldown;
}

bool Fighter::is_uppercutting() const {
	return m_broccoli_is_uppercutting;
}

//void Fighter::reset(int init_position)
void Fighter::reset()
{
	m_health = MAX_HEALTH;
	m_lives = STARTING_LIVES;
	m_is_alive = true;
	m_rotation = 0;
	m_is_jumping = false;
	m_is_punching = false;
	m_potato_is_shooting_fries = false;
	m_broccoli_is_holding_cauliflowers = false;
	m_broccoli_is_shooting_cauliflowers = false;
	m_broccoli_is_double_jumping = false;
	m_broccoli_jump_left = 2;
	m_broccoli_is_uppercutting = false;
	m_broccoli_uppercut_on_cooldown = false;
	m_broccoli_uppercut_cooldown = 0;
	m_is_holding_power_punch = false;
	m_is_power_punching = false;
	m_velocity_y = 0.0;
	m_moving_forward = false;
	m_moving_backward = false;
	m_position = m_initial_pos;


	if (m_crouch_state == IS_CROUCHING || m_crouch_state == CROUCH_PRESSED) {
		m_crouch_state = CROUCH_RELEASED;
		m_position.y += 25.f;
	}
}

Punch * Fighter::punch() {
	//create the bounding box based on fighter position
	float sizeMultiplier = 1.75;
	BoundingBox* b = get_bounding_box();
	Punch* punch;
	if (get_facing_front()) {
		//right facing
		float xpos = b->xpos + (b->width / 2.0);
		float width = sizeMultiplier * (b->width / 2.0);
		punch = new Punch(get_id(), { xpos, b->ypos }, { width, b->height }, m_strength, true);
	}
	else {
		//left facing
		float xpos = b->xpos - ((sizeMultiplier - 1) * (b->width / 2.0));
		float width = sizeMultiplier * (b->width / 2.0);
		punch = new Punch(get_id(), { xpos, b->ypos }, { width, b->height }, m_strength, false);
	}
	delete b;
	return punch;
}

Punch * Fighter::powerPunch() {
	//create the bounding box based on fighter position
	float sizeMultiplier = 1.75;
	BoundingBox* b = get_bounding_box();
	Punch* punch;
	if (get_facing_front()) {
		//right facing
		float xpos = b->xpos + (b->width / 2.0);
		float width = sizeMultiplier * (b->width / 2.0);
		punch = new Punch(get_id(), { xpos, b->ypos }, { width, b->height }, m_strength + m_holding_power_punch_timer, true);
	}
	else {
		//left facing
		float xpos = b->xpos - ((sizeMultiplier - 1) * (b->width / 2.0));
		float width = sizeMultiplier * (b->width / 2.0);
		punch = new Punch(get_id(), { xpos, b->ypos }, { width, b->height }, m_strength + m_holding_power_punch_timer, false);
	}

	delete b;
	return punch;
}

Uppercut * Fighter::broccoliUppercut() {
	//create the bounding box based on fighter position
	float sizeMultiplier = 1.75;
	BoundingBox* b = get_bounding_box();
	Uppercut* uppercut;
	if (get_facing_front()) {
		//right facing
		float xpos = b->xpos + (b->width / 2.0);
		float width = sizeMultiplier * (b->width / 2.0);
		uppercut = new Uppercut(get_id(), { xpos, b->ypos }, { width, b->height }, 15, true, BROCCOLI_UPPERCUT_VERT_VELO, 20);
	}
	else {
		//left facing
		float xpos = b->xpos - ((sizeMultiplier - 1) * (b->width / 2.0));
		float width = sizeMultiplier * (b->width / 2.0);
		uppercut = new Uppercut(get_id(), { xpos, b->ypos }, { width, b->height }, 15, false, BROCCOLI_UPPERCUT_VERT_VELO, 20);
	}

	delete b;
	return uppercut;
}

//revert to old position if new position causes a collision with platforms
void Fighter::platform_collision(std::vector<Platform> platforms, vec2 oldPosition) {
	for (int i = 0; i < platforms.size(); i++) {
		BoundingBox* b = get_bounding_box();
		if (platforms[i].check_collision(*b)) {
			if (platforms[i].check_collision_outer_left(*b)) {
				m_position = oldPosition;
				m_velocity_y = 0.0;
				m_is_jumping = false;
			}
			else if (platforms[i].check_collision_outer_right(*b)) {
				m_position = oldPosition;
				m_velocity_y = 0.0f;
				m_is_jumping = false;
			}

			if (platforms[i].check_collision_outer_top(*b)) {
				m_position.y = oldPosition.y;
				m_velocity_y = 0.0f;
				m_is_jumping = false;
				m_broccoli_jump_left = 2;
			}
			else if (platforms[i].check_collision_outer_bottom(*b)) {
				m_position.y = oldPosition.y;
				m_velocity_y = 0.0f;
			}

			if (!platforms[i].check_collision_outer_left(*b) && !platforms[i].check_collision_outer_right(*b) &&
				!platforms[i].check_collision_outer_top(*b) && !platforms[i].check_collision_outer_bottom(*b)) {
				m_position = oldPosition;
				m_velocity_y = 0.0;
				m_is_jumping = false;
			}
		}
		delete b;
	}
}

void Fighter::y_position_update(float ms) {
	float s = ms / 1000;
	m_position.y += m_velocity_y * s;
	m_velocity_y += GRAVITY.y * s;
	if (m_velocity_y > TERMINAL_VELOCITY_Y) {
		m_velocity_y = TERMINAL_VELOCITY_Y;
	}
	else if (m_velocity_y < -TERMINAL_VELOCITY_Y) {
		m_velocity_y = -TERMINAL_VELOCITY_Y;
	}
}

void Fighter::charging_up_power_punch() {
	m_holding_too_much_timer += 0.5;
	if (m_holding_power_punch_timer < MAX_POWER_PUNCH_DMG)
		m_holding_power_punch_timer += POWER_PUNCH_CHARGE_RATE;
	if (m_holding_too_much_timer >= 120) {
		m_tired_out = true;
		m_is_holding_power_punch = false;
		m_holding_power_punch_timer = 0;
		m_holding_too_much_timer = 0;
	}
}

void Fighter::broccoli_charging_up_cauliflowers() {
	m_holding_too_much_timer += 0.5;
	if (m_broccoli_holding_cauliflowers_timer < MAX_CAULIFLOWERS_VELOCITY)
		m_broccoli_holding_cauliflowers_timer += CAULIFLOWERS_CHARGE_RATE;
	if (m_holding_too_much_timer >= 120) {
		m_tired_out = true;
		m_broccoli_is_holding_cauliflowers = false;
		m_broccoli_holding_cauliflowers_timer = 0;
		m_holding_too_much_timer = 0;
	}
}

void Fighter::potato_charging_up_fries() {
	m_holding_too_much_timer += 0.5;
	if (m_potato_holding_fries_timer < MAX_CAULIFLOWERS_VELOCITY)
		m_potato_holding_fries_timer += CAULIFLOWERS_CHARGE_RATE;
	if (m_holding_too_much_timer >= 120) {
		m_tired_out = true;
		m_potato_is_holding_fries = false;
		m_potato_holding_fries_timer = 0;
		m_holding_too_much_timer = 0;
	}
}

float Fighter::get_heal_animation() {
	return m_yam_heal_animation_ms;
}

Dash * Fighter::dash() {
	BoundingBox* b = get_bounding_box();
	Dash* dash = new Dash(get_id(), { b->xpos, b->ypos }, { b->width, b->height }, m_strength, 0);
	delete b;
	return dash;
}

Emoji * Fighter::emoji() {
	Emoji * emoji = new Emoji(get_id(), m_position, m_strength, m_facing_front);
	return emoji;
}

void Fighter::set_sprite(SpriteType st) const {
	if (m_fc == POTATO) {
		switch (st) {
			default: f_texture = POTATO_TEXTURE; break;
			case IDLE: f_texture = POTATO_IDLE_TEXTURE; break;
			case PUNCH: f_texture = POTATO_PUNCH_TEXTURE; break;
			case POWER_PUNCH: f_texture = POTATO_POWER_PUNCH_TEXTURE; break;
			case CROUCH_PUNCH: f_texture = POTATO_CROUCH_PUNCH_TEXTURE; break;
			case CROUCH: f_texture = POTATO_CROUCH_TEXTURE; break;
			case CHARGING: f_texture = POTATO_CHARGING_TEXTURE; break;
			case DEATH: f_texture = POTATO_DEATH_TEXTURE; break;
			case TIRED_1: f_texture = POTATO_TIRED_1_TEXTURE; break;
			case TIRED_2: f_texture = POTATO_TIRED_2_TEXTURE; break;
		}
	}
	else if (m_fc == BROCCOLI) {
		switch (st) {
			default: f_texture = BROCCOLI_TEXTURE; break;
			case IDLE: f_texture = BROCCOLI_IDLE_TEXTURE; break;
			case PUNCH: f_texture = BROCCOLI_PUNCH_TEXTURE; break;
			case POWER_PUNCH: f_texture = BROCCOLI_PUNCH_TEXTURE; break; // TODO: STUB
			case CROUCH_PUNCH: f_texture = BROCCOLI_CROUCH_PUNCH_TEXTURE; break;
			case CROUCH: f_texture = BROCCOLI_CROUCH_TEXTURE; break;
			case DEATH: f_texture = BROCCOLI_DEATH_TEXTURE; break;
			case UPPERCUT: f_texture = BROCCOLI_UPPERCUT_TEXTURE; break;
			case TIRED_1: f_texture = BROCCOLI_TIRED_1_TEXTURE; break;
			case TIRED_2: f_texture = BROCCOLI_TIRED_2_TEXTURE; break;
			case CHARGING: f_texture = BROCCOLI_CHARGING_TEXTURE; break;
		}
	}
	else if (m_fc == EGGPLANT) { // TODO: STUB
		switch (st) {
		default: f_texture = EGGPLANT_TEXTURE; break;
		case IDLE: f_texture = EGGPLANT_IDLE_TEXTURE; break;
		case PUNCH: f_texture = EGGPLANT_PUNCH_TEXTURE; break;
		case POWER_PUNCH: f_texture = EGGPLANT_POWER_PUNCH_TEXTURE; break;
		case CROUCH_PUNCH: f_texture = EGGPLANT_CROUCH_PUNCH_TEXTURE; break;
		case CROUCH: f_texture = EGGPLANT_CROUCH_TEXTURE; break;
		case CHARGING: f_texture = EGGPLANT_CHARGING_TEXTURE; break;
		case DEATH: f_texture = EGGPLANT_DEATH_TEXTURE; break;
		case TIRED_1: f_texture = EGGPLANT_TIRED_1_TEXTURE; break;
		case TIRED_2: f_texture = EGGPLANT_TIRED_2_TEXTURE; break;
		}
	} else if (m_fc == YAM) {
		switch (st) {
		default: f_texture = YAM_TEXTURE; break;
		case IDLE: f_texture = YAM_IDLE_TEXTURE; break;
		case PUNCH: f_texture = YAM_PUNCH_TEXTURE; break;
		case POWER_PUNCH: f_texture = YAM_POWER_PUNCH_TEXTURE; break;
		case CROUCH_PUNCH: f_texture = YAM_CROUCH_PUNCH_TEXTURE; break;
		case CROUCH: f_texture = YAM_CROUCH_TEXTURE; break;
		case CHARGING: f_texture = YAM_CHARGING_TEXTURE; break;
		case DEATH: f_texture = YAM_DEATH_TEXTURE; break;
		case TIRED_1: f_texture = YAM_TIRED_1_TEXTURE; break;
		case TIRED_2: f_texture = YAM_TIRED_2_TEXTURE; break;
		}
	}
}
