#include "boundingBox.hpp"

//BoundingBox::BoundingBox(int xpos, int ypos, int width, int height) {
BoundingBox::BoundingBox(float xpos, float ypos, float width, float height) {
	this->xpos = xpos;
	this->ypos = ypos;
	this->width = width;
	this->height = height;
}

bool BoundingBox::check_collision(BoundingBox b1) {
	return (b1.xpos < xpos + width &&
		b1.xpos + b1.width > xpos &&
		b1.ypos < ypos + height &&
		b1.ypos + b1.height > ypos);
}