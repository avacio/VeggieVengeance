#pragma once

#include "common.hpp"

class BoundingBox
{
public:
	BoundingBox(int xpos, int ypos, int width, int height);

	bool check_collision(BoundingBox b1);

	int xpos;
	int ypos;
	int width;
	int height;
};