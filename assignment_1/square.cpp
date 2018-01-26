#include "square.h"

#include "glm/gtc/type_ptr.hpp"
#include <iostream>

using namespace std;

Square::Square():a(0), b(0), c(0), d(0){}

Square::Square(float length)
{
    a = vec2(length * -1);
    b = vec2(length * -1 , length);
    c = vec2(length);
    d = vec2(length, length * -1);
}
