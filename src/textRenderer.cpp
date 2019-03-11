#include "textRenderer.hpp"

TextRenderer::TextRenderer(std::string font_name, int size) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		printf("Could not init Freetype!");
	}

	FT_Face face;
	if (FT_New_Face(ft, fonts_path(font_name).c_str(), 0, &face)) {
		printf("Failed to load font!");
		exit(1);
	}

	FT_Set_Pixel_Sizes(face, 0, size);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			printf("ERROR::FREETYTPE: Failed to load Glyph");
			continue;
		}
		// Generate texture
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
		Character *character = new Character();
		character->textureId = texture;
		character->size = { static_cast<float>(face->glyph->bitmap.width),  static_cast<float>(face->glyph->bitmap.rows) };
		character->bearing = { static_cast<float>(face->glyph->bitmap_left),  static_cast<float>(face->glyph->bitmap_top) };
		character->advance = static_cast<GLuint>(face->glyph->advance.x);
		characters.insert(std::pair<GLchar, Character*>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_color = { 1.f, 0.f, 0.f };
	m_scale = { 1.f, -1.f };
	m_position = { 0.f, 0.f };
	if (!effect.load_from_file(shader_path("text.vs.glsl"), shader_path("text.fs.glsl")))
		printf("textRenderer text shaders failed!");
}

TextRenderer::~TextRenderer() {
	for (GLubyte c = 0; c < 128; c++) {
		Character *character = characters[c];
		glDeleteTextures(1, &(character->textureId));
		delete character;
	}
	characters.clear();
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDisableVertexAttribArray(0);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
	effect.release();
}

void TextRenderer::setColor(vec3 color) {
	m_color = color;
}
void TextRenderer::setPosition(vec2 position) {
	m_position = position;
}
void TextRenderer::setScale(vec2 scale) {
	m_scale = scale;
}
void TextRenderer::renderString(const mat3& projection, std::string text) {
	transform_begin();
	transform_translate(m_position);
	transform_scale(m_scale);
	transform_end();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setting shaders
	glUseProgram(effect.program);

	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);


	// Activate corresponding render state	
	glUniform3f(glGetUniformLocation(effect.program, "textColor"), m_color.x, m_color.y, m_color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	float x = 0;
	float y = 0;

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = *characters[*c];

		GLfloat xpos = x + ch.bearing.x;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y);

		GLfloat w = ch.size.x;
		GLfloat h = ch.size.y;
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
		glBindTexture(GL_TEXTURE_2D, ch.textureId);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6); // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void TextRenderer::draw(const mat3& projection) {
	// TODO
	return;
}

float TextRenderer::get_width_of_string(std::string text) {
	std::string::const_iterator c;
	float return_value = 0.f;
	Character ch;
	for (c = text.begin(); c != text.end(); c++) {
		ch = *characters[*c]; return_value += (ch.advance >> 6);
	}
	return return_value + ch.size.x;
}
