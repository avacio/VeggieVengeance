#pragma once

#include "common.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H


#include <iostream>
#include <cmath>
#include <vector>
#include <map>


struct Character {
	GLuint textureId;
	vec2 size;
	vec2 bearing;
	GLuint advance;
};

class TextRenderer : public Renderable {
public:
	TextRenderer(std::string font_name, int size);
	~TextRenderer();
	void setColor(vec3 color);
	void setPosition(vec2 position);
	void setScale(vec2 scale);
	void renderString(const mat3& projection, std::string text);
	void draw(const mat3& projection);
	float get_width_of_string(std::string text);


private:
	std::map<GLchar, Character> characters;
	vec3 m_color;

	GLuint VAO;
	GLuint VBO;

	vec2 m_position;
	vec2 m_scale;
};
