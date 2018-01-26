#pragma once
#include "glm/glm.hpp"

using namespace glm;

class Diamond
{
public:
    vec2 a, b, c, d;

    Diamond();
    Diamond(float length);
};
