#pragma once

#include "common.hpp"
#include "boundingBox.hpp"
class Platform
{
public:
	Platform(int xpos, int ypos, int width, int height);
	BoundingBox m_bounding_box;
};