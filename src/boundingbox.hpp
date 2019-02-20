#pragma once

#include "common.hpp"

class BoundingBox
{
public:
	BoundingBox(int xpos, int ypos, int width, int height);
	int xpos;
	int ypos;
	int width;
	int height;
};