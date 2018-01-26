#include "diamond.h"

#include "glm/gtc/type_ptr.hpp"
#include <iostream>

using namespace std;

Diamond::Diamond():a(0), b(0), c(0), d(0){}

Diamond::Diamond(float length)
{
    a = vec2(length * -1, 0);
    b = vec2(0, length);
    c = vec2(length, 0);
    d = vec2(0, length * -1);
}
