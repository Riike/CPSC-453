#pragma once

#include "glm/glm.hpp"

using namespace glm;

class Line{
public:
	vec2 a, b;

	Line();
	Line(vec2 a, vec2 b);

	vec2 getPerpendicular();
	float length();
};

void dragonCurveSplit(Line l, Line* newA, Line* newB);
