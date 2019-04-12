#include "common.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb_image/stb_image.h"

// stlib
#include <vector>
#include <iostream>
#include <sstream>

void gl_flush_errors()
{
	while (glGetError() != GL_NO_ERROR);
}

bool gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR) return false;

	while (error != GL_NO_ERROR)
	{
		const char* error_str = "";
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		fprintf(stderr, "OpenGL: %s", error_str);
		error = glGetError();
	}

	return true;
}

float dot(vec2 l, vec2 r)
{
	return l.x * r.x + l.y * r.y;
}

float dot(vec3 l, vec3 r)
{
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

mat3 mul(const mat3 & l, const mat3 & r)
{
	mat3 l_t = { { l.c0.x, l.c1.x, l.c2.x},
	{ l.c0.y, l.c1.y, l.c2.y } ,
	{ l.c0.z, l.c1.z, l.c2.z } };

	mat3 ret;
	ret.c0.x = dot(l_t.c0, r.c0);
	ret.c0.y = dot(l_t.c1, r.c0);
	ret.c0.z = dot(l_t.c2, r.c0);

	ret.c1.x = dot(l_t.c0, r.c1);
	ret.c1.y = dot(l_t.c1, r.c1);
	ret.c1.z = dot(l_t.c2, r.c1);

	ret.c2.x = dot(l_t.c0, r.c2);
	ret.c2.y = dot(l_t.c1, r.c2);
	ret.c2.z = dot(l_t.c2, r.c2);
	return ret;
}

vec2 normalize(vec2 v)
{
	float m = sqrtf(dot(v, v));
	return { v.x / m, v.y / m };
}

int get_random_number(int max) {
	std::random_device rd; // non-deterministic generator
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, max);
	return dist(gen);
}

Texture::Texture()
{

}

Texture::~Texture()
{
	if (id != 0) glDeleteTextures(1, &id);
	if (depth_render_buffer_id != 0) glDeleteRenderbuffers(1, &depth_render_buffer_id);
}

bool Texture::load_from_file(const char* path)
{
	if (path == nullptr) 
		return false;
	
	stbi_uc* data = stbi_load(path, &width, &height, NULL, 4);
	if (data == NULL)
		return false;

	gl_flush_errors();
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);
	return !gl_has_errors();
}

// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
bool Texture::create_from_screen(GLFWwindow const * const window) {
	gl_flush_errors();
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

    glfwGetFramebufferSize(const_cast<GLFWwindow *>(window), &width, &height);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Generate the render buffer with the depth buffer
	glGenRenderbuffers(1, &depth_render_buffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_render_buffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_render_buffer_id);

	// Set id as colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, id, 0);

	// Set the list of draw buffers.
	GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, draw_buffers); // "1" is the size of DrawBuffers

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	return !gl_has_errors();
}

bool Texture::is_valid()const
{
	return id != 0;
}

namespace
{
	bool gl_compile_shader(GLuint shader)
	{
		glCompileShader(shader);
		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint log_len;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetShaderInfoLog(shader, log_len, &log_len, log.data());
			glDeleteShader(shader);

			fprintf(stderr, "GLSL: %s", log.data());
			return false;
		}

		return true;
	}
}

