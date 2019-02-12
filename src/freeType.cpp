#include "freeType.hpp"
#include <cassert>
#include <map> // TODO
#include "common.hpp"

FreeType::FreeType() {
	FT_Init_FreeType(&lib);
}

FreeType::~FreeType() {
	//FT_Done_FreeType(lib);
}

bool FreeType::setUp() {
	FT_Face face;
	if (FT_New_Face(lib, fonts_path("Xoxoxa.ttf"), 0, &face))
		fprintf(stderr, "Failed to load font!");

	FT_Set_Pixel_Sizes(face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	characters = std::map<GLchar, Character> ();

	// load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			fprintf(stderr, "Failed to load glyph!");
			continue;
		}
		  GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
            face->glyph->bitmap_left,
			face->glyph->bitmap_top,
            face->glyph->advance.x
        };
        characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(lib);

	// Configure VAO/VBO for texture quads
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	/////////////////////
	// Clearing errors
	gl_flush_errors();

	//// Vertex Buffer creation
	//glGenBuffers(1, &mesh.vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	//// Index Buffer creation
	//glGenBuffers(1, &mesh.ibo);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	//glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("text.vs.glsl"), shader_path("text.fs.glsl")))
		return false;


	return true;	//// Setting initial values, scale is negative to make it face the opposite way
	//// 1.0 would be as big as the original texture

	//m_scale.x = 0.1725f;
	//m_scale.y = 0.1725f;
	//m_rotation = 0.f;
	//m_position = { 595.f, 455.f };

}

// need right now?
FT_Face* FreeType::loadFace(const string& fontName, int ptSize, int deviceHDPI, int deviceVDPI) {
	FT_Face* face = new FT_Face;

	FT_Error error = FT_New_Face(lib, fontName.c_str(), 0, face);
	assert(error == 0 && "Cannot open font file.");

	force_ucs2_charmap(*face);
	FT_Set_Char_Size(*face, 0, ptSize, deviceHDPI, deviceVDPI);

	return face;
}

void FreeType::freeFace(FT_Face* face) {
	FT_Done_Face(*face);
	delete face;
}

void FreeType::freeGlyph(Glyph* glyph) {
	delete glyph;
}

//Glyph* FreeType::rasterize(FT_Face* face, uint32_t glyphIndex) const {
//	Glyph* g = new Glyph;
//	FT_Int32 flags = FT_LOAD_DEFAULT;
//
//	FT_Load_Glyph(*face, glyphIndex, flags);
//	FT_GlyphSlot slot = (*face)->glyph;
//	FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
//
//	FT_Bitmap ftBitmap = slot->bitmap;
//
//	g->buffer = ftBitmap.buffer;
//	g->width = ftBitmap.width;
//	g->height = ftBitmap.rows;
//	g->bearing_x = slot->bitmap_left;
//	g->bearing_y = slot->bitmap_top;
//
//	return g;
//}

int FreeType::force_ucs2_charmap(FT_Face ftf) {
	for (int i = 0; i < ftf->num_charmaps; i++) {
		if ((ftf->charmaps[i]->platform_id == 0 && ftf->charmaps[i]->encoding_id == 3)
			|| (ftf->charmaps[i]->platform_id == 3 && ftf->charmaps[i]->encoding_id == 1)) {
			return FT_Set_Charmap(ftf, ftf->charmaps[i]);
		}
	}
	return -1;
}

//////////////////////////////
//Text::Text(const string& fontFile, FreeType* fontLib) {
//	lib = fontLib;
//	float size = 50;
//	face = lib->loadFace(fontFile, size * 64, 72, 72);
//}
//
//Text::~Text() {
//	lib->freeFace(face);
//	//TODO
//}
//
//
//void Text::draw(const mat3& projection) {
//
//	// Enabling alpha channel for textures
//	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glEnable(GL_DEPTH_TEST);
//
//	// Draw the screen texture on the quad geometry
//	// Setting vertices
//	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
//
//	// Bind to attribute 0 (in_position) as in the vertex shader
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//
//	// Draw
//	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
//	glDisableVertexAttribArray(0);
//}

/////////////////////////////
//std::map<GLchar, Character> textCharacters;
//GLuint VAO, VBO;
//
//void renderText(Effect &effect, std::string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color) {
//void FreeType::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, SDL_Color color) {
void FreeType::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale) {


	glUseProgram(effect.program);
	//fprintf(stderr, "IN RENDERTEXT!");


	//glUniform3f(glGetUniformLocation(effect.program, "textColor"), color.r, color.g, color.b);
	glActiveTexture(GL_TEXTURE0);
	//glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characters[*c];

		GLfloat xpos = x + ch.bearing_x * scale;
		GLfloat ypos = y - (ch.height - ch.bearing_y) * scale;

		GLfloat w = ch.width * scale;
		GLfloat h = ch.height * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos,     ypos,       0.0, 1.0 },
		{ xpos + w, ypos,       1.0, 1.0 },

		{ xpos,     ypos + h,   0.0, 0.0 },
		{ xpos + w, ypos,       1.0, 1.0 },
		{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.id);
		// Update content of VBO memory
		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advanceOffset >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//
//bool Text::init()
//{
//	//// Load shared texture
//	//if (!bg_texture.is_valid())
//	//{
//	//	if (!bg_texture.load_from_file(fonts_path("Xoxoxa.ttf")))
//	//	{
//	//		fprintf(stderr, "Failed to load font!");
//	//		return false;
//	//	}
//	//}
//
//	// The position corresponds to the center of the texture
//	float wr = bg_texture.width * 3.5f;
//	float hr = bg_texture.height * 3.5f;
//
//	TexturedVertex vertices[4];
//	vertices[0].position = { -wr, +hr, -0.02f };
//	vertices[0].texcoord = { 0.f, 1.f };
//	vertices[1].position = { +wr, +hr, -0.02f };
//	vertices[1].texcoord = { 1.f, 1.f };
//	vertices[2].position = { +wr, -hr, -0.02f };
//	vertices[2].texcoord = { 1.f, 0.f };
//	vertices[3].position = { -wr, -hr, -0.02f };
//	vertices[3].texcoord = { 0.f, 0.f };
//
//	// counterclockwise as it's the default opengl front winding direction
//	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };
//
//	// Clearing errors
//	gl_flush_errors();
//
//	// Vertex Buffer creation
//	glGenBuffers(1, &mesh.vbo);
//	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);
//
//	// Index Buffer creation
//	glGenBuffers(1, &mesh.ibo);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);
//
//	// Vertex Array (Container for Vertex + Index buffer)
//	glGenVertexArrays(1, &mesh.vao);
//	if (gl_has_errors())
//		return false;
//
//	// Loading shaders
//	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
//		return false;
//
//	// Setting initial values, scale is negative to make it face the opposite way
//	// 1.0 would be as big as the original texture
//
//	m_scale.x = 0.1725f;
//	m_scale.y = 0.1725f;
//	m_rotation = 0.f;
//	m_position = { 595.f, 455.f };
//
//	return true;
//}

//// Call if init() was successful
//// Releases all graphics resources
//void Text::destroy()
//{
//	glDeleteBuffers(1, &mesh.vbo);
//	glDeleteBuffers(1, &mesh.ibo);
//	glDeleteBuffers(1, &mesh.vao);
//
//	glDeleteShader(effect.vertex);
//	glDeleteShader(effect.fragment);
//	glDeleteShader(effect.program);
//}