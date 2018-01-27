#pragma once
#include "glm/glm.hpp"

using namespace glm;

class Triangle
{
public:
    vec2 a, b, c;

    Triangle();
    Triangle(vec2 a, vec2 b, vec2 c);

    float getHeight(float adj, float hyp);
    float length();
};

void triangleSplit(Triangle parent, Triangle* newA, Triangle* newB, Triangle* newC);
