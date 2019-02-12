// internal
#include "common.hpp"
#include "world.hpp"
#include "freeType.hpp"

#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>

#include <SDL.h>
#include <map>
#include <vector>

using Clock = std::chrono::high_resolution_clock;

// Global 
World world;
const int width = 1200;
const int height = 800;
const char* title = "VEGGIE VENGEANCE";


std::map<GLchar, Character> textCharacters;
GLuint VAO, VBO;

// Entry point
int main(int argc, char* argv[])
{
	// Initializing world (after renderer.init().. sorry)
	if (!world.init({ (float)width, (float)height }))
	{
		// Time to read the error message
		std::cout << "Press any key to exit" << std::endl;
		std::cin.get();
		return EXIT_FAILURE;
	}

	auto t = Clock::now();

	FreeType ft;
	ft.setUp();

	// variable timestep loop.. can be improved (:
	while (!world.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		////////
		//vector<int> col{ 0,0,0 }; 
		//glm::vec3()
		//SDL_Color color = { 0,0,0 };
		ft.renderText("SOSOSOSOS", 25.0f, 25.0f, 1.0f);

		world.update(elapsed_sec);
		world.draw();
	}

	world.destroy();

	return EXIT_SUCCESS;
}

//void renderText(Effect &effect, std::string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color) {
//
//	glUseProgram(effect.program);
//
//	glUniform3f(glGetUniformLocation(effect.program, "textColor"), color.x, color.y, color.z);
//	glActiveTexture(GL_TEXTURE0);
//	glBindVertexArray(VAO);
//
//	// Iterate through all characters
//	std::string::const_iterator c;
//	for (c = text.begin(); c != text.end(); c++)
//	{
//		Character ch = textCharacters[*c];
//
//		GLfloat xpos = x + ch.bearing_x * scale;
//		GLfloat ypos = y - (ch.height - ch.bearing_y) * scale;
//
//		GLfloat w = ch.width * scale;
//		GLfloat h = ch.height * scale;
//		// Update VBO for each character
//		GLfloat vertices[6][4] = {
//			{ xpos,     ypos + h,   0.0, 0.0 },
//		{ xpos,     ypos,       0.0, 1.0 },
//		{ xpos + w, ypos,       1.0, 1.0 },
//
//		{ xpos,     ypos + h,   0.0, 0.0 },
//		{ xpos + w, ypos,       1.0, 1.0 },
//		{ xpos + w, ypos + h,   1.0, 0.0 }
//		};
//		// Render glyph texture over quad
//		glBindTexture(GL_TEXTURE_2D, ch.id);
//		// Update content of VBO memory
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
//
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		// Render quad
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//		x += (ch.advanceOffset >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
//	}
//	glBindVertexArray(0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//}