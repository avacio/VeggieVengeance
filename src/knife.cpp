// Header
#include "knife.hpp"

// internal


// stlib
#include <vector>
#include <string>
#include <algorithm>

float knifeOffset = 75.f;
float dropSpeed = 5.f;
void Knife::spawn_knife(unsigned int damage, vec2 pos) {
	//pre-determined bullet attributes
	this->m_scale = vec2({ 0.5f, 0.5f });
	this->m_velocity = vec2({ 7.0f, 0.0f });

	//variable bullet attributes
	//this->m_fighter_id = id;
	this->m_position = pos;
	this->m_damage = damage;
	this->m_width = std::fabs(this->m_scale.x);
	this->m_height = std::fabs(this->m_scale.y);
	this->m_delete_when = AFTER_HIT;
	this->m_damageEffect = new DamageEffect(this->m_position.x, this->m_position.y, this->m_width, this->m_height, this->m_damage, this->m_fighter_id, this->m_delete_when, 0);

	boundingBox = new BoundingBox(pos.x, pos.y+knifeOffset, m_width*0.6f, m_height*1.5f); // stub
	m_position = pos;
	init();
}

//bool Knife::init(unsigned int damage, vec2 pos)
bool Knife::init()
{
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;

	// Reads the salmon mesh from a file, which contains a list of vertices and indices
	FILE* mesh_file = fopen(mesh_path("knife.mesh"), "r");
	if (mesh_file == nullptr)
		return false;

	// Reading vertices and colors
	size_t num_vertices;
	fscanf(mesh_file, "%zu\n", &num_vertices);
	for (size_t i = 0; i < num_vertices; ++i)
	{
		float x, y, z;
		//float _n[3]; // unused
		int r, g, b;
		int s, t;
		//fscanf(mesh_file, "%f %f %f %f %f %f %f %f %d %d %d\n", &x, &y, &z, _n, _n + 1, _n + 2, &s, &t, &r, &g, &b);
		fscanf(mesh_file, "%f %f %f %f %f %d %d %d\n", &x, &y, &z, &s, &t, &r, &g, &b);
		Vertex vertex;
		vertex.position = { x, -y, -z };
		vertex.color = { (float)r / 255, (float)g / 255, (float)b / 255 };
		vertices.push_back(vertex);
	}

	// Reading associated indices
	size_t num_indices;
	fscanf(mesh_file, "%zu\n", &num_indices);
	for (size_t i = 0; i < num_indices; ++i)
	{
		int idx[3];
		fscanf(mesh_file, "%d %d %d\n", idx, idx + 1, idx + 2);
		indices.push_back((uint16_t)idx[0]);
		indices.push_back((uint16_t)idx[1]);
		indices.push_back((uint16_t)idx[2]);
	}

	// Done reading
	fclose(mesh_file);

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("colored.vs.glsl"), shader_path("colored.fs.glsl")))
		return false;

	// Setting initial values
	m_scale.x = 100.f;
	m_scale.y = 100.f;
	m_num_indices = indices.size();
	m_rotation = 0.f;

	return true;
}

// Releases all graphics resources
void Knife::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
	effect.release();
}

// Called on each frame by World::update()
void Knife::update(float ms)
{
	boundingBox->xpos = m_position.x;
	boundingBox->ypos = m_position.y + knifeOffset;

	if (!m_is_on_ground) {
		m_position.y += dropSpeed;
	}
	if (m_done) {
		m_position.y -= dropSpeed*2.f;
	}
}

void Knife::draw(const mat3& projection)
{
	transform_begin();
	transform_translate(m_position);
	transform_scale(m_scale);
	transform_rotate(m_rotation);

	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_color_loc = glGetAttribLocation(effect.program, "in_color");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);

	// !!! Salmon Color
	float color[] = { 1.f, 1.f, 1.f };

	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, (GLsizei)m_num_indices, GL_UNSIGNED_SHORT, nullptr);
}


vec2 Knife::get_position()const
{
	return m_position;
}

void Knife::set_rotation(float radians)
{
	m_rotation = radians;
}

// Simple bounding box collision check, 
bool Knife::collides_with(const Fighter& f)
{
	float dx = m_position.x - f.get_position().x;
	float dy = m_position.y - f.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(f.get_bounding_box()->xpos, f.get_bounding_box()->ypos);
	float my_r = std::max(m_scale.x, m_scale.y);
	float r = std::max(other_r, my_r);
	//r *= 0.6f;
	r *= 0.15f;
	if (d_sq < r * r)
		return true;
	return false;
}