#pragma once

class BoundingBox
{
public:
	BoundingBox(float xpos, float ypos, float width, float height);

	bool check_collision(BoundingBox b1);

	float xpos;
	float ypos;
	float width;
	float height;
};