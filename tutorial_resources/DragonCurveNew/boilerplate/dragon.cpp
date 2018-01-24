#include "dragon.h"

#include "glm/gtc/type_ptr.hpp"
#include <iostream>

#define PI 3.14159

using namespace std;

vec2 Line::getPerpendicular(){
	vec2 line = b-a;
	return normalize(vec2(-line.y, line.x));
}

Line::Line():a(0), b(0)
{
}

Line::Line(vec2 a, vec2 b):a(a), b(b){}

float Line::length()
{
	return sqrt(dot(a-b, a-b));
}

void dragonCurveSplit(Line l, Line* newA, Line* newB)
{
	vec2 midpoint = 0.5f*l.a + 0.5f*l.b;

	midpoint = midpoint + 0.5f*l.length()*l.getPerpendicular();

	*newA = Line(l.a, midpoint);
	*newB = Line(l.b, midpoint);
}












