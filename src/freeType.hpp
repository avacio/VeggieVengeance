#pragma once

#include "common.hpp" // TODO

#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL_pixels.h>

#include <iostream>
#include <cmath>
#include <vector>
#include <map>

#include "project_path.hpp"
//#define fonts_path(name) PROJECT_SOURCE_DIR "./data/fonts/" name // TODO

using namespace std;

typedef struct {
	unsigned char* buffer;
	unsigned int width;
	unsigned int height;
	float bearing_x;
	float bearing_y;
} Glyph;

struct Character {
	GLuint id;
	unsigned int width;
	unsigned int height;
	float bearing_x;
	float bearing_y;
	GLuint advanceOffset;
};

class FreeType {
//class FreeType : Renderable {
public:
	FreeType();
	~FreeType();

	bool setUp();
	FT_Face* loadFace(const string& fontName, int ptSize, int deviceHDPI, int deviceVDPI);
	void freeFace(FT_Face* face);
	void freeGlyph(Glyph* glyph);
	//Glyph* rasterize(FT_Face* face, uint32_t glyphIndex) const;
	//void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, SDL_Color color);
	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale);



private:
	FT_Library lib;
	std::map<GLchar, Character> characters;
	Effect effect;
	int force_ucs2_charmap(FT_Face ftf);
};

//class Text : Renderable {
//public:
//	Text(const string& fontFIle, FreeType* lib);
//	~Text();
//
//	//void init();
//	//vector<gl::Mesh*> drawText(Text& text, float x, float y);
//	//void drawText(const mat3& projection, Text& text, float x, float y);
//
//	//void init(Text& text, float x, float y);
//	void draw(const mat3& projection);
//
//private:
//	FreeType* lib;
//	FT_Face* face;
//
//};




//class Text : Renderable {
//public:
//	// Creates all the associated render resources and default transform
//	bool init();
//
//	// Releases all the associated resources
//	void destroy();
//};
////void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);