bool Effect::load_from_file(const char* vs_path, const char* fs_path)
{
	gl_flush_errors();

	// Opening files
	std::ifstream vs_is(vs_path);
	std::ifstream fs_is(fs_path);

	if (!vs_is.good() || !fs_is.good())
	{
		fprintf(stderr, "Failed to load shader files %s, %s", vs_path, fs_path);
		return false;
	}

	// Reading sources
	std::stringstream vs_ss, fs_ss;
	vs_ss << vs_is.rdbuf();
	fs_ss << fs_is.rdbuf();
	std::string vs_str = vs_ss.str();
	std::string fs_str = fs_ss.str();
	const char* vs_src = vs_str.c_str();
	const char* fs_src = fs_str.c_str();
	GLsizei vs_len = (GLsizei)vs_str.size();
	GLsizei fs_len = (GLsizei)fs_str.size();

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs_src, &vs_len);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs_src, &fs_len);

	// Compiling
	// Shaders already delete if compilation fails
	if (!gl_compile_shader(vertex))
		return false;

	if (!gl_compile_shader(fragment))
	{
		glDeleteShader(vertex);
		return false;
	}

	// Linking
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	{
		GLint is_linked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint log_len;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetProgramInfoLog(program, log_len, &log_len, log.data());

			release();
			fprintf(stderr, "Link error: %s", log.data());
			return false;
		}
	}

	if (gl_has_errors())
	{
		release();
		fprintf(stderr, "OpenGL errors occured while compiling Effect");
		return false;
	}

	return true;
}

void Effect::release()
{
  glDetachShader(program, vertex);
  glDeleteShader(vertex);
  //
  glDetachShader(program, fragment);
  glDeleteShader(fragment);
  //
  glDeleteProgram(program);
}

std::string fonts_path(std::string name) {
	std::stringstream ss;

	ss << data_path << "/fonts/" << name;
	return ss.str();
}

void Renderable::transform_begin()
{
	transform = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} };
}

void Renderable::transform_scale(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	transform = mul(transform, S);
}

void Renderable::transform_rotate(float radians)
{
	float c = cosf(radians);
	float s = sinf(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	transform = mul(transform, R);
}

void Renderable::transform_translate(vec2 offset)
{
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x, offset.y, 1.f } };
	transform = mul(transform, T);
}

void Renderable::transform_end()
{
	//
}

BoundingBox Renderable::get_bounding_box()
{
	return BoundingBox(0, 0, 0, 0);
}

void Renderable::destroy()
{

}

int QuadTree::size()
{
	return m_size;
}

/*
 * Clears the quadtree
 */
void QuadTree::clear() 
{
	m_size = 0;
	for (auto* object : m_objects) {
		object->destroy();
		//delete object;
	}
	m_objects.clear();

	if (m_topLeftTree != NULL) {
		m_topLeftTree->clear();
		delete m_topLeftTree;
		m_topLeftTree = NULL;
	}

	if (m_topRightTree != NULL) {
		m_topRightTree->clear();
		delete m_topRightTree;
		m_topRightTree = NULL;
	}

	if (m_botLeftTree != NULL) {
		m_botLeftTree->clear();
		delete m_botLeftTree;
		m_botLeftTree = NULL;
	}

	if (m_botRightTree != NULL) {
		m_botRightTree->clear();
		delete m_botRightTree;
		m_botRightTree = NULL;
	}
}

QuadTree::QuadTree(BoundingBox b) {
	m_topLeftTree = NULL;
	m_topRightTree = NULL;
	m_botLeftTree = NULL;
	m_botRightTree = NULL;
	m_boundingBox = b;
	m_objects = {};
	m_size = 0;
}

/*
 * Splits the node into 4 subnodes
 */
void QuadTree::split() 
{
	int subWidth = (int)(m_boundingBox.width / 2);
	int subHeight = (int)(m_boundingBox.height / 2);
	int x = (int)m_boundingBox.xpos;
	int y = (int)m_boundingBox.ypos;

	m_topRightTree = new QuadTree(BoundingBox(x + subWidth, y, subWidth, subHeight));
	m_topLeftTree = new QuadTree(BoundingBox(x, y, subWidth, subHeight));
	m_botLeftTree = new QuadTree(BoundingBox(x, y + subHeight, subWidth, subHeight));
	m_botRightTree = new QuadTree(BoundingBox(x + subWidth, y + subHeight, subWidth, subHeight));
}

/*
 * Determine which node the object belongs to. -1 means
 * object cannot completely fit within a child node and is part
 * of the parent node
 */
