#include "platform.hpp"

Platform::Platform(int xpos, int ypos, int width, int height) : m_bounding_box(xpos, ypos, width, height) {
}

bool Platform::check_collision(BoundingBox b) {
	return m_bounding_box.check_collision(b);
}