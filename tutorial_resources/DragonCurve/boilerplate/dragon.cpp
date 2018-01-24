#include "dragon.h"

using namespace glm;
using namespace std;

Line::Line():a(0), b(0){}

Line::Line(vec2 a, vec2 b):a(a), b(b){}

float Line::length(){
	return sqrt(dot(a-b, a-b));
}

vec2 Line::getPerpendicular(){
	vec2 line = b-a;
	return normalize(vec2(-line.y, line.x));
}

void dragonCurveSplit(Line l, Line* newA, Line* newB){
	vec2 midpoint = 0.5f*(l.a + l.b);
	vec2 newPoint = midpoint + 0.5f*l.length()*l.getPerpendicular();

	*newA = Line(l.a, newPoint);
	*newB = Line(l.b, newPoint);
}