Quadrant QuadTree::getQuadrant(BoundingBox boundingBox)
{
	float verticalMidpoint = m_boundingBox.xpos + (m_boundingBox.width / 2);
	float horizontalMidpoint = m_boundingBox.ypos + (m_boundingBox.height / 2);

	// Object can completely fit within the top quadrants
	bool topQuadrant = (boundingBox.ypos < horizontalMidpoint && boundingBox.ypos + boundingBox.height < horizontalMidpoint);
	// Object can completely fit within the bottom quadrants
	bool bottomQuadrant = (boundingBox.ypos > horizontalMidpoint);

	// Object can completely fit within the left quadrants
	if (boundingBox.xpos < verticalMidpoint && boundingBox.xpos + boundingBox.width < verticalMidpoint) {
		if (topQuadrant) {
			return TOP_LEFT;
		}
		else if (bottomQuadrant) {
			return BOTTOM_LEFT;
		}
	}
	// Object can completely fit within the right quadrants
	else if (boundingBox.xpos > verticalMidpoint) {
		if (topQuadrant) {
			return TOP_RIGHT;
		}
		else if (bottomQuadrant) {
			return BOTTOM_RIGHT;
		}
	}
 
	return NONE;
}

void QuadTree::insert(Renderable* renderable) 
{
	m_size++;
	if (m_topLeftTree != NULL) {
		Quadrant quadrant = getQuadrant(renderable->get_bounding_box());

		if (quadrant != NONE) {
			insertIntoSubtree(quadrant, renderable);
			return;
		}
	}

	this->m_objects.push_back(renderable);

	if (this->m_objects.size() > MAX_OBJECTS) {
		if (this->m_botLeftTree == NULL) {
			split();
		}

		int i = 0;

		while (i < m_objects.size()) {
			BoundingBox b = m_objects[i]->get_bounding_box();
			Quadrant quadrant = getQuadrant(b);
			if (quadrant != NONE) {
				Renderable* r = m_objects[i];
				insertIntoSubtree(quadrant, r);
				m_objects.erase(m_objects.begin() + i);
			} else {
				i++;
			}
		}
	}
}

void QuadTree::insertIntoSubtree(Quadrant quadrant, Renderable* renderable) 
{
	switch(quadrant) {
		case TOP_LEFT:
			m_topLeftTree->insert(renderable);
			break;
		case TOP_RIGHT:
			m_topRightTree->insert(renderable);
			break;
		case BOTTOM_LEFT:
			m_botLeftTree->insert(renderable);
			break;
		case BOTTOM_RIGHT:
			m_botRightTree->insert(renderable);
			break;
	}
}

/*
 * Return all objects that could collide with the given BoundingBox
 */
std::vector<Renderable*> QuadTree::retrieve(BoundingBox boundingBox, std::vector<Renderable*> returnObjs) 
{
	Quadrant quadrant = getQuadrant(boundingBox);
	if (quadrant != NONE && m_botLeftTree != NULL) {
		std::vector<Renderable*> platforms;
		switch(quadrant) {
			case TOP_LEFT:
				platforms = m_topLeftTree->retrieve(boundingBox, {});
				returnObjs.insert(returnObjs.end(), platforms.begin(), platforms.end());
				break;
			case TOP_RIGHT:
				platforms = m_topRightTree->retrieve(boundingBox, {});
				returnObjs.insert(returnObjs.end(), platforms.begin(), platforms.end());
				break;
			case BOTTOM_LEFT:
				platforms = m_botLeftTree->retrieve(boundingBox, {});
				returnObjs.insert(returnObjs.end(), platforms.begin(), platforms.end());
				break;
			case BOTTOM_RIGHT:
				platforms = m_botRightTree->retrieve(boundingBox, {});
				returnObjs.insert(returnObjs.end(), platforms.begin(), platforms.end());
				break;
			}
	}

	returnObjs.insert(returnObjs.end(), m_objects.begin(), m_objects.end());

	return returnObjs;
}
