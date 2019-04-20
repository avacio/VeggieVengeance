#pragma once

#include <boundingBox.hpp>

// stlib
#include <fstream> // stdout, stderr..
#include <sstream>
#include <random>

// glfw
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// Simple utility macros to avoid mistyping directory name, name has to be a string literal
// audio_path("audio.ogg") -> data/audio/audio.ogg
// Get defintion of PROJECT_SOURCE_DIR from:
#include "project_path.hpp"

#define shader_path(name) PROJECT_SOURCE_DIR "./shaders/" name

#define data_path PROJECT_SOURCE_DIR "./data"
#define textures_path(name) data_path "/textures/" name
#define audio_path(name) data_path "/audio/" name
#define mesh_path(name) data_path "/meshes/" name

#define PI 3.14159265

std::string fonts_path(std::string name);
static const std::string mainFont = "HABESHAPIXELS.ttf";
static const std::string mainFontBold = "HABESHAPIXELS-Bold.ttf";

// Not much math is needed and there are already way too many libraries linked (:
// If you want to do some overloads..
struct vec2
{
	float x, y;
};
struct vec3
{
	float x, y, z;
};
struct mat3
{
	vec3 c0, c1, c2;
};

// Utility functions
float dot(vec2 l, vec2 r);
float dot(vec3 l, vec3 r);
mat3 mul(const mat3 &l, const mat3 &r);
vec2 normalize(vec2 v);
int get_random_number(int max);

// OpenGL utilities
// cleans error buffer
void gl_flush_errors();
bool gl_has_errors();

// Single Vertex Buffer element for non-textured meshes (colored.vs.glsl)
struct Vertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Texture wrapper
struct Texture
{
	Texture();
	~Texture();

	GLuint id;
	GLuint depth_render_buffer_id;
	int width;
	int height;

	// Loads texture from file specified by path
	bool load_from_file(const char *path);

	// Screen texture
	bool create_from_screen(GLFWwindow const *const window);
	bool is_valid() const; // True if texture is valid
};

// A Mesh is a collection of a VertexBuffer and an IndexBuffer. A VAO
// represents a Vertex Array Object and is the container for 1 or more Vertex Buffers and
// an Index Buffer
struct Mesh
{
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
};

// Container for Vertex and Fragment shader, which are then put(linked) together in a
// single program that is then bound to the pipeline.
struct Effect
{
	bool load_from_file(const char *vs_path, const char *fs_path);
	void release();

	GLuint vertex;
	GLuint fragment;
	GLuint program;
};

// Helper container for all the information we need when rendering an object together
// with its transform.
struct Renderable
{
	Mesh mesh;
	Effect effect;
	mat3 transform;

	// projection contains the orthographic projection matrix. As every Renderable::draw()
	// renders itself it needs it to correctly bind it to its shader.
	virtual void draw(const mat3 &projection) = 0;

	// gl Immediate mode equivalent, see the Rendering and Transformations section in the
	// specification pdf
	void transform_begin();
	void transform_scale(vec2 scale);
	void transform_rotate(float radians);
	void transform_translate(vec2 pos);
	void transform_end();

	virtual BoundingBox get_bounding_box();
	virtual void destroy();
};

enum Quadrant
{
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	NONE
};

class QuadTree 
{
	const int MAX_OBJECTS = 2;
	int m_size;
    BoundingBox m_boundingBox = BoundingBox(0, 0, 0, 0);
	std::vector<Renderable*> m_objects;
  
    // Children of this tree 
    QuadTree *m_topLeftTree; 
    QuadTree *m_topRightTree; 
    QuadTree *m_botLeftTree; 
    QuadTree *m_botRightTree; 
  
public: 
	QuadTree(BoundingBox b);

	int size();
	void clear();
	void split();
	Quadrant getQuadrant(BoundingBox boundingBox);
    void insert(Renderable* renderable); 
	void insertIntoSubtree(Quadrant quadrant, Renderable* renderable);
    std::vector<Renderable*> retrieve(BoundingBox boundingBox, std::vector<Renderable*> returnObjs);
}; 
  
enum AIType
{
	CHASE,
	AVOID,
	INIT,
	RANDOM
};

enum CrouchState
{
	NOT_CROUCHING,
	CROUCH_PRESSED,
	IS_CROUCHING,
	CROUCH_RELEASED
};


enum FighterMovementState
{
	MOVING_FORWARD = 0,
	MOVING_BACKWARD = 1,
	START_JUMPING = 2,
	CROUCHING = 3,
	PUNCHING = 4,
	STOP_MOVING_FORWARD = 5,
	STOP_MOVING_BACKWARD = 6,
	RELEASE_CROUCH = 7,
	STOP_PUNCHING = 8,
	BLOCKING = 9,
	STOP_BLOCKING = 10,
	ABILITY_1 = 11,
	ABILITY_2 = 12,
	HOLDING_ABILITY_2 = 13,
	CHARGED_ABILITY_2 = 14,
	HOLDING_POWER_PUNCH = 15,
	POWER_PUNCHING = 16,
	STOP_ABILITIES = 17,
	PAUSED = 18,
	UNPAUSED = 19
};


enum DeletionTime
{
	AFTER_UPDATE,
	AFTER_HIT,
	AFTER_TIME,
	AFTER_HIT_OR_TIME,
	AFTER_OUT_OF_BOUNDS
};

enum GameMode
{
	MENU = 0,
	CHARSELECT,
	STAGESELECT,
	FIGHTINTRO,
	PVC, // single player
	PVP, // 2 player
	TUTORIAL,
	DEV,
};

enum PauseMenuOption
{
	RESUME,
	MAINMENU,
	QUIT,
	RESTART
};

// For console log printing
static const char* ModeMap[] = { "MENU", "CHARSELECT", "STAGESELECT", "FIGHTINTRO", "PVC", "PVP", "TUTORIAL", "DEV" };
